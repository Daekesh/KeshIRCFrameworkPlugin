// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"


UKIRCCommandResponseScanner::UKIRCCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.SetNum( 0 );
	Client = NULL;
	Command = "";
	Target = "";
	State = EKIRCCommandState::S_Waiting;
	Result = "";
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
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for command response with a null client" );
		Success();
		return;
	}

	// We only accept our name!
	if ( Client->GetUser() != Source )
		return;

	Success();
}


void UKIRCCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	// If we receive these, make sure they are directed at this command.

	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchNick )
	{
		if ( !ErrorNoSuchChannel( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchChannel )
	{
		if ( !ErrorNoSuchChannel( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorUnAvailResource )
	{
		if ( !ErrorUnAvailResource( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorNotOnChannel )
	{
		if ( !ErrorNotOnChannel( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorNeedMoreParams )
	{
		if ( !ErrorNeedMoreParams( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorChanOPrivsNeeded )
	{
		if ( !ErrorChanOPrivsNeeded( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorTooManyChannels )
	{
		if ( !ErrorTooManyChannels( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorTooManyTargets )
	{
		if ( !ErrorTooManyTargets( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorChannelIsFull )
	{
		if ( !ErrorChannelIsFull( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorInviteOnlyChan )
	{
		if ( !ErrorInviteOnlyChan( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorBannedFromChan )
	{
		if ( !ErrorBannedFromChan( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorBadChannelKey )
	{
		if ( !ErrorBadChannelKey( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorErroneusNickname )
	{
		if ( !ErrorErroneusNickname( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorNickNameInUse )
	{
		if ( !ErrorNickNameInUse( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorNickCollision )
	{
		if ( !ErrorNickCollision( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorWasNoSuchNick )
	{
		if ( !ErrorWasNoSuchNick( Source, Params, Message ) )
			return;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ErrorNoChanModes )
	{
		if ( !ErrorNoChanModes( Source, Params, Message ) )
			return;
	}

	// Any other numeric reply requires no checks and is an instant fail
	Fail( Message );
}


void UKIRCCommandResponseScanner::ScanComplete_Implementation()
{
	OnScanComplete.ExecuteIfBound( this );
	RemoveRegisteredCallbacks();
}


void UKIRCCommandResponseScanner::CommandCallback( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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


void UKIRCCommandResponseScanner::Fail( const FString& Message )
{
	State = EKIRCCommandState::S_Failure;
	Result = Message;
}


void UKIRCCommandResponseScanner::Success()
{
	State = EKIRCCommandState::S_Success;
	Result = Command + " was completed successfully.";
}


bool UKIRCCommandResponseScanner::ErrorNoSuchNick( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorNoSuchChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorUnAvailResource( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorNotOnChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorNeedMoreParams( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Command.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Command, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorChanOPrivsNeeded( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorTooManyChannels( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );

}


bool UKIRCCommandResponseScanner::ErrorTooManyTargets( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorChannelIsFull( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorInviteOnlyChan( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorBannedFromChan( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorBadChannelKey( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorUserOnChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 3 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return ( Params[ 1 ] + "," + Params[ 2 ] ).Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorUserNotInChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 3 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return ( Params[ 1 ] + "," + Params[ 2 ] ).Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorErroneusNickname( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorNickNameInUse( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorNickCollision( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorWasNoSuchNick( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}


bool UKIRCCommandResponseScanner::ErrorNoChanModes( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
		return false;

	if ( Target.Len() == 0 )
		return false;

	return Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase );
}
