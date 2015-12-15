// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/KIRCNumerics.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCChannel.h"
#include "KeshIRC/Model/KIRCMode.h"
#include "KeshIRC/Controller/KIRCBlueprintMessageHandler.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCJoinCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCNickCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCPartCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCTopicCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCListCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCUserModeCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCWhoCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCWhoIsCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCWhoWasCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCKickCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCChannelNameListCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCServerNameListCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCInviteCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCAwayCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCTopicQueryCommandResponseScanner.h"
#include "KeshIRC/Controller/CommandResponseScanners/KIRCIsOnCommandResponseScanner.h"
#include "KeshIRC/Controller/KIRCClient.h"


namespace KeshIRCFramework
{
	FKIRCNumerics Numerics;
	FKIRCInvalidCharacters InvalidCharacters;
	FKIRCModes Modes;
}


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

	const FString CleanNickName = UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().NickName, NickName );

	if ( CleanNickName.Len() < 1 || CleanNickName.Len() > 30 )
	{
		KIRCLog( Error, "Invalid Nickname." );
		return false;
	}

	const FString CleanIdent = UKIRCClient::CleanString( UKIRCClient::GetInvalidCharacters().Ident, Ident );

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

		NickNameList.AddUnique( CleanedAltNickName );
	}

	SetupMessageHandlers();
	return true;
}


bool UKIRCClient::HasModeString( const FString& ModeCharacter ) const
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Tried to get user mode with null server." );
		return false;
	}
	
	const UKIRCMode* const Mode = Server->GetUserMode( ModeCharacter );

	if ( Mode == NULL )
		return false;

	return HasMode( Mode );
}


const FString& UKIRCClient::GetCachedKeyForChannel( const FString& Channel ) const
{
	static FString NoKey = "";

	if ( Channel.Len() < 2 || UKIRCChannel::HasChannelPrefix( Channel ) )
	{
		KIRCLog( Error, "Tried to get a cached key for a channel with an invalid name." );
		return NoKey;
	}

	if ( !ChannelKeyCache.Contains( Channel.ToUpper() ) )
		return NoKey;

	return ChannelKeyCache[ Channel.ToUpper() ];
}


void UKIRCClient::HandleMessage_Implementation( const FString& Line, UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	const FString CommandUpper = Command.ToUpper();

	if ( MessageHandlers.Contains( CommandUpper ) )
		MessageHandlers[ CommandUpper ].Broadcast( Source, Command, Params, Message );

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
				OnCommandResponseDelegate.Broadcast( this, CommandResponseScanners[ 0 ] );
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


FKIRCNumerics UKIRCClient::GetNumericsBP()
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


const FKIRCModes& UKIRCClient::GetModes()
{
	return KeshIRCFramework::Modes;
}


FKIRCModes UKIRCClient::GetModesBP()
{
	return KeshIRCFramework::Modes;
}


void UKIRCClient::AddMessageHandler( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Tried to add a handler for a zero length command." );
		return;
	}

	if ( CallbackObject == NULL )
	{
		KIRCLog( Error, "Tried to add a callback on a null objcet." );
		return;
	}

	if ( CallbackFunction == NULL )
	{
		KIRCLog( Error, "Tried to add a callback with a null function." );
		return;
	}

	const FString CommandUpper = Command.ToUpper();

	if ( !MessageHandlers.Contains( CommandUpper ) )
	{
		FKIRCIncomingMessageHandler MessageHandler;
		MessageHandlers.Emplace( CommandUpper, MessageHandler );
	}

	MessageHandlers[ CommandUpper ].AddUniqueDynamic( CallbackObject, CallbackFunction );
}


void UKIRCClient::AddMessageHandler( int32 Numeric, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Numeric < GetNumerics().NumericMin || Numeric > GetNumerics().NumericMax )
	{
		KIRCLog( Error, "Tried to add an out of range numeric handler." );
		return;
	}

	AddMessageHandler( NumericToString( Numeric ), CallbackObject, CallbackFunction );
}


