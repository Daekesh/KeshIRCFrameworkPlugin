// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCChannel.h"
#include "KeshIRC/Model/KIRCMode.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "Runtime/Core/Public/Internationalization/Regex.h"
#include "KeshIRC/Model/KIRCServer.h"


UKIRCServer::UKIRCServer( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Name = "";
	Host = "";
	Port = 0;
	NetworkName = "";
	State = EKIRCServerState::S_Disconnected;
	Channels.Empty();
	Users.Empty();
	UserModes.Empty();
	ChannelUnaryModes.Empty();
	ChannelListModes.Empty();
	ChannelUserModes.Empty();
}


UKIRCServer* UKIRCServer::CreateServer( UObject* Outer, const FString& ServerName, const FString& Host, int32 Port, 
										const FString& Password, const FString& NickName, const FString& Ident, 
										const FString& RealName, const TArray<FString>& AlternateNickNames,
										TSubclassOf<UKIRCClient> ClientClass )
{
	if ( Host.Len() < 3 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Invalid host." ) );
		return NULL;
	}

	if ( Port < 1 || Port > 65535 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Invalid port." ) );
		return NULL;
	}

	if ( NickName.Len() < 1 || NickName.Len() > 30 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Invalid Nickname." ) );
		return NULL;
	}

	if ( Ident.Len() < 1 || Ident.Len() > 30 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Invalid ident." ) );
		return NULL;
	}

	if ( ClientClass == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Null client class." ) );
		return NULL;
	}

	if ( ClientClass->GetClassFlags() && CLASS_Abstract )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Abstract client class." ) );
		return NULL;
	}

	bool bIsTransient = false;

	if ( Outer == NULL )
	{
		Outer = GetTransientPackage();
		bIsTransient = true;
	}

	UKIRCServer* NewServer = NewObject<UKIRCServer>( Outer );

	if ( NewServer == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Failed to create server." ) );
		return NULL;
	}

	NewServer->Name = ServerName;
	NewServer->Host = Host;
	NewServer->Port = Port;
	NewServer->Password = Password;
	NewServer->Client = NewObject<UKIRCClient>( NewServer, ClientClass );

	if ( NewServer->Client == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Failed to create client." ) );
		return NULL;
	}

	NewServer->Client->InitClient( NewServer, NickName, AlternateNickNames, Ident, RealName );

	if ( NewServer->Client->GetUser() == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Failed to create client." ) );
		return NULL;
	}

	if ( bIsTransient )
		NewServer->AddToRoot();

	return NewServer;
}


bool UKIRCServer::Connect()
{
	// TODO
	SetState( EKIRCServerState::S_Connecting );
	return true;
}


bool UKIRCServer::Disconnect()
{
	// TODO
	OnDisconnected( EKIRCServerDisconnectReason::R_ByUser );
	return true;
}


UKIRCChannel* UKIRCServer::GetChannelByName( const FString& Name ) const
{
	if ( Name.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a zero-length channel name." ) );
		return NULL;
	}

	if ( !UKIRCChannel::HasChannelPrefix( Name ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a channel without channel prefix." ) );
		return NULL;
	}

	if ( Name.Len() == 1 )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a channel with only channel prefix." ) );
		return NULL;
	}

	return Channels[ Name ];
}


UKIRCChannel* UKIRCServer::EnsureChannel( const FString& Name )
{
	if ( Name.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a zero-length channel name." ) );
		return NULL;
	}

	if ( !UKIRCChannel::HasChannelPrefix( Name ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a channel without channel prefix." ) );
		return NULL;
	}

	if ( Name.Len() == 1 )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a channel with only channel prefix." ) );
		return NULL;
	}

	UKIRCChannel* Channel = GetChannelByName( Name );

	if ( Channel == NULL )
	{
		Channel = NewObject<UKIRCChannel>( this );
		Channel->InitChannel( Name );
		Channels[ Name ] = Channel;
	}

	return Channel;
}


UKIRCUser* UKIRCServer::GetUserByName( const FString& Name ) const
{
	if ( Name.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a zero-length user name." ) );
		return NULL;
	}

	if ( UKIRCChannel::HasChannelPrefix( Name ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a user with a channel prefix." ) );
		return NULL;
	}

	return Users[ Name ];
}


UKIRCUser* UKIRCServer::EnsureUser( const FString& Name, const FString& Ident, const FString& Host )
{
	if ( Name.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a zero-length user name." ) );
		return NULL;
	}

	if ( UKIRCChannel::HasChannelPrefix( Name ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to get a user with a channel prefix." ) );
		return NULL;
	}

	UKIRCUser* User = GetUserByName( Name );

	if ( User == NULL )
	{
		User = NewObject<UKIRCUser>( this );
		User->InitUser( Name, Ident, Host );
		Users[ Name ] = User;
	}

	return User;
}


void UKIRCServer::RenameUser( UKIRCUser* User, const FString& NewName )
{
	if ( User == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to rename a null user." ) );
		return;
	}

	if ( NewName.Len() == 0 )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to rename a user to a zero-length name." ) );
		return;
	}

	if ( !Users.Contains( User->GetName() ) )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Trying to remove a user not in the map." ) );
		return;
	}

	Users.Remove( User->GetName() );
	Users[ NewName ] = User;
	User->SetName( NewName );
}


