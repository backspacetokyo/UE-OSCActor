// Copyright Epic Games, Inc. All Rights Reserved.

#include "OSCActorModule.h"
#include "ISettingsModule.h"
#include "OSCActorSubsystem.h"

#define LOCTEXT_NAMESPACE "FOSCActorModule"

void FOSCActorModule::StartupModule()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"OSCActor",
			FText::FromString(TEXT("OSCActor")),
			FText::FromString(TEXT("")),
			GetMutableDefault<UOSCActorSettings>()
		);
	}
}

void FOSCActorModule::ShutdownModule()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings(
			"Project",
			"Plugins",
			"OSCActor"
		);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOSCActorModule, OSCActor)