void UKIRCClient::RemoveMessageHandler( const FString& Command, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Command.Len() == 0 )
	{
		KIRCLog( Error, "Tried to remove a handler for a zero length command." );
		return;
	}

	if ( CallbackObject == NULL )
	{
		KIRCLog( Error, "Tried to remove an delegate with a null object." );
		return;
	}

	if ( CallbackFunction == NULL )
	{
		KIRCLog( Error, "Tried to remove an delegate with a null function." );
		return;
	}

	const FString CommandUpper = Command.ToUpper();

	if ( !MessageHandlers.Contains( CommandUpper ) )
	{
		KIRCLog( Error, "Tried to remove a handler for command that isn't being handled." );
		return;
	}

	MessageHandlers[ CommandUpper ].RemoveDynamic( CallbackObject, CallbackFunction );

	if ( !MessageHandlers[ CommandUpper ].IsBound() )
		MessageHandlers.Remove( CommandUpper );
}


void UKIRCClient::RemoveMessageHandler( int32 Numeric, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction )
{
	if ( Numeric < GetNumerics().NumericMin || Numeric > GetNumerics().NumericMax )
	{
		KIRCLog( Error, "Tried to remove an out of range numeric handler." );
		return;
	}

	RemoveMessageHandler( NumericToString( Numeric ), CallbackObject, CallbackFunction );
}


UKIRCBlueprintMessageHandler* const UKIRCClient::CreateMessageHandler( TSubclassOf<UKIRCBlueprintMessageHandler> MessageHandlerClass,
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

	UKIRCBlueprintMessageHandler* const NewMessageHandler = NewObject<UKIRCBlueprintMessageHandler>( this, MessageHandlerClass );

	if ( NewMessageHandler == NULL )
	{
		KIRCLog( Error, "Failed to create message handler." );
		return NULL;
	}

	if ( bAutoRegister )
		NewMessageHandler->RegisterHandler( this );

	if ( bStoreReference )
		this->BlueprintMessageHandlers.Add( NewMessageHandler );

	return NewMessageHandler;
}


void UKIRCClient::SetupMessageHandlers()
{
	// Nickname negotiation handlers
	AddMessageHandler( GetNumerics().ErrorNoNicknameGiven, this, static_cast< FKIRCIncomingMessageHandlerDelegate >( &UKIRCClient::OnNickNegotiationFatalHandler ) );
	AddMessageHandler( GetNumerics().ErrorNickNameInUse, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	AddMessageHandler( GetNumerics().ErrorUnAvailResource, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	AddMessageHandler( GetNumerics().ErrorErroneusNickname, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	AddMessageHandler( GetNumerics().ErrorNickCollision, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	AddMessageHandler( GetNumerics().ErrorRestricted, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );

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

	for ( TSubclassOf<UKIRCBlueprintMessageHandler> BlueprintMessagerHandlerClass : MessageHandlerClasses )
		CreateMessageHandler( BlueprintMessagerHandlerClass, true, true );
}


void UKIRCClient::OnNickNegotiationFatalHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	KIRCLog( Error, "Critical error negotiating nickname." );

	if ( Server == NULL )
	{
		KIRCLog( Error, "Unable to disconnect from null server." );
		return;
	}

	Server->Disconnect();
}


void UKIRCClient::OnNickNegotaitionNextNickHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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


void UKIRCClient::OnNetworkWelcomeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received network welcome with null server." );
		return;
	}

	// Remove nick negotiation handlers
	RemoveMessageHandler( GetNumerics().ErrorNoNicknameGiven, this, static_cast< FKIRCIncomingMessageHandlerDelegate >( &UKIRCClient::OnNickNegotiationFatalHandler ) );
	RemoveMessageHandler( GetNumerics().ErrorNickNameInUse, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RemoveMessageHandler( GetNumerics().ErrorUnAvailResource, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RemoveMessageHandler( GetNumerics().ErrorErroneusNickname, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RemoveMessageHandler( GetNumerics().ErrorNickCollision, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	RemoveMessageHandler( GetNumerics().ErrorRestricted, this, static_cast<FKIRCIncomingMessageHandlerDelegate>( &UKIRCClient::OnNickNegotaitionNextNickHandler ) );
	
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


void UKIRCClient::OnNetworkInfoHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Network info received with a null server." );
		return;
	}

	if ( Params.Num() < 6 )
	{
		KIRCLog( Error, "Not enough params in network info." );
		return;
	}

	Server->SetHostActual( Params[ 1 ] );
	Server->SetVersion( Params[ 2 ] );	

	for ( int32 i = 0; i < Params[ 3 ].Len(); ++i )
		Server->AddUserMode( FString( 1, &Params[ 3 ][ i ] ) );

	for ( int32 i = 0; i < Params[ 4 ].Len(); ++i )
	{
		const TCHAR& ModeChar = Params[ 4 ][ i ];
		bool bIsParamMode = false;

		for ( int32 j = 0; j < Params[ 5 ].Len(); ++j )
		{
			const TCHAR& ParamChar = Params[ 5 ][ j ];

			if ( ModeChar != ParamChar )
				continue;

			bIsParamMode = true;
			break;
		}

		if ( bIsParamMode )
		{
			// Hard core the 2 non-list param chars (key, limit.)
			// TODO: Some way to define custom "uses param when removing" for different modes...
			if ( ModeChar == 'l' )
			{
				Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_Param, EKIRCModeParamRequired::R_AddingOnly );
				continue;
			}

			else if ( ModeChar == 'k' )
			{
				Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_Param, EKIRCModeParamRequired::R_Always );
				continue;
			}

			// Hardcore the 3 user modes (ops, halfops/helper, voice.)
			// TODO: Define some way to have additional channel user modes
			if ( ModeChar == 'o' || ModeChar == 'h' || ModeChar == 'v' )
			{
				// All channel user modes require the user parameter when removing
				Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_User, EKIRCModeParamRequired::R_Always );
				continue;
			}

			// Otherwise we're a list mode (ban, etc.)
			Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_List, EKIRCModeParamRequired::R_Always );
			continue;
		}

		Server->AddChannelMode( FString( 1, &ModeChar ), EKIRCModeType::T_Channel_Unary, EKIRCModeParamRequired::R_Never );
	}
}


