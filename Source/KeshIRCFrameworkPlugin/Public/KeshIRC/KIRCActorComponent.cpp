// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/KIRCActorComponent.h"


UKIRCActorComponent::UKIRCActorComponent( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	ServerName = "";
	Host = "";
	Port = 0;
	ServerPassword = "";
	NickName = "";
	AlternateNickNames.SetNum( 0 );
	Ident = "";
	RealName = "";
	ClientClass = UKIRCClient::StaticClass();
	Client = NULL;
}


bool UKIRCActorComponent::Initialize()
{
	if ( ClientClass == NULL )
	{
		KIRCLog( Error, "Null client class." );
		return false;
	}

	if ( ClientClass->GetClassFlags() & CLASS_Abstract )
	{
		KIRCLog( Error, "Abstract client class." );
		return false;
	}
	
	Client = NewObject<UKIRCClient>( this, ClientClass );

	if ( Client == NULL )
	{
		KIRCLog( Error, "Failed to create client object." );
		return false;
	}

	return Client->InitClient( ServerName, Host, Port, ServerPassword, NickName, Ident, RealName, AlternateNickNames );
}
