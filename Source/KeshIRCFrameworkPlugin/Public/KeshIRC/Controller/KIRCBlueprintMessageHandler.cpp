// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/KIRCBlueprintMessageHandler.h"


UKIRCBlueprintMessageHandler::UKIRCBlueprintMessageHandler( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Client = NULL;
}


void UKIRCBlueprintMessageHandler::RegisterHandler( UKIRCClient* Client )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to register a command callback for a null client." );
		return;
	}

	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Trying to register a command callback for a zero-length command." );
		return;
	}

	this->Client = Client;
	Client->AddMessageHandler( Command, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCBlueprintMessageHandler::CommandCallback ) );
}


void UKIRCBlueprintMessageHandler::UnregisterHandler()
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to unregister a command callback with a null client." );
		return;
	}

	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Trying to unregister a command callback with a zero-length command." );
		return;
	}

	Client->RemoveMessageHandler( Command, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCBlueprintMessageHandler::CommandCallback ) );
	Client = NULL;
}


void UKIRCBlueprintMessageHandler::CommandCallback( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Zero-length command callback." );
		return;
	}

	if ( Command.IsNumeric() )
	{
		int32 Numeric = FCString::Atoi( *Command );
		HandleNumeric( Source, Numeric, Params, Message );
	}

	else
		HandleCommand( Source, Command, Params, Message );
}
