// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCMode.h"


UKIRCMode::UKIRCMode( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Mode = "";
	Type = EKIRCModeType::T_None;
}

