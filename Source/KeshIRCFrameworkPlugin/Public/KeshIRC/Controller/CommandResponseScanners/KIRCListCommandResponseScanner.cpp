// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCListCommandResponseScanner.h"


UKIRCListCommandResponseScanner::UKIRCListCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoSuchServer ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyList ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyListEnd ) );

	Channels.SetNum( 0 );
}


void UKIRCListCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ErrorNoSuchServer )
	{
		Fail( Message );
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyListEnd )
	{
		Success();
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyList )
	{
		if ( Params.Num() < 3 )
		{
			KIRCLog( Error, "Received list reply without enough parameters." );
			return;
		}

		Channels.Add( { Params[ 1 ], FCString::Atoi( *Params[ 2 ] ), Message } );
	}
}
