// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CartPusher : ModuleRules
{
	public CartPusher(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
