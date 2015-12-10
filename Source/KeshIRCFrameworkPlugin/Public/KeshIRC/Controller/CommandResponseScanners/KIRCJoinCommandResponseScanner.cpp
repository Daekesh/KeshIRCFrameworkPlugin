// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCJoinCommandResponseScanner.h"


UKIRCJoinCommandResponseScanner::UKIRCJoinCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( "JOIN" );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorInviteOnlyChan ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorChannelIsFull ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorTooManyTargets ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorBannedFromChan ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorBadChannelKey ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorBadChanMask ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorTooManyChannels ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorUnAvailResource ) );
}


void UKIRCJoinCommandResponseScanner::HandleCommand_Implementation( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for join command response with a null client" );
		Fail( "Trying to check for join command response with a null client" );
		return;
	}

	// We only accept our name!
	if ( Client->GetUser() != Source )
		return;

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for join command response with a null target." );
		Fail( "Trying to check for join command response with a null target." );
		return;
	}

	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Trying to check for join command response with no params." );
		Fail( "Trying to check for join command response with no params." );
		return;
	}

	if ( !Params[ 0 ].Equals( Target, ESearchCase::IgnoreCase ) )
		return;

	Success();
}