void UKIRCClient::OnServerSettingHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Network settings received with a null server." );
		return;
	}

	for ( const FString& Param : Params )
	{
		int32 iEquals = INDEX_NONE;
		Param.FindChar( '=', iEquals );

		if ( iEquals == INDEX_NONE )
			Server->SetSetting( Param, "" );

		else
			Server->SetSetting( Param.Left( iEquals ), Param.Mid( iEquals + 1 ) );
	}
}


void UKIRCClient::OnMOTDStartHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	MOTD.SetNum( 0 );
}


void UKIRCClient::OnMOTDLineHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	MOTD.Add( Message );
}


void UKIRCClient::OnMOTDEndHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	OnMOTDCompleteDelegate.Broadcast( this, MOTD );
	OnMOTDCompleteEvent( MOTD );

	if ( !bRegistered )
		OnRegister();
}


void UKIRCClient::OnNoMOTDHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	MOTD.SetNum( 0 );
	OnMOTDCompleteDelegate.Broadcast( this, MOTD );
	OnMOTDCompleteEvent( MOTD );

	if ( !bRegistered )
		OnRegister();
}


void UKIRCClient::OnMessageHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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


void UKIRCClient::OnNickChangeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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

	const FString OldNick = Source->GetName();

	Source->SetName( Message );
	OnNickNameChangedDelegate.Broadcast( Source, OldNick );
	OnNickNameChangedEvent( Source, OldNick );
	Source->OnNickNameChangedDelegate.Broadcast( Source, OldNick );
}


void UKIRCClient::OnJoinHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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


void UKIRCClient::OnPartHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 0 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "User trying to join a null channel." );
		return;
	}

	Channel->UserLeft( Source );

	OnPartDelegate.Broadcast( Channel, Source, Message );
	OnPartEvent( Channel, Source, Message );
	Channel->OnPartedDelegate.Broadcast( Channel, Source, Message );

	if ( Source == User )
	{
		if ( Server == NULL )
		{
			KIRCLog( Error, "Trying to rename a channel on a null server." );
		}

		else
			Server->RemoveChannel( Channel );
	}
}


