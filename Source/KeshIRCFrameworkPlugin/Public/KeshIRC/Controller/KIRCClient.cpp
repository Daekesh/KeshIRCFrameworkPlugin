// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/KIRCNumerics.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCChannel.h"
#include "KeshIRC/Model/KIRCMode.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KeshIRC/Controller/KIRCBlueprintMessageHandler.h"
#include "KeshIRC/Controller/KIRCClient.h"

namespace KeshIRCFramework
{
	FKIRCNumerics Numerics;
	FKIRCInvalidCharacters InvalidCharacters;
}

#define Cmd( Format, ... ) SendCommand( FString::Printf( TEXT( Format ), __VA_ARGS__ ) )
#define CmdScan( ScannerClass, Format, ... ) SendCommandCallback( FString::Printf( TEXT( Format ), __VA_ARGS__ ), ScannerClass )
#define CmdScanCB( ScannerClass, Object, Function, Format, ... ) SendCommandCallback( FString::Printf( TEXT( Format ), __VA_ARGS__ ), ScannerClass, ScannerClass, Object )


FString UKIRCClient::CleanString( const FString& DisallowedCharactes, const FString& String, bool bAllowUpperOctet )
{
	FString Clean = "";

	for ( int32 i = 0; i < String.Len(); ++i )
	{
		// Only octets allowed
		if ( String[ i ] > 255 )
			continue;

		if ( !bAllowUpperOctet && String[ i ] > 127 )
			continue;

		bool bAllowed = true;

		for ( int32 j = 0; j < DisallowedCharactes.Len(); ++j )
		{
			if ( String[ i ] != DisallowedCharactes[ j ] )
				continue;

			bAllowed = false;
			break;
		}

		if ( !bAllowed )
			continue;

		Clean += String[ i ];
	}

	return Clean;
}


const FKIRCInvalidCharacters& UKIRCClient::GetInvalidCharacters()
{
	return KeshIRCFramework::InvalidCharacters;
}


UKIRCClient::UKIRCClient( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Server = NULL;
	bRegistered = false;
	MOTD.SetNum( 0 );
	NickNameList.SetNum( 0 );
	bBuildingModeString = false;
	ModeChangeBuilderModeAction = EKIRCModeChange::M_Add;
	ModeChangeBuilderModeList = "";
	ModeChangeBuilderParamList = "";
	User = NULL;
	UserModes.SetNum( 0 );
}


bool UKIRCClient::InitClient( const FString& ServerName, const FString& Host, int32 Port, const FString& Password,
							  const FString& NickName, const FString& Ident, const FString& RealName,
							  const TArray<FString>& AlternateNickNames )
{
	KIRCLog( Log, "Initiating client." );

	FString CleanNickName = UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().NickName, NickName );

	if ( CleanNickName.Len() < 1 || CleanNickName.Len() > 30 )
	{
		KIRCLog( Error, "Invalid Nickname." );
		return false;
	}

	FString CleanIdent = UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().Ident, Ident );

	if ( CleanIdent.Len() < 1 || CleanIdent.Len() > 30 )
	{
		KIRCLog( Error, "Invalid ident." );
		return false;
	}

	Server = NewObject<UKIRCServer>( this );

	if ( Server == NULL )
	{
		KIRCLog( Error, "Failed to create server." );
		return false;
	}

	Server->InitServer( ServerName, Host, Port, Password );
	Server->Client = this;

	User = Server->EnsureUser( CleanNickName, CleanIdent, "" );
	User->SetRealName( UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().RealName, RealName ) );

	if ( User == NULL )
	{
		KIRCLog( Error, "Failed to create client user." );
		Server = NULL;
		return false;
	}

	NickNameList.Add( CleanNickName );

	for ( const FString& AltNickName : AlternateNickNames )
	{
		FString CleanedAltNickName = UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().NickName, AltNickName );

		if ( CleanedAltNickName.Len() < 1 || CleanedAltNickName.Len() > 30 )
			continue;

		NickNameList.Add( CleanedAltNickName );
	}

	SetupMessageHandlers();
	return true;
}


bool UKIRCClient::HasModeString( const FString& Mode ) const
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Tried to get user mode with null server." );
		return false;
	}
	
	UKIRCMode* ModeObj = Server->GetUserMode( Mode );

	if ( ModeObj == NULL )
		return false;

	return HasMode( ModeObj );
}


const FString& UKIRCClient::GetCachedKeyForChannel( const FString& Channel ) const
{
	static FString NoKey = "";

	if ( Channel.Len() < 2 || UKIRCChannel::HasChannelPrefix( Channel ) )
	{
		KIRCLog( Error, "Tried to get a cached key for a channel with an invalid name." );
		return NoKey;
	}

	if ( !ChannelKeyCache.Contains( Channel ) )
		return NoKey;

	return ChannelKeyCache[ Channel ];
}


void UKIRCClient::HandleMessage( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( MessageHandlers.Contains( Command ) )
		MessageHandlers[ Command ].Broadcast( Source, Command, Params, Message );

	else
	{
		if ( Command.IsNumeric() )
		{
			int32 Numeric = FCString::Atoi( *Command );
			OnUnhandledNumericDelegate.Broadcast( Server, Numeric, Params, Message );
			OnUnhandledNumericEvent( Server, Numeric, Params, Message );
		}

		else
		{
			OnUnhandledRawMessageDelegate.Broadcast( Server, Line );
			OnUnhandledRawMessageEvent( Server, Line );
		}
	}

	CommandScannerQueueLock.Lock();
	{
		if ( CommandResponseScanners.Num() > 0 )
		{
			if ( CommandResponseScanners[ 0 ]->IsComplete() )
			{
				CommandResponseScanners[ 0 ]->ScanComplete();
				CommandResponseScanners.RemoveAt( 0 );

				if ( CommandResponseScanners.Num() > 0 )
					CommandResponseScanners[ 0 ]->StartScan( this );
			}
		}
	}
	CommandScannerQueueLock.Unlock();
}


bool UKIRCClient::SendToServer( const FString& Command )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Tried to send command with null server." );
		return false;
	}

	if ( Server->GetState() != EKIRCServerState::S_Connected )
	{
		KIRCLog( Error, "Tried to send a command with an unconnected server." );
		return false;
	}

	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Tried to send zero-length command." );
		return false;
	}

	return Server->Send( Command );
}


void UKIRCClient::OnConnected()
{
	OnConnectedDelegate.Broadcast( Server );
	OnConnectedEvent( Server );

	Register();
}


void UKIRCClient::OnDisconnected( EKIRCServerDisconnectReason Reason )
{
	OnDisconnectedDelegate.Broadcast( Server, Reason );
	OnDisconnectedEvent( Server, Reason );
}


