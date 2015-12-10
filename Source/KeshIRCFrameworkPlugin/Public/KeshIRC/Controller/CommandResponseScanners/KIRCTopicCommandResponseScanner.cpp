// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCTopicCommandResponseScanner.h"


UKIRCTopicCommandResponseScanner::UKIRCTopicCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( "TOPIC" );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNotOnChannel ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorChanOPrivsNeeded ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoChanModes ) );
}


void UKIRCTopicCommandResponseScanner::HandleCommand_Implementation( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for topic command response with a null client" );
		Fail( "Trying to check for topic command response with a null client" );
		return;
	}

	// We only accept our name!
	if ( Client->GetUser() != Source )
		return;

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for topic command response with a null target." );
		Fail( "Trying to check for topic command response with a null target." );
		return;
	}

	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Trying to check for topic command response with no params." );
		Fail( "Trying to check for topic command response with no params." );
		return;
	}

	if ( !Params[ 0 ].Equals( Target, ESearchCase::IgnoreCase ) )
		return;

	Success();
}