void UKIRCClient::OnModeChangeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Trying to change mode on a null server." );
		return;
	}

	if ( Params.Num() == 0 )
	{
		KIRCLog( Error, "Mode change received with no target." );
		return;
	}

	TArray<FString> ModeParams;

	if ( Message.Len() > 0 )
	{
		FString ModeParamList = Message;
		ModeParamList = ModeParamList.Trim().TrimTrailing();
		ModeParamList.ParseIntoArray( ModeParams, TEXT( " " ), true );
	}

	else
	{
		ModeParams = Params;
		ModeParams.RemoveAt( 0 );
	}

	if ( ModeParams.Num() == 0 )
	{
		KIRCLog( Error, "Mode change received with no modes." );
		return;
	}

	if ( UKIRCChannel::HasChannelPrefix( Params[ 0 ] ) )
	{
		UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 0 ] );

		if ( Channel == NULL )
		{
			KIRCLog( Error, "Mode change on a null channel." );
			return;
		}

		const FString ModeString = ModeParams[ 0 ];
		int32 iCurrentParam = 1;
		EKIRCModeChange ModeChange = EKIRCModeChange::M_Add;
		TCHAR ModeChar;
		const UKIRCMode* Mode;

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

				UKIRCUser* const Target = Server->EnsureUser( ModeParams[ iCurrentParam ] );
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

				OnChannelUserModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, Target );
				OnChannelUserModeEvent( Channel, Source, Mode, ModeChange, Target );
				Channel->OnChannelUserModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, Target );
				Target->OnChannelUserModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, Target );
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

				OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
				OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
				Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
				++iCurrentParam;
			}

			else if ( Mode->GetType() == EKIRCModeType::T_Channel_Param )
			{
				if ( Mode->GetMode() == UKIRCClient::GetModes().Channel.Key )
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
						ChannelKeyCache.Emplace( Channel->GetName().ToUpper(), ModeParams[ iCurrentParam ] );
					}

					else
					{
						Channel->RemoveUnaryMode( Mode );
						Channel->SetJoinKey( "" );
						ChannelKeyCache.Remove( Channel->GetName().ToUpper() );
					}

					OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
					OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
					Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
					++iCurrentParam;
				}

				else if ( Mode->GetMode() == UKIRCClient::GetModes().Channel.UserLimit )
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

						const int Limit = FCString::Atoi( *ModeParams[ iCurrentParam ] );

						Channel->AddUnaryMode( Mode );
						Channel->SetLimit( Limit );
						OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
						++iCurrentParam;
					}

					else
					{
						Channel->RemoveUnaryMode( Mode );
						Channel->SetLimit( 0 );
						OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, "0" );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, "0" );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, "0" );
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
						OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
						OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
						Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
						++iCurrentParam;
					}

					else
					{
						if ( Mode->GetParamRequired() == EKIRCModeParamRequired::R_Always )
						{
							if ( iCurrentParam == ModeParams.Num() )
							{
								KIRCLog( Error, "Not enough params in mode string." );
								return;
							}

							OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
							OnChannelModeEvent( Channel, Source, Mode, ModeChange, ModeParams[ iCurrentParam ] );
							Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, ModeParams[ iCurrentParam ] );
							++iCurrentParam;
						}

						else
						{
							OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, "" );
							OnChannelModeEvent( Channel, Source, Mode, ModeChange, "" );
							Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, "" );
						}
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
				
				OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, "" );
				OnChannelModeEvent( Channel, Source, Mode, ModeChange, "" );
				Channel->OnChannelModeDelegate.Broadcast( Channel, Source, const_cast<UKIRCMode*>( Mode ), ModeChange, "" );
			}
		}
	}

	else
	{
		UKIRCUser* const User = Server->GetUserByName( Params[ 0 ] );

		if ( User == NULL )
		{
			KIRCLog( Error, "Unable to ensure user object." );
			return;
		}

		// Don't care about others' modes.
		if ( User != this->User )
			return;

		const FString ModeString = ModeParams[ 0 ];
		int32 iCurrentParam = 2;
		EKIRCModeChange ModeChange = EKIRCModeChange::M_Add;
		TCHAR ModeChar;
		const UKIRCMode* Mode;

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

			OnUserModeDelegate.Broadcast( User, const_cast<UKIRCMode*>( Mode ), ModeChange );
			OnUserModeEvent( User, Mode, ModeChange );
			User->OnUserModeDelegate.Broadcast( User, const_cast<UKIRCMode*>( Mode ), ModeChange );
		}
	}
}


void UKIRCClient::OnTopicChangeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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


void UKIRCClient::OnKickHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Params.Num() < 2 )
	{
		KIRCLog( Error, "Received kick with insufficient params." );
		return;
	}

	UKIRCChannel* const Channel = Server->EnsureChannel( Params[ 0 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Unable to ensure channel object." );
		return;
	}

	UKIRCUser* const Target = Server->EnsureUser( Params[ 1 ] );

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


void UKIRCClient::OnInviteHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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

	UKIRCUser* const User = Server->GetUserByName( Params[ 0 ] );

	if ( User != NULL )
		User->OnInvitedDelegate.Broadcast( Source, Params[ 1 ] );

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel != NULL )
		Channel->OnInvitedDelegate.Broadcast( Source, Params[ 1 ] );
}