void UKIRCClient::OnConnectionError( const FString& Error )
{
	OnConnectionErrorDelegate.Broadcast( Server, Error );
	OnConnectionErrorEvent( Server, Error );

	OnDisconnectedDelegate.Broadcast( Server, EKIRCServerDisconnectReason::R_Socket );
	OnDisconnectedEvent( Server, EKIRCServerDisconnectReason::R_Socket );
}


void UKIRCClient::OnRegister()
{
	bRegistered = true;
	OnRegisteredDelegate.Broadcast( this );
	OnRegisteredEvent();
}


void UKIRCClient::Register()
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Tried to register with a null server." );
		return;
	}

	if ( User == NULL )
	{
		KIRCLog( Error, "Tried to register with a null user." );
		return;
	}

	if ( Server->GetState() != EKIRCServerState::S_Connected )
	{
		KIRCLog( Error, "Tried to register with an unconnected server." );
		return;
	}

	if ( bRegistered )
	{
		KIRCLog( Error, "Tried to register when already registered." );
		return;
	}
	
	if ( Server->GetPassword().Len() > 0 )
		Cmd( "PASSWORD %s", *Server->GetPassword() );

	Cmd( "USER %s 8 * :%s", *User->GetIdent(), *User->GetRealName() );
	Cmd( "NICK %s", *User->GetName() );
}


const FKIRCNumerics& UKIRCClient::GetNumerics()
{
	return KeshIRCFramework::Numerics;
}


FString UKIRCClient::NumericToString( int32 Numeric )
{
	FString NumericString = FString::FromInt( Numeric );

	while ( NumericString.Len() < 3 )
		NumericString = "0" + NumericString;

	return NumericString;
}


FDelegateHandle UKIRCClient::AddMessageHandler( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Tried to add a handler for a zero length command." );
		return FDelegateHandle();
	}

	if ( CallbackObject == NULL )
	{
		KIRCLog( Error, "Tried to add a callback on a null objcet." );
		return FDelegateHandle();
	}

	if ( CallbackFunction == NULL )
	{
		KIRCLog( Error, "Tried to add a callback with a null function." );
		return FDelegateHandle();
	}

	FString CommandUpper = Command.ToUpper();

	if ( !MessageHandlers.Contains( CommandUpper ) )
	{
		FKIRCIncomingMessageHandler MessageHandler;
		MessageHandlers.Emplace( CommandUpper, MessageHandler );
	}

	return MessageHandlers[ CommandUpper ].AddUObject( CallbackObject, CallbackFunction );
}


FDelegateHandle UKIRCClient::AddMessageHandler( int32 Numeric, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Numeric < GetNumerics().NumericMin || Numeric > GetNumerics().NumericMax )
	{
		KIRCLog( Error, "Tried to add an out of range numeric handler." );
		return FDelegateHandle();
	}

	return AddMessageHandler( NumericToString( Numeric ), CallbackObject, CallbackFunction );
}


void UKIRCClient::RemoveMessageHandler( const FString& Command, FDelegateHandle Handle )
{
	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Tried to remove a handler for a zero length command." );
		return;
	}

	if ( !Handle.IsValid() )
	{
		KIRCLog( Error, "Tried to remove an invalid delegate handle." );
		return;
	}

	FString CommandUpper = Command.ToUpper();

	if ( !MessageHandlers.Contains( CommandUpper ) )
	{
		KIRCLog( Error, "Tried to remove a handler for command that isn't being handled." );
		return;
	}

	MessageHandlers[ CommandUpper ].Remove( Handle );

	if ( !MessageHandlers[ CommandUpper ].IsBound() )
		MessageHandlers.Remove( CommandUpper );
}


void UKIRCClient::RemoveMessageHandler( int32 Numeric, FDelegateHandle Handle )
{
	if ( Numeric < GetNumerics().NumericMin || Numeric > GetNumerics().NumericMax )
	{
		KIRCLog( Error, "Tried to remove an out of range numeric handler." );
		return;
	}

	RemoveMessageHandler( NumericToString( Numeric ), Handle );
}


UKIRCBlueprintMessageHandler* UKIRCClient::CreateMessageHandler( UKIRCClient* Client, TSubclassOf<UKIRCBlueprintMessageHandler> MessageHandlerClass,
																 bool bAutoRegister, bool bStoreReference )
{
	if ( MessageHandlerClass == NULL )
	{
		KIRCLog( Error, "Tried to create a blueprint message handler with a null class." );
		return NULL;
	}

	if ( MessageHandlerClass->GetClassFlags() & CLASS_Abstract )
	{
		KIRCLog( Error, "Tried to create a blueprint message handler with an abstract class." );
		return NULL;
	}

	const UKIRCBlueprintMessageHandler* const CDO = MessageHandlerClass->GetDefaultObject<UKIRCBlueprintMessageHandler>();

	if ( CDO == NULL )
	{
		KIRCLog( Error, "Unable to fetch message handler default object." );
		return NULL;
	}

	if ( CDO->GetCommand().Len() == 0 )
	{
		KIRCLog( Error, "Tried to create a message handler with no command set. Set the command in the class default properties." );
		return NULL;
	}

	UKIRCBlueprintMessageHandler* NewMessageHandler = NewObject<UKIRCBlueprintMessageHandler>( Client, MessageHandlerClass );

	if ( NewMessageHandler == NULL )
	{
		KIRCLog( Error, "Failed to create message handler." );
		return NULL;
	}

	if ( bAutoRegister )
		NewMessageHandler->RegisterHandler( Client );

	if ( bStoreReference )
		Client->BlueprintMessageHandlers.Add( NewMessageHandler );

	return NewMessageHandler;
}


