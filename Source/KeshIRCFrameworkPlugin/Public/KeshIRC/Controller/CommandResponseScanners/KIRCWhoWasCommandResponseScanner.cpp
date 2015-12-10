// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCWhoWasCommandResponseScanner.h"


UKIRCWhoWasCommandResponseScanner::UKIRCWhoWasCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoNicknameGiven ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorWasNoSuchNick ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoWasUser ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoIsServer) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyEndOfWhoWas ) );
}


void UKIRCWhoWasCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* const Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	// Validity
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for whowas command response with a null client." );
		Fail( "Trying to check for whowas command response with a null client." );
		return;
	}

	UKIRCServer* Server = Client->GetServer();

	if ( Server == NULL )
	{
		KIRCLog( Error, "Trying to check for whowas command response with a null server." );
		Fail( "Trying to check for whowas command response with a null server." );
		return;
	}

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for whowas command response with a null target." );
		Fail( "Trying to check for whowas command response with a null target." );
		return;
	}

	if ( !Params[ 1 ].Equals( Target, ESearchCase::IgnoreCase ) )
		return;

	// Errors
	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchNick )
	{
		Super::HandleNumeric_Implementation( Source, Numeric, Params, Message );
		return;
	}

	// Replies
	if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoWasUser )
	{
		if ( Params.Num() < 4 )
		{
			KIRCLog( Error, "WhoIs user received without enough params." );
			return;
		}

		UserInfo.NickName = Params[ 1 ];
		UserInfo.Ident = Params[ 2 ];
		UserInfo.Host = Params[ 3 ];
		UserInfo.RealName = Message;

		UKIRCUser* User = Server->GetUserByName( Params[ 1 ] );

		if ( User != NULL )
		{
			User->UpdateMask( Params[ 1 ] + "!" + Params[ 2 ] + "@" + Params[ 3 ] );
			User->SetRealName( Message );
		}
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoIsServer )
	{
		if ( Params.Num() < 3 )
		{
			KIRCLog( Error, "Received whowas server reply without a server address." );
			return;
		}

		UserInfo.ServerHost = Params[ 2 ];
		UserInfo.ServerInfo = Message;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyEndOfWhoWas )
	{
		Success();
	}
}
