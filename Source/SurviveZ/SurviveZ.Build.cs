// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SurviveZ : ModuleRules
{
	public SurviveZ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","EnhancedInput","Niagara","UMG","PhysicsCore","NavigationSystem","AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.Add(ModuleDirectory);
		PublicIncludePaths.AddRange(new string[] { "/Project/SurviveZ/Source/SurviveZ" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
