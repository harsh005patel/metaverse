// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DroneGameEditorTarget : TargetRules
{
	public DroneGameEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		bOverrideBuildEnvironment = true;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		// UE 5.1 trips over newer MSVC toolchains when probing ASAN support on Windows.
		GlobalDefinitions.Add("PLATFORM_HAS_ASAN_INCLUDE=0");
		ExtraModuleNames.Add("DroneGame");
	}
}
