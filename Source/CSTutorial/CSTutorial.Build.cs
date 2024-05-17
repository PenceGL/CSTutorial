// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CSTutorial : ModuleRules
{
	public CSTutorial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"SlateCore",
			"Slate",
			"UMG",
			"EnhancedInput"
		});
	}
}