void UKIRCClient::SetupMessageHandlers()
{
	// Nickname negotiation handlers
	RegistrationNickErrorNoneGiven = AddMessageHandler( GetNumerics().ErrorNoNicknameGiven, this, static_cast< FKIRCIncomingMessageHandlerDelegate >( &UKIRCClient::OnNickNegotiationFatalHandler ) );
	RegistrationNickErrorNickInUse = AddMessageHandler( GetNumerics().ErrorNickNameInUse, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RegistrationNickErrorUnavailableResource = AddMessageHandler( GetNumerics().ErrorUnAvailResource, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RegistrationNickErrorErroneousNick = AddMessageHandler( GetNumerics().ErrorErroneusNickname, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RegistrationNickErrorNickCollision = AddMessageHandler( GetNumerics().ErrorNickCollision, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RegistrationNickErrorRestricted = AddMessageHandler( GetNumerics().ErrorRestricted, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );

	// Network info
	AddMessageHandler( GetNumerics().ReplyWelcome, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNetworkWelcomeHandler ) );
	AddMessageHandler( GetNumerics().ReplyMyInfo, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNetworkInfoHandler ) );
	AddMessageHandler( GetNumerics().ReplyMap, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnServerSettingHandler ) );

	// MOTD
	AddMessageHandler( GetNumerics().ReplyMotdStart, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnMOTDStartHandler ) );
	AddMessageHandler( GetNumerics().ReplyMotd, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnMOTDLineHandler ) );
	AddMessageHandler( GetNumerics().ReplyMotdEnd, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnMOTDEndHandler ) );
	AddMessageHandler( GetNumerics().ErrorNoMotd, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNoMOTDHandler ) );

	// Channel joining
	AddMessageHandler( GetNumerics().ReplyTopic, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnTopicBodyHandler ) );
	AddMessageHandler( GetNumerics().ReplyTopicSetBy, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnTopicDetailsHandler ) );
	AddMessageHandler( GetNumerics().ReplyNamReply, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNameListHandler ) );

	// General operation
	AddMessageHandler( "PRIVMSG", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnMessageHandler ) );
	AddMessageHandler( "NOTICE", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnMessageHandler ) );
	AddMessageHandler( "NICK", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickChangeHandler ) );
	AddMessageHandler( "JOIN", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnJoinHandler ) );
	AddMessageHandler( "PART", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnPartHandler ) );
	AddMessageHandler( "MODE", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnModeChangeHandler ) );
	AddMessageHandler( "TOPIC", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnTopicChangeHandler ) );
	AddMessageHandler( "KICK", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnKickHandler ) );
	AddMessageHandler( "INVITE", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnInviteHandler ) );
	AddMessageHandler( "QUIT", this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnQuitHandler ) );
}


void UKIRCClient::OnNickNegotiationFatalHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	KIRCLog( Error, "Critical error negotiating nickname." );

	if ( Server == NULL )
	{
		KIRCLog( Error, "Unable to disconnect from null server." );
		return;
	}

	Server->Disconnect();
}


void UKIRCClient::OnNickNegotaitionNextNickHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	KIRCLog( Warning, "Failed to set nickname. Trying next available." );

	if ( User == NULL )
	{
		KIRCLog( Error, "Tried to scan for new nickname with null user." );

		if ( Server == NULL )
		{
			KIRCLog( Error, "Unable to disconnect from null server." );
			return;
		}

		Server->Disconnect();
		return;
	}
	
	int32 iNickIndex = INDEX_NONE;

	for ( int32 i = 0; i < NickNameList.Num(); ++i )
	{
		if ( User->GetName() != NickNameList[ i ] )
			continue;

		iNickIndex = i;
		break;
	}

	if ( iNickIndex == INDEX_NONE )
		User->SetName( NickNameList[ 0 ] );

	else
	{
		++iNickIndex;

		if ( iNickIndex < NickNameList.Num() )
			User->SetName( NickNameList[ iNickIndex ] );

		else
		{
			KIRCLog( Warning, "Unable to use any of the given nicknames; generating random one." );
			User->SetName( "KIRC" + FString::FromInt( rand() ) );
		}
	}	

	Cmd( "NICK %s", *User->GetName() );
}


void UKIRCClient::OnNetworkWelcomeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received network welcome with null server." );
		return;
	}

	// Remove nick negotiation handlers
	if ( RegistrationNickErrorNoneGiven.IsValid() )
		RemoveMessageHandler( GetNumerics().ErrorNoNicknameGiven, RegistrationNickErrorNoneGiven );

	if ( RegistrationNickErrorNickInUse.IsValid() )
		RemoveMessageHandler( GetNumerics().ErrorNickNameInUse, RegistrationNickErrorNickInUse );

	if ( RegistrationNickErrorUnavailableResource.IsValid() )
		RemoveMessageHandler( GetNumerics().ErrorUnAvailResource, RegistrationNickErrorUnavailableResource );

	if ( RegistrationNickErrorErroneousNick.IsValid() )
		RemoveMessageHandler( GetNumerics().ErrorErroneusNickname, RegistrationNickErrorErroneousNick );
	
	if ( RegistrationNickErrorNickCollision.IsValid() )
		RemoveMessageHandler( GetNumerics().ErrorNickCollision, RegistrationNickErrorNickCollision );
	
	if ( RegistrationNickErrorRestricted.IsValid() )
		RemoveMessageHandler( GetNumerics().ErrorRestricted, RegistrationNickErrorRestricted );
	
	FString MessageChop = Message;
	
	if ( MessageChop.StartsWith( "Welcome to the " ) )
		MessageChop = MessageChop.RightChop( 15 );

	int32 iLastSpace = INDEX_NONE;
	MessageChop.FindLastChar( ' ', iLastSpace );

	if ( iLastSpace == INDEX_NONE )
	{
		Server->SetNetworkName( MessageChop );
		return;
	}

	// What?
	if ( iLastSpace == 0 )
	{
		KIRCLog( Error, "Received zero length welcome message." );
		return;
	}

	if ( MessageChop[ iLastSpace - 1 ] == ',' )
		--iLastSpace;

	Server->SetNetworkName( MessageChop.Left( iLastSpace ) );
}


void UKIRCClient::OnNetworkInfoHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Network info received with a null server." );
		return;
	}

	if ( Params.Num() < 5 )
	{
		KIRCLog( Error, "Not enough params in network info." );
		return;
	}

	Server->SetHostActual( Params[ 0 ] );
	Server->SetVersion( Params[ 1 ] );	

	for ( int32 i = 0; i < Params[ 2 ].Len(); ++i )
		Server->AddUserMode( FString( 1, &Params[ 2 ][ i ] ) );

	for ( int32 i = 0; i < Params[ 3 ].Len(); ++i )
	{
		const TCHAR& ModeChar = Params[ 3 ][ i ];
		bool bIsParamMode = false;

		for ( int32 j = 0; j < Params[ 4 ].Len(); ++j )
		{
			const TCHAR& ParamChar = Params[ 4 ][ j ];

			if ( ModeChar != ParamChar )
				continue;

			bIsParamMode = true;
			break;
		}

		if ( bIsParamMode )
		{
			// Hard core the 2 non-list param chars (key, limit.)
			if ( ModeChar == 'k' || ModeChar == 'l' )
			{
				Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_Param );
				continue;
			}

			// Hardcore the 3 user modes (ops, halfops/helper, voice.)
			if ( ModeChar == 'o' || ModeChar == 'h' || ModeChar == 'v' )
			{
				Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_User );
				continue;
			}

			// Otherwise we're a list mode (ban, etc.)
			Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_List );
			continue;
		}

		Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_Unary );
	}
}


