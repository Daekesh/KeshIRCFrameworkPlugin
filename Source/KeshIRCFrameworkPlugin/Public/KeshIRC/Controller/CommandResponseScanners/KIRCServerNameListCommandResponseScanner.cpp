// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCServerNameListCommandResponseScanner.h"


UKIRCServerNameListCommandResponseScanner::UKIRCServerNameListCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchServer ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyNamReply ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyEndOfNames ) );

	Channels.SetNum( 0 );
}


void UKIRCServerNameListCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchServer )
	{
		Fail( Message );
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyEndOfNames )
	{
		Success();
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyNamReply )
	{
		if ( Client == NULL )
		{
			KIRCLog( Error, "Trying to check for command response with a null client." );
			Fail( "Trying to check for command response with a null client." );
			return;
		}

		UKIRCServer* Server = Client->GetServer();

		if ( Server == NULL )
		{
			KIRCLog( Error, "Trying to check for command response with a null server." );
			Fail( "Trying to check for command response with a null server." );
			return;
		}

		int32 iChannelIndex = INDEX_NONE;

		for ( int32 i = 0; i < Channels.Num(); ++i )
		{
			if ( !Channels[ i ].Channel.Equals( Params[ 2 ], ESearchCase::IgnoreCase ) )
				continue;

			iChannelIndex = i;
			break;
		}

		if ( iChannelIndex == INDEX_NONE )
		{
			iChannelIndex = Channels.Num();

			FKIRCServerNameListResponseInfo stNewInfo;
			stNewInfo.Channel = Params[ 2 ];
			Channels.Add( stNewInfo );
		}

		TArray<FString> UserList;
		Message.ParseIntoArray( UserList, L" " );

		for ( const FString& User : UserList )
		{
			FKIRCServerNameListResponseUserInfo UserInfo;

			for ( int32 i = 0; i < User.Len(); ++i )
			{
				if ( User[ i ] == UKIRCClient::GetModes().Channel.OpsPrefix[ 0 ] )
				{
					UserInfo.bOp = true;
					continue;
				}

				if ( User[ i ] == UKIRCClient::GetModes().Channel.HalfOpsPrefix[ 0 ] )
				{
					UserInfo.bHalfOp = true;
					continue;
				}

				if ( User[ i ] == UKIRCClient::GetModes().Channel.VoicePrefix[ 0 ] )
				{
					UserInfo.bVoice = true;
					continue;
				}

				UserInfo.User = User.Mid( i );
				Channels[ iChannelIndex ].Users.Add( UserInfo );
				break;
			}
		}
	}
}