void UKIRCClient::OnQuitHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Source == NULL )
	{
		KIRCLog( Error, "Null user trying to quit (server?)." );
		return;
	}

	OnQuitDelegate.Broadcast( Source, Message );
	OnQuitEvent( Source, Message );

	if ( Source != NULL )
		Source->OnQuitDelegate.Broadcast( Source, Message );

	TArray<UKIRCChannel*> Channels = Source->GetChannels();

	for ( int32 i = Channels.Num() - 1; i >= 0; --i )
		Channels[ i ]->UserLeft( Source );

	if ( Source == User )
		Server->Disconnect();
}


void UKIRCClient::OnTopicBodyHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Topic body received, but cannot find channel." );
		return;
	}

	Channel->SetTopicBody( Message );
	OnTopicReceiveDelegate.Broadcast( Channel, Message );
	Channel->OnTopicReceiveDelegate.Broadcast( Channel, Message );
}


void UKIRCClient::OnTopicDetailsHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 1 ] );

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

	const FDateTime TopicDate = FDateTime::FromUnixTimestamp( FCString::Atoi( *Params[ 3 ] ) );

	Channel->SetTopicAuthor( AuthorName );
	Channel->SetTopicDate( TopicDate );
	
	OnTopicDetailsDelegate.Broadcast( Channel, AuthorName, TopicDate, AuthorIdent, AuthorHost, Params[ 2 ] );
	Channel->OnTopicDetailsDelegate.Broadcast( Channel, AuthorName, TopicDate, AuthorIdent, AuthorHost, Params[ 2 ] );
}


void UKIRCClient::OnNameListHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
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

	// Names reply with no channel - users without +i set... I think.
	if ( Params[ 2 ] == "*" )
		return;

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 2 ] );

	if ( Channel == NULL )
	{
		// This could be a server names scan - and we may not be in the channel.
		//KIRCLog( Error, "Channel names list received, but cannot find channel." );
		return;
	}

	const UKIRCMode* const Private = Server->GetChannelMode( UKIRCClient::GetModes().Channel.Private );
	const UKIRCMode* const Secret = Server->GetChannelMode( UKIRCClient::GetModes().Channel.Secret );

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

	const UKIRCMode* const Ops = Server->GetChannelMode( UKIRCClient::GetModes().Channel.Ops );
	const UKIRCMode* const HalfOps = Server->GetChannelMode( UKIRCClient::GetModes().Channel.HalfOps );
	const UKIRCMode* const Voice = Server->GetChannelMode( UKIRCClient::GetModes().Channel.Voice );

	TArray<FString> Names;
	Message.ParseIntoArray( Names, TEXT( " " ) );

	for ( FString& Name : Names )
	{
		int32 iNameStartIndex;
		TArray<const UKIRCMode*> Modes;

		for ( iNameStartIndex = 0; iNameStartIndex < Name.Len(); ++iNameStartIndex )
		{
			if ( Name[ iNameStartIndex ] == UKIRCClient::GetModes().Channel.OpsPrefix[ 0 ] )
			{
				if ( Ops != NULL )
					Modes.AddUnique( Ops );

				continue;
			}

			if ( Name[ iNameStartIndex ] == UKIRCClient::GetModes().Channel.HalfOpsPrefix[ 0 ] )
			{
				if ( HalfOps != NULL )
					Modes.AddUnique( HalfOps );

				continue;
			}

			if ( Name[ iNameStartIndex ] == UKIRCClient::GetModes().Channel.VoicePrefix[ 0 ] )
			{
				if ( Voice != NULL )
					Modes.AddUnique( Voice );

				continue;
			}

			break;
		}

		UKIRCUser* const User = Server->EnsureUser( Name.Mid( iNameStartIndex ) );

		if ( User == NULL )
		{
			KIRCLog( Error, "Unable to ensure user's existence." );
			return;
		}

		if ( !User->IsInChannel( Channel ) )
			Channel->UserJoined( User, User != this->User ); // We don't know when other users joined the channel

		for ( const UKIRCMode* const Mode : Modes )
			Channel->AddUserMode( User, Mode );
	}
}


