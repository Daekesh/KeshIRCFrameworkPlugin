// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/KIRCActorComponent.h"
#include "KeshIRC/KIRCActor.h"


AKIRCActor::AKIRCActor( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	IRCComponent = ObjectInitializer.CreateDefaultSubobject<UKIRCActorComponent>( this, "Kesh IRC Component", false );
}


UKIRCActorComponent* const AKIRCActor::GetKeshIRCComponent() const
{
	return IRCComponent;
}
