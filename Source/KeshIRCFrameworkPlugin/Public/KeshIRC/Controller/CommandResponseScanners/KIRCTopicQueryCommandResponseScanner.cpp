// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCTopicQueryCommandResponseScanner.h"


UKIRCTopicQueryCommandResponseScanner::UKIRCTopicQueryCommandResponseScanner( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNeedMoreParams ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ErrorNoChanModes ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyTopic ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyTopicSetBy ) );
	CommandCallbacks.Add( UKIRCClient::NumericToString( UKIRCClient::GetNumerics().ReplyNoTopic ) );

	bHasTopic = false;
	TopicBody = "";
	AuthorNickName = "";
	AuthorIdent = "";
	AuthorHost = "";
	TopicDate = FDateTime( 0 );
}


void UKIRCTopicQueryCommandResponseScanner::HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message )
{
	if ( Numeric == UKIRCClient::GetNumerics().ReplyTopic )
	{
		bHasTopic = true;
		TopicBody = Message;
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyTopicSetBy )
	{
		if ( Params.Num() < 4 )
		{
			KIRCLog( Error, "Topic details received without enough parameters." );
			Fail( "Topic details received without enough parameters." );
			return;
		}
		
		if ( !Params[ 3 ].IsNumeric() )
		{
			KIRCLog( Error, "Topic details received, but time is not numeric." );
			Fail( "Topic details received, but time is not numeric." );
			return;
		}

		UKIRCUser::ParseHostMask( Params[ 2 ], AuthorNickName, AuthorIdent, AuthorHost );
		TopicDate = FDateTime::FromUnixTimestamp( FCString::Atoi( *Params[ 3 ] ) );
		Success();
		return;
	}

	if ( Numeric == UKIRCClient::GetNumerics().ReplyNoTopic )
	{
		bHasTopic = false;
		Success();
		return;
	}

	Super::HandleNumeric_Implementation( Source, Numeric, Params, Message );
}
