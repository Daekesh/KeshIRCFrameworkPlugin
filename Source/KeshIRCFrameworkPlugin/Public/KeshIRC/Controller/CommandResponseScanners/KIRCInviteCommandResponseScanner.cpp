// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCInviteCommandResponseScanner.h"


UKIRCInviteCommandResponseScanner::UKIRCInviteCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchNick ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNotOnChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorUserOnChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorChanOPrivsNeeded ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyInviting ) );
}


void UKIRCInviteCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ReplyInviting )
	{
		Success();
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ErrorNeedMoreParams )
	{
		if ( ErrorNeedMoreParams( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ErrorUserOnChannel )
	{
		if ( ErrorUserOnChannel( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	// We've got 2 params, so play around with the value. Yeah.
	TArray<FString> TargetParams;
	Target.ParseIntoArray( TargetParams, L"," );
	FString OriginalTarget = Target;

	if ( TargetParams.Num() != 2 )
	{
		KIRCLog( Error, "Invalid target parameters for invite command response scanner." );
		Fail( "Invalid target parameters for invite command response scanner." );
		return;
	}

	// Nick
	Target = TargetParams[ 0 ];

	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchNick )
	{
		if ( ErrorNoSuchChannel( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	// Channel
	Target = TargetParams[ 1 ];

	if ( Numeric == UKIRCClient::GetNumerics().ErrorNotOnChannel )
	{
		if ( ErrorNotOnChannel( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ErrorChanOPrivsNeeded )
	{
		if ( ErrorChanOPrivsNeeded( Source, Params, Message ) )
			Fail( Message );

		return;
	}

	// Any other numeric reply is an instant fail
	Fail( Message );
}
