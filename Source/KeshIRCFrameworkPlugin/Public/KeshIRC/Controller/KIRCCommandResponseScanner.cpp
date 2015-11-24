// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"


UKIRCCommandResponseScanner::UKIRCCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	bComplete = false;
	CommandCallbacks.SetNum( 0 );
	Client = NULL;
	RegisteredCallbacks.Empty();
}


void UKIRCCommandResponseScanner::StartScan( UKIRCClient* Client )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Tried to start command response scanning with a null client." );
		return;
	}

	this->Client = Client;
	RegisterCallbacks();
}


void UKIRCCommandResponseScanner::HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	bComplete = true;
}


void UKIRCCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	bComplete = true;
}


void UKIRCCommandResponseScanner::ScanComplete_Implementation()
{
	OnScanComplete.ExecuteIfBound( this );
}


void UKIRCCommandResponseScanner::CommandCallback( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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


void UKIRCCommandResponseScanner::RegisterCallbacks()
{
	for ( FString& Command : CommandCallbacks )
	{
		FDelegateHandle Handle = RegisterCallback( Command, this, static_cast< FKIRCIncomingMessageHandlerDelegate >( &UKIRCCommandResponseScanner::CommandCallback ) );

		if ( Handle.IsValid() )
			RegisteredCallbacks.Emplace( Command, Handle );
	}
}


FDelegateHandle UKIRCCommandResponseScanner::RegisterCallback( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Tried to register a command response scanner callback with a null client." );
		return FDelegateHandle();
	}

	return Client->AddMessageHandler( Command, CallbackObject, CallbackFunction );
}


void UKIRCCommandResponseScanner::RemoveRegisteredCallbacks()
{
	for ( auto Iterator = RegisteredCallbacks.CreateConstIterator(); Iterator; ++Iterator )
		UnregisterCallback( Iterator.Key(), Iterator.Value() );
}


void UKIRCCommandResponseScanner::UnregisterCallback( const FString& Command, FDelegateHandle Handle )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Tried to unregister a command response scanner callback with a null client." );
		return;
	}

	Client->RemoveMessageHandler( Command, Handle );
}
