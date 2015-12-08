// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCObject.h"


UKIRCObject::UKIRCObject( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Name = "";
}


void UKIRCObject::InitObject( const FString& Name )
{
	SetName( Name );
}


UKIRCServer* const UKIRCObject::GetServer() const
{
	return Cast<UKIRCServer>( GetOuter() );
}
