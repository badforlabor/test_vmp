// Fill out your copyright notice in the Description page of Project Settings.

#include "test_vmp.h"
#include "VMPActorComponent.h"

#include "StormUtility.h"

#include <windows.h>
#include "vmprotectsdk.h"

// Sets default values for this component's properties
UVMPActorComponent::UVMPActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVMPActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	VMProtectBegin("UVMPActorComponent::BeginPlay");
	VMPCode = TEXT("1234");
	VMProtectEnd();

	FStormUtility::CreateTableFromCSVString(NULL, TEXT(""));
}


// Called every frame
void UVMPActorComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