void UKIRCClient::OnChannelModesHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received channel modes with null server." );
	}

	if ( Params.Num() < 3 )
	{
		KIRCLog( Error, "Received channel modes with no channel." );
		return;
	}

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Channel modes received, but cannot find channel." );
		return;
	}

	const FString ModeString = Params[ 2 ];
	TCHAR ModeChar;
	const UKIRCMode* Mode;

	for ( int32 i = 0; i < ModeString.Len(); ++i )
	{
		ModeChar = ModeString[ i ];

		if ( ModeChar == '+' )
			continue;

		if ( ModeChar == '-' )
		{
			KIRCLog( Warning, "Received negative mode in channel modes." );
			continue;
		}

		Mode = Server->GetChannelMode( FString( 1, &ModeChar ) );

		if ( Mode == NULL )
		{
			KIRCLog( Error, "Unknown mode in channel modes." );
			return;
		}

		switch ( Mode->GetType() )
		{
			case EKIRCModeType::T_Channel_User:
			case EKIRCModeType::T_Channel_List:
				KIRCLog( Error, "Received user or list channel mode in channel modes." );
				return;

			case EKIRCModeType::T_User:
				KIRCLog( Error, "Received user mode in channel modes." );
				return;

			case EKIRCModeType::T_Channel_Unary:
			case EKIRCModeType::T_Channel_Param:
				Channel->AddUnaryMode( Mode );
				break;
		}
	}
}


void UKIRCClient::OnChannelCreatedHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message )
{
	if ( Server == NULL )
	{
		KIRCLog( Error, "Received channel creation date with null server." );
	}

	if ( Params.Num() < 3 )
	{
		KIRCLog( Error, "Received channel creation date with no channel." );
		return;
	}

	UKIRCChannel* const Channel = Server->GetChannelByName( Params[ 1 ] );

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Channel creation date received, but cannot find channel." );
		return;
	}

	if ( !Params[ 2 ].IsNumeric() )
	{
		KIRCLog( Error, "Channel creation date is not a number." );
		return;
	}

	Channel->SetCreated( FDateTime::FromUnixTimestamp( FCString::Atoi( *Params[ 2 ] ) ) );
}


bool UKIRCClient::SendCommand( const FString& Command, UKIRCCommandResponseScanner* ResponseScanner )
{
	if ( ResponseScanner != NULL )
	{
		int32 iSpaceIndex = INDEX_NONE;
		Command.FindChar( ' ', iSpaceIndex );

		if ( iSpaceIndex != INDEX_NONE )
			ResponseScanner->SetCommand( Command.Left( iSpaceIndex ) );

		CommandScannerQueueLock.Lock();
		CommandResponseScanners.Add( ResponseScanner );

		if ( CommandResponseScanners.Num() == 1 )
			ResponseScanner->StartScan( this );

		CommandScannerQueueLock.Unlock();
	}

	return SendToServer( Command.Left( UKIRCServer::MaxCommandLength ) );
}


bool UKIRCClient::SendCommandCallback( const FString& Command, TSubclassOf<UKIRCCommandResponseScanner> ScannerClass,
									   UObject* CallbackObject, FKIRCCommandResponseCallbackDelegate CallbackFunction, 
									   const FString& Target )
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

		ResponseScanner->SetTarget( Target );

		if ( CallbackObject != NULL && CallbackFunction != NULL )
			ResponseScanner->OnScanComplete.BindDynamic( CallbackObject, CallbackFunction );
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


bool UKIRCClient::ChangeUserMode( const UKIRCMode* const Mode, EKIRCModeChange Change )
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

	return Cmd( "MODE %s %s%s", *User->GetName(), *FString( Change == EKIRCModeChange::M_Add ? "+" : "-" ), *Mode->GetMode() );
}


bool UKIRCClient::JoinChannel( const FString& Channel, const FString& Key )
{
	if ( Channel.Len() < 2 || !UKIRCChannel::HasChannelPrefix( Channel ) )
	{
		KIRCLog( Error, "Tried to join a channel with an invalid name." );
		return false;
	}

	if ( IsInChannel( Channel ) )
	{
		KIRCLog( Error, "Tried to join a channel we are already in." );
		return false;
	}

	FString ActualKey = Key;

	if ( ActualKey == "" && ChannelKeyCache.Contains( Channel.ToUpper() ) )
		ActualKey = ChannelKeyCache[ Channel.ToUpper() ];

	if ( ActualKey.Len() == 0 )
		return CmdScanTgt( UKIRCJoinCommandResponseScanner::StaticClass(), Channel, "JOIN %s", *Channel );

	else
	{
		return CmdScanTgt( UKIRCJoinCommandResponseScanner::StaticClass(), Channel, "JOIN %s %s", *Channel, *ActualKey );
		ChannelKeyCache.Emplace( Channel.ToUpper(), ActualKey );
	}
}


