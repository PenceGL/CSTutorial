// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SideScrollingCPP : ModuleRules
{
	public SideScrollingCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"StateTreeModule",
			"GameplayStateTreeModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] {
			"SideScrollingCPP",
			"SideScrollingCPP/NPC",
			"SideScrollingCPP/Gameplay"
		});
	}
}
