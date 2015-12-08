// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCWhoCommandResponseScanner.h"


UKIRCWhoCommandResponseScanner::UKIRCWhoCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchServer ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyWhoReply ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyEndOfWho ) );

	UserInfo.SetNum( 0 );
}


void UKIRCWhoCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchServer )
	{
		Super::HandleNumeric_Implementation( Source, Numeric, Params, Message );
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyEndOfWho )
	{
		Success();
		return;
	}

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

	if ( Params.Num() < 5 )
	{
		KIRCLog( Error, "Received who reply without enough parameters." );
		Fail( "Received who reply without enough parameters." );
		return;
	}

	FKIRCWhoResponseInfo ResponseInfo;
	ResponseInfo.CommonChannel = Params[ 1 ];
	ResponseInfo.Ident = Params[ 2 ];
	ResponseInfo.Host = Params[ 3 ];
	ResponseInfo.Server = Params[ 4 ];
	ResponseInfo.NickName = Params[ 5 ];
	ResponseInfo.Flags = Params[ 6 ];

	int iFirstSpace = INDEX_NONE;
	Message.FindChar( ' ', iFirstSpace );

	// No real name?
	if ( iFirstSpace == INDEX_NONE )
	{
		if ( !Message.IsNumeric() )
		{
			KIRCLog( Error, "Received who reply with a non-numeric hop count." );
			return;
		}

		ResponseInfo.Hops = FCString::Atoi( *Message );
	}

	else
	{
		FString Hops = Message.Left( iFirstSpace );
		ResponseInfo.RealName = Message.Mid( iFirstSpace + 1 );

		if ( !Hops.IsNumeric() )
		{
			KIRCLog( Error, "Received who reply with a non-numeric hop count." );
		}

		else
			ResponseInfo.Hops = FCString::Atoi( *Hops );
	}

	UserInfo.Add( ResponseInfo );

	UKIRCUser* User = Server->GetUserByName( ResponseInfo.NickName );

	if ( User == NULL )
		return;

	User->UpdateMask( ResponseInfo.NickName + "!" + ResponseInfo.Ident + "@" + ResponseInfo.Host );
	User->SetRealName( ResponseInfo.RealName );
}
