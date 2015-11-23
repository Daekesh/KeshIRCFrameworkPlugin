// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCChannel.h"
#include "KeshIRC/Model/KIRCMode.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "Runtime/Core/Public/Internationalization/Regex.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "KeshIRC/Model/KIRCServer.h"


UKIRCServer::UKIRCServer( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Name = "";
	Host = "";
	Port = 0;
	Password = "";
	HostActual = "";
	NetworkName = "";
	Version = "";
	Client = NULL;
	State = EKIRCServerState::S_Disconnected;
	HostResolver = NULL;
	HostAddr.Reset();
	Ticker = NULL;
	Socket = NULL;
	ReadBuffer = NULL;
	Channels.Empty();
	Users.Empty();
	UserModes.Empty();
	ChannelModes.Empty();
}


UKIRCServer::~UKIRCServer()
{
	if ( Ticker != NULL )
		delete Ticker;

	if ( Socket != NULL )
	{
		Socket->Close();
		delete Socket;
	}
}


bool UKIRCServer::InitServer( const FString& ServerName, const FString& Host, int32 Port, const FString& Password )
{
	if ( Host.Len() < 3 )
	{
		KIRCLog( Error, "Invalid host." );
		return false;
	}

	if ( Port < 1 || Port > 65535 )
	{
		KIRCLog( Error, "Invalid port." );
		return false;
	}

	Name = ServerName;
	this->Host = Host;
	this->Port = Port;
	this->Password = Password;

	if ( Ticker != NULL )
	{
		delete Ticker;
		KIRCLog( Warning, "Ticker still alive for some reason?" );
	}

	ISocketSubsystem* SocketSub = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM );

	if ( SocketSub == NULL )
	{
		KIRCLog( Error, "Unable to initialise socket subsystem." );
		return false;
	}

	HostAddr = SocketSub->CreateInternetAddr();
	HostAddr->SetIp( 0 );
	HostAddr->SetPort( Port );
	HostResolver = SocketSub->GetHostByName( TCHAR_TO_ANSI( *Host ) );
	Ticker = new FKIRCServerTicket( this );
	
	return true;
}


void UKIRCServer::Reset()
{
	if ( HostResolver != NULL )
	{
		delete HostResolver;
		HostResolver = NULL;
	}
	
	Disconnect();

	NetworkName = "";
	Version = "";
	HostActual = "";
	Channels.Empty();
	Users.Empty();
	UserModes.Empty();
	ChannelModes.Empty();
	Settings.Empty();
	
	if ( HostAddr.IsValid() )
	{
		int32 iIntIp = 0;
		HostAddr->SetIp( iIntIp );
	}
}


bool UKIRCServer::Connect()
{
	KIRCLog( Log, "Connection initiated." );
	// Create socket
	ISocketSubsystem* SocketSub = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM );

	if ( SocketSub == NULL )
	{
		KIRCLog( Error, "Unable to initialise socket subsystem." );
		return false;
	}

	Socket = SocketSub->CreateSocket( NAME_Stream, "KIRC Server Connection" );
	Socket->SetNonBlocking( true );

	SetState( EKIRCServerState::S_Connecting );
	return true;
}


bool UKIRCServer::Disconnect()
{
	KIRCLog( Log, "Disconnecting..." );

	OnDisconnected( EKIRCServerDisconnectReason::R_ByUser );

	if ( Socket != NULL )
	{
		Socket->Close();
		delete Socket;
		Socket = NULL;
	}

	return true;
}


UKIRCChannel* UKIRCServer::GetChannelByName( const FString& Name ) const
{
	if ( Name.Len() == 0 )
	{
		KIRCLog( Error, "Trying to get a zero-length channel name." );
		return NULL;
	}

	if ( !UKIRCChannel::HasChannelPrefix( Name ) )
	{
		KIRCLog( Error, "Trying to get a channel without channel prefix." );
		return NULL;
	}

	if ( Name.Len() == 1 )
	{
		KIRCLog( Error, "Trying to get a channel with only channel prefix." );
		return NULL;
	}

	return Channels[ Name ];
}