void UKIRCClient::OnServerSettingHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Network settings received with a null server." );
		return;
	}

	for ( FString Param : Params )
	{
		int32 iEquals = INDEX_NONE;
		Param.FindChar( '=', iEquals );

		if ( iEquals == INDEX_NONE )
			Server->SetSetting( Param, "" );

		else
			Server->SetSetting( Param.Left( iEquals ), Param.Mid( iEquals + 1 ) );
	}
}


void UKIRCClient::OnMOTDStartHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	MOTD.SetNum( 0 );
}


void UKIRCClient::OnMOTDLineHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	MOTD.Add( Message );
}


void UKIRCClient::OnMOTDEndHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	OnMOTDCompleteDelegate.Broadcast( this, MOTD );
	OnMOTDCompleteEvent( MOTD );

	if ( !bRegistered )
		OnRegister();
}


void UKIRCClient::OnNoMOTDHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	MOTD.SetNum( 0 );
	OnMOTDCompleteDelegate.Broadcast( this, MOTD );
	OnMOTDCompleteEvent( MOTD );

	if ( !bRegistered )
		OnRegister();
}


void UKIRCClient::OnMessageHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Received message with no target." );
		return;
	}

	if ( Message.Len() == 0 )
	{
		KIRCLog( Error, "Received zero length message." );
		return;
	}

	UKIRCChannel* Channel = NULL;

	if ( UKIRCChannel::HasChannelPrefix( Params[ 0 ] ) )
		Channel = Server->GetChannelByName( Params[ 0 ] );

	if ( Command == "PRIVMSG" )
	{
		if ( Message.Len() >= 2 && Message[ 0 ] == '\1' || Message[ Message.Len() - 1 ] == '\1' )
		{
			OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Emote, Message.Mid( 1, Message.Len() - 2 ) );
			OnMessageEvent( Source, Channel, EKIRCMessageType::T_Emote, Message.Mid( 1, Message.Len() - 2 ) );
			
			if ( Source != NULL )
				Source->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Emote, Message.Mid( 1, Message.Len() - 2 ) );

			if ( Channel != NULL )
				Channel->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Emote, Message.Mid( 1, Message.Len() - 2 ) );
		}

		else
		{
			OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Message, Message );
			OnMessageEvent( Source, Channel, EKIRCMessageType::T_Message, Message );

			if ( Source != NULL )
				Source->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Message, Message );

			if ( Channel != NULL )
				Channel->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Message, Message );
		}
	}

	else if ( Command == "NOTICE" )
	{
		if ( Message.Len() >= 2 && Message[ 0 ] == '\1' || Message[ Message.Len() - 1 ] == '\1' )
		{
			OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_CTCP, Message.Mid( 1, Message.Len() - 2 ) );
			OnMessageEvent( Source, Channel, EKIRCMessageType::T_CTCP, Message.Mid( 1, Message.Len() - 2 ) );

			if ( Source != NULL )
				Source->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_CTCP, Message.Mid( 1, Message.Len() - 2 ) );

			if ( Channel != NULL )
				Channel->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_CTCP, Message.Mid( 1, Message.Len() - 2 ) );
		}

		else
		{
			OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Notice, Message );
			OnMessageEvent( Source, Channel, EKIRCMessageType::T_Notice, Message );

			if ( Source != NULL )
				Source->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Notice, Message );

			if ( Channel != NULL )
				Channel->OnMessageDelegate.Broadcast( Source, Channel, EKIRCMessageType::T_Notice, Message );
		}
	}
}


void UKIRCClient::OnNickChangeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Source == NULL )
	{
		KIRCLog( Error, "Null user trying to change nickname." );
		return;
	}

	if ( Message.Len() == 0 )
	{
		KIRCLog( Error, "Nickname change but no nickname provided." );
		return;
	}

	if ( Server == NULL )
	{
		KIRCLog( Error, "Trying to rename a user on a null server." );
		return;
	}

	FString OldNick = Source->GetName();

	Server->RenameUser( Source, Message );
	OnNickNameChangedDelegate.Broadcast( Source, OldNick );
	OnNickNameChangedEvent( Source, OldNick );
	Source->OnNickNameChangedDelegate.Broadcast( Source, OldNick );
}


void UKIRCClient::OnJoinHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Received join with no target." );
		return;
	}

	if ( Source == NULL )
	{
		KIRCLog( Error, "Null user trying to join a channel (server?)" );
		return;
	}

	UKIRCChannel* Channel = Server->GetChannelByName( Params[ 0 ] );

	if ( Channel == NULL )
	{
		if ( Source == User )
		{
			Channel = Server->EnsureChannel( Params[ 0 ] );

			if ( Channel == NULL )
			{
				KIRCLog( Error, "Error ensuring channel existence." );
				return;
			}

			// Just in case
			TArray<UKIRCUser*> Users = Channel->GetUsers();

			for ( UKIRCUser* User : Users )
				Channel->UserLeft( User );
		}

		else
		{
			KIRCLog( Error, "User trying to join a null channel." );
			return;
		}
	}

	Channel->UserJoined( Source );
	OnJoinDelegate.Broadcast( Channel, Source );
	OnJoinEvent( Channel, User );
	Channel->OnJoinedDelegate.Broadcast( Channel, User );

	if ( Source == User )
		QueryObjectModes( Channel );
}


void UKIRCClient::OnPartHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Received part with no target." );
		return;
	}

	if ( Source == NULL )
	{
		KIRCLog( Error, "Null user trying to join a channel (server?)." );
		return;
	}

	UKIRCChannel* Channel = Server->GetChannelByName( Params[ 0 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "User trying to join a null channel." );
		return;
	}

	Channel->UserLeft( Source );

	if ( Source == User )
	{
		if ( Server == NULL )
		{
			KIRCLog( Error, "Trying to rename a channel on a null server." );
		}

		else
			Server->RemoveChannel( Channel );
	}

	OnPartDelegate.Broadcast( Channel, Source, Message );
	OnPartEvent( Channel, Source, Message );
	Channel->OnPartedDelegate.Broadcast( Channel, Source, Message );
}


