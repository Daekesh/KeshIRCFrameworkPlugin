// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCKickCommandResponseScanner.h"


UKIRCKickCommandResponseScanner::UKIRCKickCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( "KICK" );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorBadChanMask ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorChanOPrivsNeeded ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorUserNotInChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNotOnChannel ) );
}


void UKIRCKickCommandResponseScanner::HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for kick command response with a null client" );
		Fail( "Trying to check for kick command response with a null client" );
		return;
	}

	// We only accept our name!
	if ( Client->GetUser() != Source )
		return;

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for kick command response with a null target." );
		Fail( "Trying to check for kick command response with a null target." );
		return;
	}

	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Trying to check for kick command response with insufficient params." );
		Fail( "Trying to check for kick command response with insufficient params." );
		return;
	}

	TArray<FString> TargetParams;
	Target.ParseIntoArray( TargetParams, L"," );

	if ( TargetParams.Num() != 2 )
	{
		KIRCLog( Error, "Trying to check for kick command response with an invalid target." );
		Fail( "Trying to check for kick command response with an invalid target." );
		return;
	}

	if ( !Params[ 0 ].Equals( TargetParams[ 1 ], ESearchCase::IgnoreCase ) )
		return;

	if ( !Params[ 1 ].Equals( TargetParams[ 0 ], ESearchCase::IgnoreCase ) )
		return;

	Success();
}


void UKIRCKickCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for kick command response with a null target." );
		Fail( "Trying to check for kick command response with a null target." );
		return;
	}

	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Trying to check for kick command response with no params." );
		Fail( "Trying to check for kick command response with no params." );
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ErrorNeedMoreParams )
	{
		if ( ErrorNeedMoreParams( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ErrorUserNotInChannel )
	{
		if ( ErrorNeedMoreParams( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	TArray<FString> TargetParams;
	Target.ParseIntoArray( TargetParams, L"," );
	FString OriginalTarget = Target;

	if ( TargetParams.Num() != 2 )
	{
		KIRCLog( Error, "Trying to check for kick command response with an invalid target." );
		Fail( "Trying to check for kick command response with an invalid target." );
		return;
	}

	Target = TargetParams[ 1 ]; // Channel

	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchChannel )
	{
		if ( ErrorNeedMoreParams( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	// Any other numeric reply is an instant fail
	Fail( Message );
}
