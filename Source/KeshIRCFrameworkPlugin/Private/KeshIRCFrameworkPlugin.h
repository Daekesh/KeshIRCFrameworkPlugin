// Copyright 2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma  once

class FKeshIRCFrameworkPlugin : public IKeshIRCFrameworkPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
