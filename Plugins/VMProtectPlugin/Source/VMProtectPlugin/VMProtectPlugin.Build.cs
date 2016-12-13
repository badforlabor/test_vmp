// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VMProtectPlugin : ModuleRules
{
	public VMProtectPlugin(TargetInfo Target)
	{
        string vmDir = ModuleDirectory + "/DLL";

        PublicSystemIncludePaths.Add(vmDir);
        PublicAdditionalLibraries.Add(vmDir + "/VMProtectSDK64.lib");
        PublicLibraryPaths.Add(vmDir + "");

        PublicIncludePaths.AddRange(
			new string[] {
				"VMProtectPlugin/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"VMProtectPlugin/Private",
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
