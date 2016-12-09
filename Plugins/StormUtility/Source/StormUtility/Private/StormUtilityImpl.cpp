// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "StormUtilityPrivatePCH.h"

#include "Engine.h"
#include "CsvParser.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "vmprotectsdk.h"

namespace DataTablePlugin
{
	TArray<UProperty*> GetTablePropertyArray(UDataTable* This, const TArray<const TCHAR*>& Cells, UStruct* InRowStruct, TArray<FString>& OutProblems)
	{
		VMProtectBegin("GetTablePropertyArray");

		TArray<UProperty*> ColumnProps;

		// Get list of all expected properties from the struct
		TArray<FName> ExpectedPropNames = DataTableUtils::GetStructPropertyNames(InRowStruct);

		// Need at least 2 columns, first column is skipped, will contain row names
		if (Cells.Num() > 1)
		{
			ColumnProps.AddZeroed(Cells.Num());

			// first element always NULL - as first column is row names

			for (int32 ColIdx = 1; ColIdx < Cells.Num(); ++ColIdx)
			{
				const TCHAR* ColumnValue = Cells[ColIdx];

				FName PropName = DataTableUtils::MakeValidName(ColumnValue);
				if (PropName == NAME_None)
				{
					OutProblems.Add(FString::Printf(TEXT("Missing name for column %d."), ColIdx));
				}
				else
				{
					UProperty* ColumnProp = FindField<UProperty>(InRowStruct, PropName);

					for (TFieldIterator<UProperty> It(InRowStruct); It && !ColumnProp; ++It)
					{
						const auto DisplayName = DataTableUtils::GetPropertyDisplayName(*It, FString());
						ColumnProp = (!DisplayName.IsEmpty() && DisplayName == ColumnValue) ? *It : NULL;
					}

					// Didn't find a property with this name, problem..
					if (ColumnProp == NULL)
					{
						OutProblems.Add(FString::Printf(TEXT("Cannot find Property for column '%s' in struct '%s'."), *PropName.ToString(), *InRowStruct->GetName()));
					}
					// Found one!
					else
					{
						// Check we don't have this property already
						if (ColumnProps.Contains(ColumnProp))
						{
							OutProblems.Add(FString::Printf(TEXT("Duplicate column '%s'."), *ColumnProp->GetName()));
						}
						// Check we support this property type
						else if (!DataTableUtils::IsSupportedTableProperty(ColumnProp))
						{
							OutProblems.Add(FString::Printf(TEXT("Unsupported Property type for struct member '%s'."), *ColumnProp->GetName()));
						}
						// Looks good, add to array
						else
						{
							ColumnProps[ColIdx] = ColumnProp;
						}

						// Track that we found this one
						ExpectedPropNames.Remove(ColumnProp->GetFName());
					}
				}
			}
		}

		// Generate warning for any properties in struct we are not filling in
		for (int32 PropIdx = 0; PropIdx < ExpectedPropNames.Num(); PropIdx++)
		{
			const UProperty* const ColumnProp = FindField<UProperty>(InRowStruct, ExpectedPropNames[PropIdx]);
			const FString DisplayName = DataTableUtils::GetPropertyDisplayName(ColumnProp, ExpectedPropNames[PropIdx].ToString());
			OutProblems.Add(FString::Printf(TEXT("Expected column '%s' not found in input."), *DisplayName));
		}

		VMProtectEnd();

		return ColumnProps;
	}

	class FDataTableImporterCSV
	{
	public:
		FDataTableImporterCSV(UDataTable& InDataTable, FString InCSVData, TArray<FString>& OutProblems)
			: DataTable(&InDataTable)
			, CSVData(MoveTemp(InCSVData))
			, ImportProblems(OutProblems)
		{
		}

		~FDataTableImporterCSV()
		{
		}

