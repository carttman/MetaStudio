// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MetaStudios : ModuleRules
{
	public MetaStudios(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam", "WebSockets", "HTTP", "Json", "JsonUtilities", "MediaAssets", "Slate", "SlateCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
