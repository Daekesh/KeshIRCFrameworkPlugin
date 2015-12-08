// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCChannelNameListCommandResponseScanner.h"


UKIRCChannelNameListCommandResponseScanner::UKIRCChannelNameListCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchServer ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyNamReply ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyEndOfNames ) );

	Users.SetNum( 0 );
}


void UKIRCChannelNameListCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
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
		TArray<FString> UserList;
		Message.ParseIntoArray( UserList, L" " );

		for ( const FString& User : UserList )
		{
			FKIRCChannelNameListResponseInfo UserInfo;

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
				Users.Add( UserInfo );
				break;
			}
		}
	}
}
