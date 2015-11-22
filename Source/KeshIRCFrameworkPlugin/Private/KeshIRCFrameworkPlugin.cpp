// Copyright 2015 Matthew Chapman, Inc. All Rights Reserved.

#include "KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRCFrameworkPlugin.h"

IMPLEMENT_MODULE( FKeshIRCFrameworkPlugin, KeshIRCFrameworkPlugin )
DEFINE_LOG_CATEGORY( LogKeshIRCFramework );

void FKeshIRCFrameworkPlugin::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FKeshIRCFrameworkPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