UKIRCChannel* UKIRCServer::EnsureChannel( const FString& Name )
{
	if ( Name.Len() == 0 )
	{
		KIRCLog( Error, "Trying to get a zero-length channel name." );
		return NULL;
	}

	if ( !UKIRCChannel::HasChannelPrefix( Name ) )
	{
		KIRCLog( Error, "Trying to get a channel without channel prefix." );
		return NULL;
	}

	if ( Name.Len() == 1 )
	{
		KIRCLog( Error, "Trying to get a channel with only channel prefix." );
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
		KIRCLog( Error, "Trying to get a zero-length user name." );
		return NULL;
	}

	if ( UKIRCChannel::HasChannelPrefix( Name ) )
	{
		KIRCLog( Error, "Trying to get a user with a channel prefix." );
		return NULL;
	}

	return Users[ Name ];
}


UKIRCUser* UKIRCServer::EnsureUser( const FString& Name, const FString& Ident, const FString& Host )
{
	if ( Name.Len() == 0 )
	{
		KIRCLog( Error, "Trying to get a zero-length user name." );
		return NULL;
	}

	if ( UKIRCChannel::HasChannelPrefix( Name ) )
	{
		KIRCLog( Error, "Trying to get a user with a channel prefix." );
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
		KIRCLog( Error, "Trying to rename a null user." );
		return;
	}

	if ( NewName.Len() == 0 )
	{
		KIRCLog( Error, "Trying to rename a user to a zero-length name." );
		return;
	}

	if ( !Users.Contains( User->GetName() ) )
	{
		KIRCLog( Error, "Trying to remove a user not in the map." );
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
		KIRCLog( Error, "Trying to remove a null user." );
		return;
	}

	Users.Remove( User->GetName() );
}


void UKIRCServer::RemoveChannel( UKIRCChannel* Channel )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Trying to remove a null channel." );
		return;
	}

	Channels.Remove( Channel->GetName() );
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

		Send( "PONG " + Line.RightChop( 5 ) );
		return;
	}

	if ( Client == NULL )
	{
		KIRCLog( Error, "Cannot process messages without a client." );
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


bool UKIRCServer::Send( const FString& Command )
{
	if ( Socket == NULL )
	{
		KIRCLog( Error, "Attempting to send a command to a null socket." );
		return false;
	}

	FString CleanCommand = "";

	switch ( Socket->GetConnectionState() )
	{
		case SCS_NotConnected:
			KIRCLog( Error, "Attempting to send a command to an unconnected socket." );
			return false;

		case SCS_ConnectionError:
			KIRCLog( Error, "Attempting to send a command to an errored socket." );
			return false;

		case SCS_Connected:
			CleanCommand = UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().Command, Command );

			if ( CleanCommand.Len() > MaxCommandLength )
			{
				KIRCLog( Error, "Command length too long." );
				return false;
			}

			int32 iSent = 0;
			uint8 buffer[ MaxCommandLength + 2 ];

			for ( int32 i = 0; i < CleanCommand.Len(); ++i )
				buffer[ i ] = static_cast< uint8 >( CleanCommand[ i ] );

			buffer[ CleanCommand.Len() ] = '\r';
			buffer[ CleanCommand.Len() + 1 ] = '\n';

			Socket->Send( buffer, CleanCommand.Len() + 2, iSent );
			return true;
	}	

	return false;
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
	{
		Client->OnConnectionErrorDelegate.Broadcast( this, Reason );
		Client->OnServerDisconnectedDelegate.Broadcast( this, EKIRCServerDisconnectReason::R_Network );
	}
}


UKIRCMode* UKIRCServer::AddUserMode( const FString& ModeCharacter )
{
	if ( ModeCharacter.Len() != 1 )
	{
		KIRCLog( Error, "Tried to add a user mode that wasn't 1 character long." );
		return NULL;
	}

	UKIRCMode* NewMode = NewObject<UKIRCMode>( this );
	NewMode->InitMode( ModeCharacter, EKIRCModeType::T_User );

	UserModes[ ModeCharacter ] = NewMode;

	return NewMode;
}


