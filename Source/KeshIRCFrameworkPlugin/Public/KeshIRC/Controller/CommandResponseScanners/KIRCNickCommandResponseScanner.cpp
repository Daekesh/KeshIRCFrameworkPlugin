// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCNickCommandResponseScanner.h"


UKIRCNickCommandResponseScanner::UKIRCNickCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( "NICK" );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoNicknameGiven ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNickNameInUse ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorUnAvailResource ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorErroneusNickname ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNickCollision ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorRestricted ) );
}


void UKIRCNickCommandResponseScanner::HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for nick command response with a null client" );
		Fail( "Trying to check for nick command response with a null client" );
		return;
	}

	// We only accept our name!
	if ( Client->GetUser() != Source )
		return;

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for nick command response with a null target." );
		Fail( "Trying to check for nick command response with a null target." );
		return;
	}

	if ( Message.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for nick command response with no nickname." );
		Fail( "Trying to check for nick command response with no nickname." );
		return;
	}

	if ( !Message.Equals( Target, ESearchCase::IgnoreCase ) )
		return;

	Success();
}