bool UKIRCClient::PartChannel( const UKIRCChannel* const Channel, const FString& Message )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to part a null channel." );
		return false;
	}

	if ( !IsInChannel( Channel->GetName() ) )
	{
		KIRCLog( Error, "Tried to leave a channel we aren't in." );
		return false;
	}

	if ( Message.Len() == 0 )
		return CmdScanTgt( UKIRCPartCommandResponseScanner::StaticClass(), Channel->GetName(), "PART %s", *Channel->GetName() );

	else
		return CmdScanTgt( UKIRCPartCommandResponseScanner::StaticClass(), Channel->GetName(), "PART %s :%s", *Channel->GetName(), 
						   *Message.Left( UKIRCServer::MaxCommandLength - 7 - Channel->GetName().Len() ) );
}


bool UKIRCClient::InviteUserToChannel( const FString& NickName, const UKIRCChannel* const Channel )
{
	if ( NickName.Len() == 0 )
	{
		KIRCLog( Error, "Tried to invite a zero-length named user." );
		return false;
	}

	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to invite to a null channel." );
		return false;
	}

	return CmdScanTgt( UKIRCInviteCommandResponseScanner::StaticClass(), NickName + "," + Channel->GetName(), "INVITE %s %s", *NickName, *Channel->GetName() );
}


bool UKIRCClient::KickUserFromChannel( const UKIRCChannel* const Channel, const UKIRCUser* const User, const FString& Message )
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
		return CmdScanTgt( UKIRCKickCommandResponseScanner::StaticClass(), User->GetName() + "," + Channel->GetName(),
						   "KICK %s %s", *Channel->GetName(), *User->GetName() );

	else
		return CmdScanTgt( UKIRCKickCommandResponseScanner::StaticClass(), User->GetName() + "," + Channel->GetName(),
						   "KICK %s %s :%s", *Channel->GetName(), *User->GetName(), 
						   *Message.Left( UKIRCServer::MaxCommandLength - 8 - Channel->GetName().Len() - User->GetName().Len() ) );
}


void UKIRCClient::StartModeChangeBuilder()
{
	bBuildingModeString = true;
	ModeChangeBuilderModeAction = EKIRCModeChange::M_Add;
	ModeChangeBuilderModeList = "";
	ModeChangeBuilderParamList = "";
}


bool UKIRCClient::ChangeChannelMode( const UKIRCChannel* const Channel, const UKIRCMode* const Mode, EKIRCModeChange ModeChange )
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
			*FString( ModeChange == EKIRCModeChange::M_Add ? "+" : "-" ),
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
		ModeChangeBuilderModeList += ( ModeChange == EKIRCModeChange::M_Add ? "+" : "-" );

	ModeChangeBuilderModeList += Mode->GetMode();
	return true;
}


bool UKIRCClient::ChangeChannelParamMode( const UKIRCChannel* const Channel, const UKIRCMode* const Mode, EKIRCModeChange ModeChange, const FString& Param )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Tried to change a null mode." );
		return false;
	}

	if ( Mode->RequireParamForChange( ModeChange ) && Param.Len() == 0 )
	{
		KIRCLog( Error, "Tried to change mode without required parameter." );
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
			*FString( ModeChange == EKIRCModeChange::M_Add ? "+" : "-" ),
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
		ModeChangeBuilderModeList += ( ModeChange == EKIRCModeChange::M_Add ? "+" : "-" );

	ModeChangeBuilderModeList += Mode->GetMode();

	if ( Mode->RequireParamForChange( ModeChange ) )
	{
		if ( ModeChangeBuilderParamList.Len() > 0 )
			ModeChangeBuilderParamList += " ";

		ModeChangeBuilderParamList += Param;
	}

	return true;
}


bool UKIRCClient::ChangeChannelUserMode( const UKIRCChannel* const Channel, const UKIRCMode* const Mode, EKIRCModeChange ModeChange, const UKIRCUser* const User )
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


void UKIRCClient::FlushModeChanges( const UKIRCChannel* const Channel )
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


bool UKIRCClient::QueryObjectModes( const UKIRCObject* const Object )
{
	if ( Object == NULL )
	{
		KIRCLog( Error, "Tried to query the modes of a null object." );
		return false;
	}

	return Cmd( "MODE %s", *Object->GetName() );
}


