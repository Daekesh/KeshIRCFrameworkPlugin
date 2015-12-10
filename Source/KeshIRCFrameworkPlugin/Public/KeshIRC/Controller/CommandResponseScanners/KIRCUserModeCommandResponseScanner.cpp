// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCMode.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCUserModeCommandResponseScanner.h"


UKIRCUserModeCommandResponseScanner::UKIRCUserModeCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorUsersDontMatch ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorUModeUnknownFlag ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyUModeIs ) );
}


void UKIRCUserModeCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* const Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ErrorNeedMoreParams || 
		 Numeric == UKIRCClient::GetNumerics().ErrorUsersDontMatch ||
		 Numeric == UKIRCClient::GetNumerics().ErrorUModeUnknownFlag )
	{
		Super::HandleNumeric_Implementation( Source, Numeric, Params, Message );
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

	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Received user mode without enough parameters." );
		Fail( "Received user mode without enough parameters." );
		return;
	}

	Modes.SetNum( 0 );
	FString ModeString = Params[ 1 ];
	TCHAR ModeChar;
	const UKIRCMode* Mode;

	for ( int32 i = 0; i < ModeString.Len(); ++i )
	{
		ModeChar = ModeString[ i ];

		if ( ModeChar == '+' )
			continue;

		if ( ModeChar == '-' )
		{
			KIRCLog( Warning, "Received negative mode in user modes." );
			continue;
		}

		Mode = Server->GetUserMode( FString( 1, &ModeChar ) );

		if ( Mode == NULL )
		{
			KIRCLog( Error, "Unknown mode in user modes." );
			return;
		}

		switch ( Mode->GetType() )
		{
			case EKIRCModeType::T_Channel_Unary:
			case EKIRCModeType::T_Channel_Param:
			case EKIRCModeType::T_Channel_List:
			case EKIRCModeType::T_Channel_User:
				KIRCLog( Error, "Received channel mode in user modes." );
				return;

			case EKIRCModeType::T_User:
				Modes.AddUnique( Mode );
				break;
		}
	}

	Client->UserModes = Modes;
}
