// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCChannel.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCUser.h"


UKIRCUser::UKIRCUser( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Ident = "";
	Host = "";
	RealName = "";
	HostMask = "";
	DisplayName = "";

	Channels.SetNum( 0 );
}


void UKIRCUser::InitUser( const FString& Name, const FString& Ident, const FString& Host )
{
	InitObject( Name );
	
	this->Ident = Ident;
	this->Host = Host;
}


void UKIRCUser::SetName( const FString& Name )
{
	Super::SetName( Name );
	DisplayName = "";
}


const FString& UKIRCUser::GetDisplayName()
{
	if ( DisplayName.Len() == 0 )
	{
		if ( Name.Len() > 0 )
			DisplayName = Name;

		else
			DisplayName = Ident + "@" + Host;
	}

	return DisplayName;
}


const FKIRCChannelUserInfo& UKIRCUser::GetChannelUserInfo( UKIRCChannel* Channel )
{
	static FKIRCChannelUserInfo DefaultReturnValue = { NULL, NULL, FDateTime( 0 ), TArray<UKIRCMode*>() };

	if ( Channel == NULL )
		return DefaultReturnValue;

	return Channel->GetChannelUserInfo( this );
}


void UKIRCUser::ParseHostMask( const FString& Mask, FString& Name, FString& Ident, FString& Host )
{
	Name = "";
	Ident = "";
	Host = "";

	if ( Mask.Len() == 0 )
	{
		Name = "?";
		return;
	}

	int32 iIndexOfAt = INDEX_NONE;
	Mask.FindChar( '@', iIndexOfAt );

	int32 iIndexOfExclaim = INDEX_NONE;
	Mask.FindChar( '!', iIndexOfExclaim );

	if ( iIndexOfAt == INDEX_NONE && iIndexOfExclaim == INDEX_NONE )
	{
		Name = Mask;
	}

	else if ( iIndexOfAt == INDEX_NONE )
	{
		Name = iIndexOfExclaim > 0 ? Mask.Left( iIndexOfExclaim ) : "?";
		Host = iIndexOfExclaim < ( Mask.Len() - 1 ) ? Mask.Mid( iIndexOfExclaim + 1 ) : "";
	}

	else if ( iIndexOfExclaim == INDEX_NONE )
	{
		Name = iIndexOfAt > 0 ? Mask.Left( iIndexOfAt ) : "?";
		Host = iIndexOfAt < ( Mask.Len() - 1 ) ? Mask.Mid( iIndexOfAt + 1 ) : "";
	}

	else
	{
		Name = iIndexOfExclaim > 0 ? Mask.Left( iIndexOfExclaim ) : "?";
		Ident = ( iIndexOfAt - iIndexOfExclaim - 1 ) > 0 ? Mask.Mid( iIndexOfExclaim + 1, iIndexOfAt - iIndexOfExclaim - 1 ) : "";
		Host = iIndexOfAt < ( Mask.Len() - 1 ) ? Mask.Mid( iIndexOfAt + 1 ) : "";
	}
}


void UKIRCUser::UpdateMask( const FString& Mask )
{
	HostMask = Mask;
	UKIRCUser::ParseHostMask( Mask, Name, Ident, Host );
	DisplayName = "";
}


bool UKIRCUser::JoinChannel( UKIRCChannel* Channel )
{
	if ( Channel == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Tried to join a null channel." ) );
		return false;
	}

	if ( Channels.Contains( Channel ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Tried to join a a channel we're already in." ) );
		return false;
	}

	Channels.Add( Channel );
	return true;
}


bool UKIRCUser::LeaveChannel( UKIRCChannel* Channel )
{
	if ( Channel == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Tried to leave a null channel." ) );
		return false;
	}

	if ( !Channels.Contains( Channel ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Tried to leave a channel we're not in." ) );
		return false;
	}

	Channels.Remove( Channel );
	return true;
}
