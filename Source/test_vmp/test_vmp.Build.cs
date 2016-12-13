// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;
using System;

public class test_vmp : ModuleRules
{
	public test_vmp(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "StormUtility", "VMProtectPlugin" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

//         string vmDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "Plugins", "VMProtect2"));
// 
//         PublicSystemIncludePaths.Add(vmDir);
//         PublicAdditionalLibraries.Add(vmDir + "/VMProtectSDK64.lib");
//         PublicLibraryPaths.Add(vmDir + "");


        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }
    }
}