void UKIRCClient::OnModeChangeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Mode change received with no target." );
		return;
	}

	FString ModeParamList = Message;
	ModeParamList = ModeParamList.Trim().TrimTrailing();

	TArray<FString> ModeParams;
	ModeParamList.ParseIntoArray( ModeParams, TEXT( " " ), true );

	if ( ModeParams.Num() == 0 )
	{
		KIRCLog( Error, "Mode change received with no modes." );
		return;
	}

	if ( Server == NULL )
	{
		KIRCLog( Error, "Trying to change mode on a null server." );
		return;
	}

	if ( UKIRCChannel::HasChannelPrefix( Params[ 0 ] ) )
	{
		UKIRCChannel* Channel = Server->GetChannelByName( Params[ 0 ] );

		if ( Channel == NULL )
		{
			KIRCLog( Error, "Mode change on a null channel." );
			return;
		}

		FString ModeString = ModeParams[ 0 ];
		int32 iCurrentParam = 1;
		EKIRCModeChange ModeChange = EKIRCModeChange::M_Add;
		TCHAR ModeChar;
		UKIRCMode* Mode;

		for ( int32 i = 0; i < ModeString.Len(); ++i )
		{
			ModeChar = ModeString[ i ];

			if ( ModeChar == '+' )
			{
				ModeChange = EKIRCModeChange::M_Add;
				continue;
			}

			if ( ModeChar == '-' )
			{
				ModeChange = EKIRCModeChange::M_Remove;
				continue;
			}

			Mode = Server->GetChannelMode( FString( 1, &ModeChar ) );

			if ( Mode == NULL )
			{
				KIRCLog( Error, "Unknown mode in mode change." );
				return;
			}

			if ( Mode->GetType() == EKIRCModeType::T_Channel_User )
			{
				// All user mode changes require a param
				if ( iCurrentParam == ModeParams.Num() )
				{
					KIRCLog( Error, "Not enough params in mode string." );
					return;
				}

				UKIRCUser* Target = Server->EnsureUser( ModeParams[ iCurrentParam ] );
				++iCurrentParam;

				if ( Target == NULL )
				{
					KIRCLog( Error, "Error ensuring user for mode change." );
					return;
				}
				
				if ( !Channel->HasUser( Target ) )
					Channel->UserJoined( Target );

				if ( ModeChange == EKIRCModeChange::M_Add )
					Channel->AddUserMode( Target, Mode );

				else
					Channel->RemoveUserMode( Target, Mode );

				OnChannelUserModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, Target );
				OnChannelUserModeEvent( Channel, Source, Mode, ModeChange, Target );
				Channel->OnChannelUserModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, Target );
				Target->OnChannelUserModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, Target );
			}

			else if ( Mode->GetType() == EKIRCModeType::T_Channel_List )
			{
				// All list mode changes require a param
				if ( iCurrentParam == ModeParams.Num() )
				{
					KIRCLog( Error, "Not enough params in mode string." );
					return;
				}

				if ( ModeChange == EKIRCModeChange::M_Add )
					Channel->AddListModeEntry( Mode, ModeParams[ iCurrentParam ] );

				else
					Channel->RemoveListModeEntry( Mode, ModeParams[ iCurrentParam ] );

				OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
				OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
				Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
				++iCurrentParam;
			}

			else if ( Mode->GetType() == EKIRCModeType::T_Channel_Param )
			{
				if ( Mode->GetMode() == MODE_CHANNEL_KEY )
				{
					// Key always requires a param to be given
					if ( iCurrentParam == ModeParams.Num() )
					{
						KIRCLog( Error, "Not enough params in mode string." );
						return;
					}

					if ( ModeChange == EKIRCModeChange::M_Add )
					{
						Channel->AddUnaryMode( Mode );
						Channel->SetJoinKey( ModeParams[ iCurrentParam ] );
						ChannelKeyCache.Emplace( Channel->GetName(), ModeParams[ iCurrentParam ] );
					}

					else
					{
						Channel->RemoveUnaryMode( Mode );
						Channel->SetJoinKey( "" );
						ChannelKeyCache.Remove( Channel->GetName() );
					}

					OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
					OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
					Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
					++iCurrentParam;
				}

				else if ( Mode->GetMode() == MODE_CHANNEL_USER_LIMIT )
				{
					if ( ModeChange == EKIRCModeChange::M_Add )
					{
						// Limit only needs the param when it's added
						if ( iCurrentParam == ModeParams.Num() )
						{
							KIRCLog( Error, "Not enough params in mode string." );
							return;
						}

						if ( !ModeParams[ iCurrentParam ].IsNumeric() )
						{
							KIRCLog( Error, "Limit param is not numeric." );
							return;
						}

						int Limit = FCString::Atoi( *ModeParams[ iCurrentParam ] );

						Channel->AddUnaryMode( Mode );
						Channel->SetLimit( Limit );
						OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						++iCurrentParam;
					}

					else
					{
						Channel->RemoveUnaryMode( Mode );
						Channel->SetLimit( 0 );
						OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, "0" );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, "0" );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, "0" );
					}
				}

				else
				{
					// Unknwown mode, assume it operates like user limit
					if ( ModeChange == EKIRCModeChange::M_Add )
					{
						if ( iCurrentParam == ModeParams.Num() )
						{
							KIRCLog( Error, "Not enough params in mode string." );
							return;
						}

						Channel->AddUnaryMode( Mode );
						OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						++iCurrentParam;
					}

					else
					{
						Channel->RemoveUnaryMode( Mode );
						OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, "" );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, "" );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, "" );
					}
				}
			}

			// Unary
			else
			{
				if ( ModeChange == EKIRCModeChange::M_Add )
					Channel->AddUnaryMode( Mode );

				else
					Channel->RemoveUnaryMode( Mode );
				
				OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, "" );
				OnChannelModeEvent( Channel, Source, Mode, ModeChange, "" );
				Channel->OnChannelModeDelegate.Broadcast( Channel, Source, Mode, ModeChange, "" );
			}
		}
	}

	else
	{
		UKIRCUser* User = Server->GetUserByName( Params[ 0 ] );

		if ( User == NULL )
		{
			KIRCLog( Error, "Unable to ensure user object." );
			return;
		}

		// Don't care about others' modes.
		if ( User != this->User )
			return;

		FString ModeString = ModeParams[ 0 ];
		int32 iCurrentParam = 2;
		EKIRCModeChange ModeChange = EKIRCModeChange::M_Add;
		TCHAR ModeChar;
		UKIRCMode* Mode;

		for ( int32 i = 0; i < ModeString.Len(); ++i )
		{
			ModeChar = ModeString[ i ];

			if ( ModeChar == '+' )
			{
				ModeChange = EKIRCModeChange::M_Add;
				continue;
			}

			if ( ModeChar == '-' )
			{
				ModeChange = EKIRCModeChange::M_Remove;
				continue;
			}

			Mode = Server->GetUserMode( FString( 1, &ModeChar ) );

			if ( Mode == NULL )
			{
				KIRCLog( Error, "Unknown mode in mode change." );
				return;
			}

			// All user modes are unary
			if ( ModeChange == EKIRCModeChange::M_Add )
			{
				if ( UserModes.Contains( Mode ) )
				{
					KIRCLog( Error, "Trying to add user mode that server user already has." );
					return;
				}

				UserModes.Add( Mode );
			}

			else
			{
				if ( !UserModes.Contains( Mode ) )
				{
					KIRCLog( Error, "Trying to remove user mode that server does not have." );
					return;
				}

				UserModes.Remove( Mode );
			}

			OnUserModeDelegate.Broadcast( User, Mode, ModeChange );
			OnUserModeEvent( User, Mode, ModeChange );
			User->OnUserModeDelegate.Broadcast( User, Mode, ModeChange );
		}
	}
}