void UKIRCServer::RemoveUser( UKIRCUser* User )
{
	if ( User == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to remove a null user." ) );
		return;
	}

	Users.Remove( User->GetName() );
}


void UKIRCServer::RemoveChannel( UKIRCChannel* Channel )
{
	if ( Channel == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Error, TEXT( "Trying to remove a null channel." ) );
		return;
	}

	Channels.Remove( Channel->GetName() );
}


UKIRCMode* UKIRCServer::GetChannelMode( const FString& Mode ) const
{
	UKIRCMode* ModeObj = GetChannelUserMode( Mode );

	if ( ModeObj != NULL )
		return ModeObj;

	ModeObj = GetChannelListMode( Mode );

	if ( ModeObj != NULL )
		return ModeObj;

	ModeObj = GetChannelUnaryMode( Mode );

	return ModeObj;
}


TArray<UKIRCMode*> UKIRCServer::GetAvailableChannelModes() const
{
	TArray<UKIRCMode*> ChannelUnaryModeList;
	ChannelUnaryModes.GenerateValueArray( ChannelUnaryModeList );

	TArray<UKIRCMode*> ChannelListModeList;
	ChannelListModes.GenerateValueArray( ChannelListModeList );

	TArray<UKIRCMode*> ChannelUserModeList;
	ChannelUserModes.GenerateValueArray( ChannelUserModeList );

	TArray<UKIRCMode*> ChannelModeList;
	ChannelModeList += ChannelUnaryModeList;
	ChannelModeList += ChannelListModeList;
	ChannelModeList += ChannelUserModeList;

	return ChannelModeList;
}


void UKIRCServer::ParseLine( const FString& Line )
{
	if ( Line.Len() < 6 )
	{
		if ( Client != NULL )
			Client->OnUnhandledRawDelegate.Broadcast( this, Line );

		return;
	}

	if ( Line[ 0 ] != ':' )
	{
		if ( Line.Left( 5 ) != "PING " )
		{
			if ( Client != NULL )
				Client->OnUnhandledRawDelegate.Broadcast( this, Line );

			return;
		}

		Command( "PONG " + Line.RightChop( 5 ) );
		return;
	}

	if ( Client == NULL )
	{
		UE_LOG( LogKeshIRCFramework, Warning, TEXT( "Cannot process messages without a client." ) );
		return;
	}

	static FRegexPattern Pattern = FRegexPattern( "^:(([^!@ ]+)(!([^@ ]+))?(@([^ ]+))?) ([^: ]+)(( [^: ]+)*)?( :(.*))?$" );
	FRegexMatcher Matcher = FRegexMatcher( Pattern, Line );

	if ( Matcher.FindNext() )
	{
		Client->OnUnhandledRawDelegate.Broadcast( this, Line );
		return;
	}

	FString SourceName = Matcher.GetCaptureGroup( 2 );

	if ( SourceName.Len() == 0 )
	{
		Client->OnUnhandledRawDelegate.Broadcast( this, Line );
		return;
	}

	FString SourceMask = Matcher.GetCaptureGroup( 1 );
	FString SourceIdent = Matcher.GetCaptureGroup( 4 );
	FString SourceHost = Matcher.GetCaptureGroup( 6 );
	FString Command = Matcher.GetCaptureGroup( 7 ).ToUpper();
	FString ParamString = Matcher.GetCaptureGroup( 8 );
	FString Message = Matcher.GetCaptureGroup( 11 );
	UKIRCUser* Source = NULL;

	if ( SourceIdent.Len() > 0 && SourceHost.Len() > 0 )
	{
		Source = EnsureUser( SourceName, "", "" );

		if ( Source != NULL && Source->GetHostMask() == "" )
			Source->UpdateMask( SourceMask );
	}

	TArray<FString> Params;
	ParamString.ParseIntoArray( Params, TEXT( " ." ) );

	Client->HandleMessage( Line, Source, Command, Params, Message );
}


void UKIRCServer::Command( const FString& Command )
{
	// TODO
}


void UKIRCServer::OnConnected()
{
	SetState( EKIRCServerState::S_Connected );
	
	if ( Client != NULL )
		Client->OnServerConnectedDelegate.Broadcast( this );
}


void UKIRCServer::OnDisconnected( EKIRCServerDisconnectReason Reason )
{
	SetState( EKIRCServerState::S_Disconnected );

	if ( Client != NULL )
		Client->OnServerDisconnectedDelegate.Broadcast( this, Reason );
}


void UKIRCServer::OnConnectionError( const FString& Reason )
{
	SetState( EKIRCServerState::S_Error );

	if ( Client != NULL )
		Client->OnConnectionErrorDelegate.Broadcast( this, Reason );
}
