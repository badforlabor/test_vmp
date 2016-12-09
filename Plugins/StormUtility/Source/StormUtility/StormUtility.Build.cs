// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StormUtility : ModuleRules
{
	public StormUtility(TargetInfo Target)
	{

        string vmDir = UEBuildConfiguration.UEThirdPartySourceDirectory + "/VMProtect";

        PublicSystemIncludePaths.Add(vmDir);
        PublicAdditionalLibraries.Add(vmDir + "/VMProtectSDK64.lib");
        PublicLibraryPaths.Add(vmDir + "");

        PublicIncludePaths.AddRange(
			new string[] {
				"StormUtility/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"StormUtility/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