		bool FDataTableImporterCSV::ReadTable()
		{
			if (CSVData.IsEmpty())
			{
				ImportProblems.Add(TEXT("Input data is empty."));
				return false;
			}

			// Check we have a RowStruct specified
			if (!DataTable->RowStruct)
			{
				ImportProblems.Add(TEXT("No RowStruct specified."));
				return false;
			}

			const FCsvParser Parser(CSVData);
			const auto& Rows = Parser.GetRows();

			// Must have at least 2 rows (column names + data)
			if (Rows.Num() <= 1)
			{
				ImportProblems.Add(TEXT("Too few rows."));
				return false;
			}

			// Find property for each column
			TArray<UProperty*> ColumnProps = GetTablePropertyArray(DataTable, Rows[0], DataTable->RowStruct, ImportProblems);

			// Empty existing data
			DataTable->EmptyTable();

			// Iterate over rows
			for (int32 RowIdx = 1; RowIdx < Rows.Num(); RowIdx++)
			{
				const TArray<const TCHAR*>& Cells = Rows[RowIdx];

				// Need at least 1 cells (row name)
				if (Cells.Num() < 1)
				{
					ImportProblems.Add(FString::Printf(TEXT("Row '%d' has too few cells."), RowIdx));
					continue;
				}

				// Need enough columns in the properties!
				if (ColumnProps.Num() < Cells.Num())
				{
					ImportProblems.Add(FString::Printf(TEXT("Row '%d' has more cells than properties, is there a malformed string?"), RowIdx));
					continue;
				}

				// Get row name
				FName RowName = DataTableUtils::MakeValidName(Cells[0]);

				// Check its not 'none'
				if (RowName == NAME_None)
				{
					ImportProblems.Add(FString::Printf(TEXT("Row '%d' missing a name."), RowIdx));
					continue;
				}

				// Check its not a duplicate
				if (DataTable->RowMap.Find(RowName) != NULL)
				{
					ImportProblems.Add(FString::Printf(TEXT("Duplicate row name '%s'."), *RowName.ToString()));
					continue;
				}

				// Allocate data to store information, using UScriptStruct to know its size
				uint8* RowData = (uint8*)FMemory::Malloc(DataTable->RowStruct->PropertiesSize);
				DataTable->RowStruct->InitializeStruct(RowData);
				// And be sure to call DestroyScriptStruct later

				if (auto UDStruct = Cast<const UUserDefinedStruct>(DataTable->RowStruct))
				{
					UDStruct->InitializeStruct(RowData);
				}

				// Add to row map
				DataTable->RowMap.Add(RowName, RowData);

				// Now iterate over cells (skipping first cell, that was row name)
				for (int32 CellIdx = 1; CellIdx < Cells.Num(); CellIdx++)
				{
					// Try and assign string to data using the column property
					UProperty* ColumnProp = ColumnProps[CellIdx];
					const FString CellValue = Cells[CellIdx];
					FString Error = DataTableUtils::AssignStringToProperty(CellValue, ColumnProp, RowData);

					// If we failed, output a problem string
					if (Error.Len() > 0)
					{
						FString ColumnName = (ColumnProp != NULL)
							? DataTableUtils::GetPropertyDisplayName(ColumnProp, ColumnProp->GetName())
							: FString(TEXT("NONE"));
						ImportProblems.Add(FString::Printf(TEXT("Problem assigning string '%s' to property '%s' on row '%s' : %s"), *CellValue, *ColumnName, *RowName.ToString(), *Error));
					}
				}

				// Problem if we didn't have enough cells on this row
				if (Cells.Num() < ColumnProps.Num())
				{
					ImportProblems.Add(FString::Printf(TEXT("Too few cells on row '%s'."), *RowName.ToString()));
				}
			}

			DataTable->Modify(true);

			return true;
		}

	private:
		UDataTable* DataTable;
		FString CSVData;
		TArray<FString>& ImportProblems;
	};
};

TArray<FString> FStormUtility::CreateTableFromCSVString(UDataTable* DataTable, const FString& RawData)
{

	TArray<FString> OutProblems;

	if (DataTable != NULL)
	{
		DataTablePlugin::FDataTableImporterCSV(*DataTable, RawData, OutProblems).ReadTable();
	}

	return OutProblems;
}