void UKIRCClient::OnTopicChangeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Received topic change with no target." );
		return;
	}

	UKIRCChannel* Channel = Server->EnsureChannel( Params[ 0 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Unable to ensure channel object." );
		return;
	}

	Channel->SetTopicBody( Message );
	Channel->SetTopicAuthor( Source != NULL ? Source->GetName() : "" );
	Channel->SetTopicDate( FDateTime::Now() );
	OnTopicChangeDelegate.Broadcast( Channel, Source, Message );
	OnTopicChangeEvent( Channel, Source, Message );
	Channel->OnTopicChangedDelegate.Broadcast( Channel, Source, Message );

	if ( Source != NULL )
		Source->OnTopicChangedDelegate.Broadcast( Channel, Source, Message );
}


void UKIRCClient::OnKickHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Received kick with insufficient params." );
		return;
	}

	UKIRCChannel* Channel = Server->EnsureChannel( Params[ 0 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Unable to ensure channel object." );
		return;
	}

	UKIRCUser* Target = Server->EnsureUser( Params[ 1 ] );

	if ( Target == NULL )
	{
		KIRCLog( Error, "Unable to ensure user object." );
		return;
	}

	Channel->UserLeft( Target );
	OnKickDelegate.Broadcast( Channel, Source, Target, Message );
	OnKickEvent( Channel, Source, Target, Message );
	Channel->OnKickedDelegate.Broadcast( Channel, Source, Target, Message );
	
	if ( Source != NULL )
		Source->OnKickDelegate.Broadcast( Channel, Source, Target, Message );

	Target->OnKickedDelegate.Broadcast( Channel, Source, Target, Message );
}


void UKIRCClient::OnInviteHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received invit with a null server." );
		return;
	}

	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Received invite with insufficient params." );
		return;
	}

	OnInvitedDelegate.Broadcast( Source, Params[ 1 ] );
	OnInvitedEvent( Source, Params[ 1 ] );

	UKIRCUser* User = Server->GetUserByName( Params[ 0 ] );

	if ( User != NULL )
		User->OnInvitedDelegate.Broadcast( Source, Params[ 1 ] );

	UKIRCChannel* Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel != NULL )
		Channel->OnInvitedDelegate.Broadcast( Source, Params[ 1 ] );
}


void UKIRCClient::OnQuitHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Source == NULL )
	{
		KIRCLog( Error, "Null user trying to quit (server?)." );
		return;
	}

	TArray<UKIRCChannel*> Channels = Source->GetChannels();

	for ( int32 i = Channels.Num() - 1; i >= 0; --i )
		Channels[ i ]->UserLeft( Source );

	OnQuitDelegate.Broadcast( Source, Message );
	OnQuitEvent( Source, Message );

	if ( Source != NULL )
		Source->OnQuitDelegate.Broadcast( Source, Message );

	if ( Source == User )
		Server->Disconnect();
}


void UKIRCClient::OnTopicBodyHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received topic body with null server." );
	}
	
	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Received topic body with no channel." );
		return;
	}

	UKIRCChannel* Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Topic body received, but cannot find channel." );
		return;
	}

	Channel->SetTopicBody( Message );
	OnTopicReceiveDelegate.Broadcast( Channel, Message );
	Channel->OnTopicReceiveDelegate.Broadcast( Channel, Message );
}


void UKIRCClient::OnTopicDetailsHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received topic details with null server." );
	}

	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Received topic details with no channel." );
		return;
	}

	UKIRCChannel* Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Topic details received, but cannot find channel." );
		return;
	}

	if ( !Params[ 3 ].IsNumeric() )
	{
		KIRCLog( Error, "Topic details received, but time is not numeric." );
		return;
	}

	FString AuthorName = "";
	FString AuthorIdent = "";
	FString AuthorHost = "";

	UKIRCUser::ParseHostMask( Params[ 2 ], AuthorName, AuthorIdent, AuthorHost );

	int32 iDate = FCString::Atoi( *Params[ 3 ] );
	FDateTime TopicDate = FDateTime::FromUnixTimestamp( iDate );

	Channel->SetTopicAuthor( AuthorName );
	Channel->SetTopicDate( TopicDate );
	
	OnTopicDetailsDelegate.Broadcast( Channel, AuthorName, TopicDate, AuthorIdent, AuthorHost, Params[ 2 ] );
	Channel->OnTopicDetailsDelegate.Broadcast( Channel, AuthorName, TopicDate, AuthorIdent, AuthorHost, Params[ 2 ] );
}


void UKIRCClient::OnNameListHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received channel name list with null server." );
	}

	if ( Params.Num() < 3 )
	{
		KIRCLog( Error, "Received channel name list with no channel." );
		return;
	}

	UKIRCChannel* Channel = Server->GetChannelByName( Params[ 2 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Channel names list received, but cannot find channel." );
		return;
	}

	UKIRCMode* Private = Server->GetChannelMode( MODE_CHANNEL_PRIVATE );
	UKIRCMode* Secret = Server->GetChannelMode( MODE_CHANNEL_SECRET );

	if ( Params[ 1 ] == "*" )
	{
		if ( Private != NULL && !Channel->IsChannelModeSet( Private ) )
			Channel->AddUnaryMode( Private );
	}

	else if ( Params[ 1 ] == "@" )
	{
		if ( Secret != NULL && !Channel->IsChannelModeSet( Secret ) )
			Channel->AddUnaryMode( Secret );
	}

	UKIRCMode* Ops = Server->GetChannelMode( MODE_CHANNEL_USER_OP );
	UKIRCMode* HalfOps = Server->GetChannelMode( MODE_CHANNEL_USER_HALFOP );
	UKIRCMode* Voice = Server->GetChannelMode( MODE_CHANNEL_USER_VOICE );

	TArray<FString> Names;
	Message.ParseIntoArray( Names, TEXT( " " ) );

	for ( FString& Name : Names )
	{
		int32 iNameStartIndex;
		TArray<UKIRCMode*> Modes;

		for ( iNameStartIndex = 0; iNameStartIndex < Name.Len(); ++iNameStartIndex )
		{
			if ( Name[ iNameStartIndex ] == TAG_USER_OP )
			{
				if ( Ops != NULL )
					Modes.Add( Ops );

				continue;
			}

			if ( Name[ iNameStartIndex ] == TAG_USER_HALFOP )
			{
				if ( HalfOps != NULL )
					Modes.Add( HalfOps );

				continue;
			}

			if ( Name[ iNameStartIndex ] == TAG_USER_VOICE )
			{
				if ( Voice != NULL )
					Modes.Add( Voice );

				continue;
			}

			break;
		}

		UKIRCUser* User = Server->EnsureUser( Name.Mid( iNameStartIndex ) );

		if ( User == NULL )
		{
			KIRCLog( Error, "Unable to ensure user's existence." );
			return;
		}

		if ( !User->IsInChannel( Channel ) )
			Channel->UserJoined( User, User != this->User ); // We don't know when other users joined the channel

		for ( UKIRCMode* Mode : Modes )
			Channel->AddUserMode( User, Mode );
	}
}