UKIRCMode* UKIRCServer::AddChannelMode( const FString& ModeCharacter, EKIRCModeType ModeType )
{
	if ( ModeCharacter.Len() != 1 )
	{
		KIRCLog( Error, "Tried to add a user mode that wasn't 1 character long." );
		return NULL;
	}

	UKIRCMode* NewMode = NewObject<UKIRCMode>( this );
	NewMode->InitMode( ModeCharacter, ModeType );

	switch ( ModeType )
	{
		// New object should be immediately gc'd!
		default:
		case EKIRCModeType::T_None:
		case EKIRCModeType::T_User:
			KIRCLog( Error, "Invalid channel mode type." );
			return NULL;

		case EKIRCModeType::T_Channel_Unary:
		case EKIRCModeType::T_Channel_Param:
		case EKIRCModeType::T_Channel_List:
		case EKIRCModeType::T_Channel_User:
			ChannelModes[ ModeCharacter ] = NewMode;
	}

	return NewMode;
}


void UKIRCServer::Tick()
{
	switch ( State )
	{
		default:
		case EKIRCServerState::S_Error:
			return;

		// We've not yet connected, so need to try to resolve the address, if we have to.
		case EKIRCServerState::S_Disconnected:
			if ( HostResolver == NULL )
				return;

			if ( !HostResolver->IsComplete() )
				return;

			if ( HostResolver->GetErrorCode() == SE_NO_ERROR )
			{
				uint32 iIntIp;
				HostResolver->GetResolvedAddress().GetIp( iIntIp );
				HostAddr->SetIp( iIntIp );
				KIRCLog( Log, "Host resolved." );
			}

			else
			{
				KIRCLog( Error, "Unable to resolve host address." );
				SetState( EKIRCServerState::S_Error );
			}

			delete HostResolver;
			HostResolver = NULL;
			return;

		// Wait for the ip to be resolved and connect the socket.
		case EKIRCServerState::S_Connecting:
			if ( Socket == NULL )
				return;

			switch ( Socket->GetConnectionState() )
			{
				case SCS_NotConnected:
					uint32 iInetIp;
					HostAddr->GetIp( iInetIp );

					// Wait for the host to be resolved.
					if ( iInetIp == 1 )
						return;

					KIRCLog( Log, "Connecting..." );

					if ( !Socket->Connect( *HostAddr ) )
					{
						KIRCLog( Error, "Unable to connect to server." );
						Socket->Close();
						delete Socket;
						Socket = NULL;
						OnConnectionError( "Socket failed to connect." );
						return;
					}

					ReadBuffer = "";
					OnConnected();
					return;

				// Should never reach this point, but just in case.
				case SCS_ConnectionError:
					KIRCLog( Error, "Unable to connect to server." );
					Socket->Close();
					delete Socket;
					Socket = NULL;
					OnConnectionError( "Socket failed to connect." );
					return;

				// Should never reach this point, but just in case.
				case SCS_Connected:
					OnConnected();
					return;
			}

			return;			

		// Read data from the socket into the buffer.
		case EKIRCServerState::S_Connected:
			if ( Socket == NULL )
			{
				KIRCLog( Error, "Null socket in connected state." );
				OnConnectionError( "Socket is null." );
				return;
			}

			switch ( Socket->GetConnectionState() )
			{
				case SCS_NotConnected:
					KIRCLog( Error, "Socket no longer connected." );
					OnConnectionError( "Socket no longer connected." );
					Socket->Close();
					delete Socket;
					Socket = NULL;
					return;

				case SCS_ConnectionError:
					KIRCLog( Error, "Socket error." );
					OnConnectionError( "Socket error." );
					Socket->Close();
					delete Socket;
					Socket = NULL;
					return;

				case SCS_Connected:
					int32 iRead;
					Socket->Recv( SocketBuffer, sizeof( SocketBuffer ), iRead, ESocketReceiveFlags::None );

					if ( iRead == 0 )
						return;

					for ( int32 i = 0; i < iRead; ++i )
					{
						TCHAR Char = static_cast< TCHAR >( SocketBuffer[ i ] );

						if ( Char == '\r' )
							continue;

						if ( Char == '\n' )
						{
							ParseLine( ReadBuffer );
							ReadBuffer = "";
							continue;
						}

						ReadBuffer += Char;
					}

					return;
			}

			return;
	}
}
