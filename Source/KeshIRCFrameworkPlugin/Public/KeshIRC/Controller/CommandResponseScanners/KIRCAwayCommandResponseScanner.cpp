// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCAwayCommandResponseScanner.h"


UKIRCAwayCommandResponseScanner::UKIRCAwayCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyNowAway ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyUnAway ) );
}


void UKIRCAwayCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* const Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	Success();
}