bool UKIRCClient::SendCommand( const FString& Command, UKIRCCommandResponseScanner* ResponseScanner )
{
	if ( ResponseScanner != NULL )
	{
		CommandScannerQueueLock.Lock();
		CommandResponseScanners.Add( ResponseScanner );

		if ( CommandResponseScanners.Num() == 1 )
			ResponseScanner->StartScan( this );

		CommandScannerQueueLock.Unlock();
	}

	return SendToServer( Command.Left( UKIRCServer::MaxCommandLength ) );
}


bool UKIRCClient::SendCommandCallback( const FString& Command, TSubclassOf<UKIRCCommandResponseScanner> ScannerClass,
						   UObject* CallbackObject, FKIRCCommandResponseCallbackDelegate CallbackFunction )
{
	UKIRCCommandResponseScanner* ResponseScanner = NULL;

	if ( ScannerClass != NULL )
	{
		if ( ScannerClass->GetClassFlags() & CLASS_Abstract )
		{
			KIRCLog( Error, "Tried to instantiate an abstract scanner class." );
			return false;
		}

		ResponseScanner = NewObject<UKIRCCommandResponseScanner>( this, ScannerClass );

		if ( ResponseScanner == NULL )
		{
			KIRCLog( Error, "Unable to instantiate scanner response class." );
			return false;
		}

		if ( CallbackObject != NULL && CallbackFunction != NULL )
			ResponseScanner->OnScanComplete.BindUObject( CallbackObject, CallbackFunction );
	}

	return this->SendCommand( Command, ResponseScanner );
}


bool UKIRCClient::Message( const FString& Target, EKIRCMessageType Type, const FString& Message )
{
	if ( Target.Len() == 0 )
	{
		KIRCLog( Error, "Trying to send a message to a zero-length." );
		return false;
	}

	if ( UKIRCChannel::HasChannelPrefix( Target ) && Target.Len() == 1 )
	{
		KIRCLog( Error, "Trying to send to channel with just a prefix." );
		return false;
	}

	if ( Message.Len() == 0 )
	{
		KIRCLog( Error, "Cannot send zero-length messages." );
		return false;
	}

	switch ( Type )
	{
		default:
		case EKIRCMessageType::T_Message:
			return Cmd( "PRIVMSG %s :%s", *Target, *Message.Left( UKIRCServer::MaxCommandLength - 10 - Target.Len() ) );

		case EKIRCMessageType::T_Emote:
			return Cmd( "PRIVMSG %s :\1%s\1", *Target, *Message.Left( UKIRCServer::MaxCommandLength - 12 - Target.Len() ) );

		case EKIRCMessageType::T_Notice:
			return Cmd( "NOTICE %s :%s", *Target, *Message.Left( UKIRCServer::MaxCommandLength - 9 - Target.Len() ) );

		case EKIRCMessageType::T_CTCP:
			return Cmd( "NOTICE %s :\1%s\1", *Target, *Message.Left( UKIRCServer::MaxCommandLength - 11 - Target.Len() ) );
	}
}


bool UKIRCClient::UserMode( UKIRCMode* Mode, EKIRCModeChange Change )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "User is null." );
		return false;
	}

	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to check if the server user has a null mode." );
		return false;
	}

	return Cmd( "MODE %s %s%s", *User->GetName(), *( Change == EKIRCModeChange::M_Add ? "+" : "-" ), *Mode->GetMode() );
}


bool UKIRCClient::JoinChannel( const FString& Channel, const FString& Key )
{
	if ( Channel.Len() < 2 || !UKIRCChannel::HasChannelPrefix( Channel ) )
	{
		KIRCLog( Error, "Tried to join a channel with an invalid name." );
		return false;
	}

	FString ActualKey = Key;

	if ( ActualKey == "" && ChannelKeyCache.Contains( Channel ) )
		ActualKey = ChannelKeyCache[ Channel ];

	if ( ActualKey.Len() == 0 )
		return Cmd( "JOIN %s", *Channel );

	else
	{
		return Cmd( "JOIN %s %s", *Channel, *ActualKey );
		ChannelKeyCache.Emplace( Channel, ActualKey );
	}
}


bool UKIRCClient::PartChannel( UKIRCChannel* Channel, const FString& Message )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to part a null channel." );
		return false;
	}

	if ( Message.Len() == 0 )
		return Cmd( "PART %s", *Channel->GetName() );

	else
		return Cmd( "PART %s :%s", *Channel->GetName(), *Message.Left( UKIRCServer::MaxCommandLength - 7 - Channel->GetName().Len() ) );
}


bool UKIRCClient::InviteUserToChannel( UKIRCUser* User, UKIRCChannel* Channel )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Tried to invite a null user." );
		return false;
	}

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to invite to a null channel." );
		return false;
	}

	return Cmd( "INVITE %s %s", *User->GetName(), *Channel->GetName() );
}


bool UKIRCClient::KickUserFromChannel( UKIRCChannel* Channel, UKIRCUser* User, const FString& Message )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to kick a user from a null channel." );
		return false;
	}

	if ( User == NULL )
	{
		KIRCLog( Error, "Tried to kick a null user from a channel." );
		return false;
	}

	if ( Message.Len() == 0 )
		return Cmd( "KICK %s %s", *Channel->GetName(), *User->GetName() );

	else
		return Cmd( "KICK %s %s :%s", *Channel->GetName(), *User->GetName(), *Message.Left( UKIRCServer::MaxCommandLength - 8 - Channel->GetName().Len() - User->GetName().Len() ) );
}


void UKIRCClient::StartModeChangeBuilder()
{
	bBuildingModeString = true;
	ModeChangeBuilderModeAction = EKIRCModeChange::M_Add;
	ModeChangeBuilderModeList = "";
	ModeChangeBuilderParamList = "";
}


