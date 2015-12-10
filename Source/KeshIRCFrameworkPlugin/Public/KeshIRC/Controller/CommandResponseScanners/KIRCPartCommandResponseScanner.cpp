// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCPartCommandResponseScanner.h"


UKIRCPartCommandResponseScanner::UKIRCPartCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( "PART" );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNotOnChannel ) );
}


void UKIRCPartCommandResponseScanner::HandleCommand_Implementation( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for part command response with a null client" );
		Fail( "Trying to check for part command response with a null client" );
		return;
	}

	// We only accept our name!
	if ( Client->GetUser() != Source )
		return;

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for part command response with a null target." );
		Fail( "Trying to check for part command response with a null target." );
		return;
	}

	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Trying to check for part command response with no params." );
		Fail( "Trying to check for part command response with no params." );
		return;
	}

	if ( !Params[ 0 ].Equals( Target, ESearchCase::IgnoreCase ) )
		return;

	Success();
}
