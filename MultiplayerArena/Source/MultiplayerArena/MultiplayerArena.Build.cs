// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerArena : ModuleRules
{
	public MultiplayerArena(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MultiplayerArena",
			"MultiplayerArena/Variant_Platforming",
			"MultiplayerArena/Variant_Platforming/Animation",
			"MultiplayerArena/Variant_Combat",
			"MultiplayerArena/Variant_Combat/AI",
			"MultiplayerArena/Variant_Combat/Animation",
			"MultiplayerArena/Variant_Combat/Gameplay",
			"MultiplayerArena/Variant_Combat/Interfaces",
			"MultiplayerArena/Variant_Combat/UI",
			"MultiplayerArena/Variant_SideScrolling",
			"MultiplayerArena/Variant_SideScrolling/AI",
			"MultiplayerArena/Variant_SideScrolling/Gameplay",
			"MultiplayerArena/Variant_SideScrolling/Interfaces",
			"MultiplayerArena/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