bool UKIRCClient::ChangeChannelMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Tried to change a null mode." );
		return false;
	}

	if ( !bBuildingModeString )
	{
		if ( Channel == NULL )
		{
			KIRCLog( Error, "Tried to change channel mode of a null channel." );
			return false;
		}

		return Cmd(
			"MODE %s %s%s",
			*Channel->GetName(),
			*( ModeChange == EKIRCModeChange::M_Add ? "+" : "-" ),
			*Mode->GetMode()
		);

		return false;
	}

	if ( ( ModeChangeBuilderModeList.Len() + ModeChangeBuilderParamList.Len() + 9 ) >= UKIRCServer::MaxCommandLength )
	{
		FlushModeChanges( Channel );
		StartModeChangeBuilder();
	}

	if ( ModeChangeBuilderModeList.Len() == 0 || ModeChange != ModeChangeBuilderModeAction )
		ModeChangeBuilderModeList += ModeChange == EKIRCModeChange::M_Add ? "+" : "-";

	ModeChangeBuilderModeList += Mode->GetMode();
	return true;
}


bool UKIRCClient::ChangeChannelParamMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange, const FString& Param )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Tried to change a null mode." );
		return false;
	}

	if ( !bBuildingModeString )
	{
		if ( Channel == NULL )
		{
			KIRCLog( Error, "Tried to change channel mode of a null channel." );
			return false;
		}

		return Cmd(
			"MODE %s %s%s %s",
			*Channel->GetName(),
			*( ModeChange == EKIRCModeChange::M_Add ? "+" : "-" ),
			*Mode->GetMode(),
			*Param
		);

		return false;
	}

	if ( ( ModeChangeBuilderModeList.Len() + ModeChangeBuilderParamList.Len() + 9 + Param.Len() ) >= UKIRCServer::MaxCommandLength )
	{
		FlushModeChanges( Channel );
		StartModeChangeBuilder();
	}

	if ( ModeChangeBuilderModeList.Len() == 0 || ModeChange != ModeChangeBuilderModeAction )
		ModeChangeBuilderModeList += ModeChange == EKIRCModeChange::M_Add ? "+" : "-";

	ModeChangeBuilderModeList += Mode->GetMode();

	if ( ModeChangeBuilderParamList.Len() > 0 )
		ModeChangeBuilderParamList += " ";

	ModeChangeBuilderParamList += Param;
	return true;
}


bool UKIRCClient::ChangeChannelUserMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange, UKIRCUser* User )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Tried to change a null mode." );
		return false;
	}

	if ( User == NULL )
	{
		KIRCLog( Error, "Tried to change a channel user mode for a null user." );
		return false;
	}

	return ChangeChannelParamMode( Channel, Mode, ModeChange, User->GetName() );
}


void UKIRCClient::FlushModeChanges( UKIRCChannel* Channel )
{
	if ( Channel != NULL )
	{
		Cmd(
			"MODE %s %s %s",
			*Channel->GetName(),
			*ModeChangeBuilderModeList,
			*ModeChangeBuilderParamList
		);
	}

	bBuildingModeString = false;
	ModeChangeBuilderModeAction = EKIRCModeChange::M_Add;
	ModeChangeBuilderModeList = "";
	ModeChangeBuilderParamList = "";
}


bool UKIRCClient::QueryObjectModes( UKIRCObject* Object )
{
	if ( Object == NULL )
	{
		KIRCLog( Error, "Tried to query the modes of a null object." );
		return false;
	}

	return Cmd( "MODE %s", *Object->GetName() );
}


bool UKIRCClient::QueryChannelModeList( UKIRCChannel* Channel, UKIRCMode* Mode )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to query the mode list of a null channel." );
		return false;
	}

	if ( Mode == NULL )
	{
		KIRCLog( Error, "Tried to query the mode list of a null mode." );
		return false;
	}

	if ( Mode->GetType() != EKIRCModeType::T_Channel_List )
	{
		KIRCLog( Error, "Tried to query the mode list of a non-list node." );
		return false;
	}

	return Cmd( "MODE %s +%s", *Channel->GetName(), *Mode->GetName() );
}


bool UKIRCClient::ChangeNickname( const FString& NewName )
{
	if ( NewName.Len() == 0 )
		return false;

	return Cmd( "NICK %s", *NewName );
}


bool UKIRCClient::QueryTopic( UKIRCChannel* Channel )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to query the topic of a null channel." );
		return false;
	}

	return Cmd( "TOPIC %s", *Channel->GetName() );
}


bool UKIRCClient::SetTopic( UKIRCChannel* Channel, const FString& Body )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to set the topic of a null channel." );
		return false;
	}

	return Cmd( "TOPIC %s :%s", *Channel->GetName(), *Body.Left( UKIRCServer::MaxCommandLength - 8 - Channel->GetName().Len() ) );
}


bool UKIRCClient::GetServerNameList()
{
	return Cmd( "NAMES" );
}


bool UKIRCClient::UpdateChannelNameList( UKIRCChannel* Channel )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to set name list of a null channel." );
		return false;
	}

	return Cmd( "NAMES %s", *Channel->GetName() );
}


bool UKIRCClient::ChannelList( const FString& Mask )
{
	if ( Mask.Len() == 0 )
		return Cmd( "LIST" );

	else
		return Cmd( "LIST %s", *Mask.Left( UKIRCServer::MaxCommandLength - 5 ) );
}


bool UKIRCClient::Who( const FString& Mask )
{
	if ( Mask.Len() == 0 )
		return Cmd( "WHO" );

	else
		return Cmd( "WHO %s", *Mask );
}


bool UKIRCClient::WhoIs( UKIRCUser* User )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to whois a null user." );
		return false;
	}

	return Cmd( "WHOIS %s", *User->GetName() );
}


bool UKIRCClient::WhoWas( const FString& Name )
{
	if ( Name.Len() == 0 )
	{
		KIRCLog( Error, "Trying to whowas a zero-length nickname." );
		return false;
	}

	return Cmd( "WHOWAS %s", *Name );
}


bool UKIRCClient::Away( const FString& Message )
{
	return Cmd( "AWAY :%s", *Message );
}


bool UKIRCClient::Return()
{
	return Cmd( "AWAY" );
}


bool UKIRCClient::Quit( const FString& Message )
{
	bool bSuccess = false;

	if ( Message.Len() == 0 )
		bSuccess = Cmd( "QUIT" );

	else
		bSuccess = Cmd( "QUIT :%s", *Message.Left( UKIRCServer::MaxCommandLength - 6 ) );

	return bSuccess && Server->Disconnect();
}
