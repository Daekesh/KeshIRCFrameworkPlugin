// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCChannel.h"


UKIRCChannel::UKIRCChannel( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	TopicBody = "";
	TopicAuthor = "";
	TopicDate = FDateTime( 0 );
	Modes.SetNum( 0 );
	ModeLists.Empty();
	Limit = 0;
	JoinKey = "";
	Users.Empty();
}


bool UKIRCChannel::HasChannelPrefix( const FString& ObjectName )
{
	if ( ObjectName.Len() == 0 )
		return false;
	
	return ( ObjectName[ 0 ] == '#' || ObjectName[ 0 ] == '&' || ObjectName[ 0 ] == '+' || ObjectName[ 0 ] == '!' );
}


void UKIRCChannel::InitChannel( const FString& Name )
{
	InitObject( Name );
}


const FKIRCChannelUserInfo& UKIRCChannel::GetChannelUserInfo( UKIRCUser* User )
{
	static FKIRCChannelUserInfo DefaultReturnValue = { NULL, NULL, FDateTime( 0 ), TArray<UKIRCMode*>() };
	
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to get channel user info for a null user." );
		return DefaultReturnValue;
	}

	if ( !Users.Contains( User ) )
		return DefaultReturnValue;

	return Users[ User ];
}


bool UKIRCChannel::HasUserLimit() const
{
	UKIRCServer* Server = GetServer();

	if ( Server == NULL )
	{
		KIRCLog( Error, "Cannot get server." );
		return false;
	}

	UKIRCMode* KeyMode = Server->GetChannelMode( MODE_CHANNEL_USER_LIMIT );

	if ( KeyMode == NULL )
		return false;

	return IsChannelModeSet( KeyMode );
}


bool UKIRCChannel::IsJoinKeySet() const
{
	UKIRCServer* Server = GetServer();

	if ( Server == NULL )
	{
		KIRCLog( Error, "Cannot get server." );
		return false;
	}

	UKIRCMode* KeyMode = Server->GetChannelMode( MODE_CHANNEL_KEY );

	if ( KeyMode == NULL )
		return false;

	return IsChannelModeSet( KeyMode );
}


const TArray<FString>& UKIRCChannel::GetChannelModeListValues( UKIRCMode* Mode ) const
{
	static TArray<FString> DefaultReturnValue = TArray<FString>();

	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to channel mode list for null mode." );
		return DefaultReturnValue;
	}

	if ( !ModeLists.Contains( Mode ) )
		return DefaultReturnValue;

	return ModeLists[ Mode ].List;
}


void UKIRCChannel::UserJoined( UKIRCUser* User )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Null user trying to join a channel." );
		return;
	}

	if ( HasUser( User ) )
	{
		KIRCLog( Error, "User joining channel that they are already in." );
		return;
	}

	Users[ User ] = { this, User, FDateTime::Now(), TArray<UKIRCMode*>() };
	User->JoinChannel( this );
}


void UKIRCChannel::UserLeft( UKIRCUser* User )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Null user trying to leave a channel." );
		return;
	}

	if ( !HasUser( User ) )
	{
		KIRCLog( Error, "User leaving channel that they are not in." );
		return;
	}

	Users.Remove( User );
	User->LeaveChannel( this );
}


void UKIRCChannel::AddUnaryMode( UKIRCMode* Mode )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to add a null unary mode." );
		return;
	}

	if ( IsChannelModeSet( Mode ) )
	{
		KIRCLog( Error, "Trying to add a unary mode that's already set." );
		return;
	}

	Modes.Add( Mode );
}


void UKIRCChannel::RemoveUnaryMode( UKIRCMode* Mode )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to remove a null unary mode." );
		return;
	}

	if ( !IsChannelModeSet( Mode ) )
	{
		KIRCLog( Error, "Trying to remove a unary mode that isn't set." );
		return;
	}

	Modes.Remove( Mode );
}


void UKIRCChannel::AddListModeEntry( UKIRCMode* Mode, const FString& Entry )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to add a null list mode." );
		return;
	}

	if ( Entry.Len() == 0 )
	{
		KIRCLog( Error, "Trying to add a zero length list mode value." );
		return;
	}

	if ( !ModeLists.Contains( Mode ) )
	{
		FKIRCModeListContainer Entries;
		ModeLists[ Mode ] = Entries;
	}

	ModeLists[ Mode ].List.Add( Entry );
}



void UKIRCChannel::RemoveListModeEntry( UKIRCMode* Mode, const FString& Entry )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to remove a null list mode." );
		return;
	}

	if ( Entry.Len() == 0 )
	{
		KIRCLog( Error, "Trying to remove a zero length list mode value." );
		return;
	}

	if ( !ModeLists.Contains( Mode ) )
	{
		KIRCLog( Error, "Trying to remove a list mode value that has none set." );
		return;
	}

	ModeLists[ Mode ].List.Remove( Entry );

	if ( ModeLists[ Mode ].List.Num() == 0 )
		ModeLists.Remove( Mode );
}


void UKIRCChannel::AddUserMode( UKIRCUser* User, UKIRCMode* Mode )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to add a mode to a null user." );
		return;
	}
	
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying add a nul mode to a user." );
		return;
	}

	if ( Users.Contains( User ) )
	{
		KIRCLog( Error, "Trying add a mode to a user not in the channel." );
		return;
	}

	Users[ User ].Modes.Add( Mode );
}


void UKIRCChannel::RemoveUserMode( UKIRCUser* User, UKIRCMode* Mode )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to remove a mode to a null user." );
		return;
	}

	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying remove a nul mode to a user." );
		return;
	}

	if ( Users.Contains( User ) )
	{
		KIRCLog( Error, "Trying remove a mode to a user not in the channel." );
		return;
	}

	Users[ User ].Modes.Remove( Mode );
}