bool UKIRCClient::QueryChannelModeList( const UKIRCChannel* const Channel, const UKIRCMode* const Mode )
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

	return Cmd( "MODE %s %s", *Channel->GetName(), *Mode->GetMode() );
}


bool UKIRCClient::ChangeNickname( const FString& NewName )
{
	if ( NewName.Len() == 0 )
		return false;

	return CmdScanTgt( UKIRCNickCommandResponseScanner::StaticClass(), NewName, "NICK %s", *NewName );
}


bool UKIRCClient::QueryTopic( const FString& Channel )
{
	if ( Channel.Len() == 0 )
	{
		KIRCLog( Error, "Tried to query the topic of a zero-length channel." );
		return false;
	}

	return CmdScanTgt( UKIRCTopicQueryCommandResponseScanner::StaticClass(), Channel, "TOPIC %s", *Channel );
}


bool UKIRCClient::SetTopic( const UKIRCChannel* const Channel, const FString& Body )
{
	if ( Channel == NULL )
	{
		KIRCLog( Error, "Tried to set the topic of a null channel." );
		return false;
	}

	return CmdScanTgt( UKIRCTopicCommandResponseScanner::StaticClass(), Channel->GetName(), 
					   "TOPIC %s :%s", *Channel->GetName(), 
					   *Body.Left( UKIRCServer::MaxCommandLength - 8 - Channel->GetName().Len() ) );
}


bool UKIRCClient::GetServerNameList()
{
	return CmdScan( UKIRCServerNameListCommandResponseScanner::StaticClass(), "NAMES" );
}


bool UKIRCClient::GetChannelNameList( const FString& Channel )
{
	if ( Channel.Len() )
	{
		KIRCLog( Error, "Tried to set name list of a zero-length channel." );
		return false;
	}

	return CmdScanTgt( UKIRCChannelNameListCommandResponseScanner::StaticClass(), Channel, "NAMES %s", *Channel );
}


bool UKIRCClient::ListChannels( const FString& Mask )
{
	if ( Mask.Len() == 0 )
		return CmdScan( UKIRCListCommandResponseScanner::StaticClass(), "LIST" );

	else
		return CmdScan( UKIRCListCommandResponseScanner::StaticClass(), "LIST %s", *Mask.Left( UKIRCServer::MaxCommandLength - 5 ) );
}


bool UKIRCClient::Who( const FString& Mask )
{
	if ( Mask.Len() == 0 )
		return CmdScan( UKIRCWhoCommandResponseScanner::StaticClass(), "WHO" );

	else
		return CmdScan( UKIRCWhoCommandResponseScanner::StaticClass(), "WHO %s", *Mask );
}


bool UKIRCClient::WhoIs( const FString& Name )
{
	if ( Name.Len() == 0 )
	{
		KIRCLog( Error, "Trying to whois a zero-length nick name." );
		return false;
	}

	return CmdScanTgt( UKIRCWhoIsCommandResponseScanner::StaticClass(), Name, "WHOIS %s", *Name );
}


bool UKIRCClient::WhoWas( const FString& Name )
{
	if ( Name.Len() == 0 )
	{
		KIRCLog( Error, "Trying to whowas a zero-length nickname." );
		return false;
	}

	return CmdScanTgt( UKIRCWhoWasCommandResponseScanner::StaticClass(), Name, "WHOWAS %s", *Name );
}


bool UKIRCClient::Away( const FString& Message )
{
	// Force use of away message.
	return CmdScan( UKIRCAwayCommandResponseScanner::StaticClass(), "AWAY :%s", *( Message.Len() > 0 ? Message : "Away" ) );
}


bool UKIRCClient::Return()
{
	return CmdScan( UKIRCAwayCommandResponseScanner::StaticClass(), "AWAY" );
}


bool UKIRCClient::AreUsersOnline( const TArray<FString>& NickNames )
{
	if ( NickNames.Num() == 0 )
	{
		KIRCLog( Error, "Trying to check if no users are online." );
		return false;
	}

	FString NickList = "";

	for ( const FString& Nick : NickNames )
		NickList += ( NickList.Len() > 0 ? " " : "" ) + Nick;
	
	return CmdScan( UKIRCIsOnCommandResponseScanner::StaticClass(), "ISON %s", *NickList );
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
