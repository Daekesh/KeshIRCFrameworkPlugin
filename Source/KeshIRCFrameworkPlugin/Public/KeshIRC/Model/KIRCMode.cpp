// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCMode.h"


UKIRCMode::UKIRCMode( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Mode = "";
	Type = EKIRCModeType::T_None;
	ParamRequired = EKIRCModeParamRequired::R_Never;
}


bool UKIRCMode::RequireParamForChange( EKIRCModeChange ModeChange ) const
{
	if ( ParamRequired == EKIRCModeParamRequired::R_Never )
		return false;

	if ( ModeChange == EKIRCModeChange::M_Add )
		return true;

	return ( ParamRequired == EKIRCModeParamRequired::R_Always );
}
