// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCIsOnCommandResponseScanner.h"


UKIRCIsOnCommandResponseScanner::UKIRCIsOnCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyIsOn ) );

	OnlineUsers.SetNum( 0 );
}


void UKIRCIsOnCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ErrorNeedMoreParams )
	{
		Super::HandleNumeric_Implementation( Source, Numeric, Params, Message );
		return;
	}

	TArray<FString> Nicks;
	Message.ParseIntoArray( Nicks, L" " );

	for ( const FString& Nick : Nicks )
		OnlineUsers.Add( Nick );

	Success();
}
