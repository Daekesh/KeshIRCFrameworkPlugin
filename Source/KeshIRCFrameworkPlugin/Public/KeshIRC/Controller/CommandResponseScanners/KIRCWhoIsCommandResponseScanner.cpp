// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCWhoIsCommandResponseScanner.h"


UKIRCWhoIsCommandResponseScanner::UKIRCWhoIsCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchNick ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoIsUser ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoIsChannels ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoIsServer ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoIsOperator ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoIsIdle ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoAuthName ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyAway ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyEndOfWhoIs ) );

	UserInfo.Channels.SetNum( 0 );
}


void UKIRCWhoIsCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	// Validity
	if ( Client == NULL )
	{
		KIRCLog( Error, "Trying to check for whois command response with a null client." );
		Fail( "Trying to check for whois command response with a null client." );
		return;
	}

	UKIRCServer* Server = Client->GetServer();

	if ( Server == NULL )
	{
		KIRCLog( Error, "Trying to check for whois command response with a null server." );
		Fail( "Trying to check for whois command response with a null server." );
		return;
	}

	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to check for whois command response with a null target." );
		Fail( "Trying to check for whois command response with a null target." );
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
	if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoIsUser )
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

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoIsChannels )
	{
		TArray<FString> ChannelList;
		Message.ParseIntoArray( ChannelList, L" " );

		for ( const FString& Channel : ChannelList )
		{
			FKIRCWhoIsResponseChannelInfo ChannelInfo;

			for ( int32 i = 0; i < Channel.Len(); ++i )
			{
				if ( Channel[ i ] == UKIRCClient::GetModes().Channel.OpsPrefix[ 0 ] )
				{
					ChannelInfo.bOp = true;
					continue;
				}

				if ( Channel[ i ] == UKIRCClient::GetModes().Channel.HalfOpsPrefix[ 0 ] )
				{
					ChannelInfo.bHalfOp = true;
					continue;
				}

				if ( Channel[ i ] == UKIRCClient::GetModes().Channel.VoicePrefix[ 0 ] )
				{
					ChannelInfo.bVoice = true;
					continue;
				}

				ChannelInfo.Channel = Channel.Mid( i );
				UserInfo.Channels.Add( ChannelInfo );
				break;
			}
		}
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoIsServer )
	{
		if ( Params.Num() < 3 )
		{
			KIRCLog( Error, "Received whois server reply without a server address." );
			return;
		}

		UserInfo.ServerHost = Params[ 2 ];
		UserInfo.ServerName = Message;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoIsOperator )
		UserInfo.bOperator = true;

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyAway )
	{
		UserInfo.bAway = true;
		UserInfo.AwayMessage = Message;
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoIsIdle )
	{
		if ( Params.Num() < 3 )
		{
			KIRCLog( Error, "Received whois idle reply without an idle time." );
			return;
		}

		UserInfo.bIdle = true;
		UserInfo.SecondsIdle = FCString::Atoi( *Params[ 2 ] );
	}
	
	else if ( Numeric == UKIRCClient::GetNumerics().ReplyWhoAuthName )
	{
		if ( Params.Num() < 3 )
		{
			KIRCLog( Error, "Received whois auth name reply without an auth name." );
			return;
		}

		UserInfo.AuthName = Params[ 2 ];
	}

	else if ( Numeric == UKIRCClient::GetNumerics().ReplyEndOfWhoIs )
	{
		Success();
	}
}
