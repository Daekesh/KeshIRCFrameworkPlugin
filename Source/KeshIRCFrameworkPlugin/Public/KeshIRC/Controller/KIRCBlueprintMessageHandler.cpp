// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/KIRCBlueprintMessageHandler.h"


UKIRCBlueprintMessageHandler::UKIRCBlueprintMessageHandler( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Client = NULL;
	DelegateHandle.Reset();
}


void UKIRCBlueprintMessageHandler::RegisterHandler( UKIRCClient* Client )
{
	if ( Client == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to register a command callback for a null client." ) );
		return;
	}

	if ( Command.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to register a command callback for a zero-length command." ) );
		return;
	}

	if ( DelegateHandle.IsValid() )
	{
		if ( Client != NULL && Command.Len() > 0 )
			Client->RemoveMessageHandler( Command, DelegateHandle );

		else
			UE_LOG( LogKeshIRCFramework, Error, TEXT( "Registering a command callback with an already valid delegate handle which is unable to be removed." ) );
	}

	this->Client = Client;
	this->Command = Command;
	DelegateHandle.Reset();

	Client->AddMessageHandler( Command, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCBlueprintMessageHandler::CommandCallback ) );
}


void UKIRCBlueprintMessageHandler::UnregisterHandler()
{
	if ( Client == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to unregister a command callback with a null client." ) );
		return;
	}

	if ( Command.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to unregister a command callback with a zero-length command." ) );
		return;
	}

	if ( !DelegateHandle.IsValid() )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to unregister a command callback without a valid delegate handle." ) );
		return;
	}

	Client->RemoveMessageHandler( Command, DelegateHandle );
	
	Client = NULL;
	Command = "";
	DelegateHandle.Reset();
}


void UKIRCBlueprintMessageHandler::CommandCallback( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Command.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Zero-length command callback." ) );
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
