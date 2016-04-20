// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/KIRCNumerics.h"
#include "KeshIRC/KIRCModes.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KIRCClient.generated.h"

#define KIRCCmd( Cmd ) SendCommand( Cmd )
#define KIRCCmdArgs( Format, ... ) KIRCCmd( FString::Printf( TEXT( Format ), __VA_ARGS__ ) )

#define KIRCCmdScan( ScannerClass, Cmd ) SendCommandCallback( Cmd, ScannerClass )
#define KIRCCmdScanArgs( ScannerClass, Format, ... ) KIRCCmdScan( ScannerClass, FString::Printf( TEXT( Format ), __VA_ARGS__ ) )

#define KIRCCmdScanCB( ScannerClass, Object, Function, Cmd ) SendCommandCallback( Cmd, ScannerClass, ScannerClass, Object )
#define KIRCCmdScanCBArgs( ScannerClass, Object, Function, Format, ... ) KIRCCmdScanCB( ScannerClass, Object, Function, FString::Printf( TEXT( Format ), __VA_ARGS__ ) )

#define KIRCCmdScanTgt( ScannerClass, Target, Cmd ) SendCommandCallback( Cmd, ScannerClass, NULL, NULL, Target )
#define KIRCCmdScanTgtArgs( ScannerClass, Target, Format, ... ) KIRCCmdScanTgt( ScannerClass, Target, FString::Printf( TEXT( Format ), __VA_ARGS__ ) )

#define KIRCCmdScanCbTgt( ScannerClass, Object, Function, Target, Cmd ) SendCommandCallback( Cmd, ScannerClass, ScannerClass, Object, Target )
#define KIRCCmdScanCbTgtArgs( ScannerClass, Object, Function, Target, Format, ... ) KIRCCmdScanCbTgt( ScannerClass, Object, Function, Target, FString::Printf( TEXT( Format ), __VA_ARGS__ ) )

/**
* Representation of an IRC server connection and the user using it.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCClient : public UObject
{
	GENERATED_BODY()

public:

	UKIRCClient( const class FObjectInitializer& ObjectInitializer );

	/* Initialises client values and creates a server. 
	 * Does not connect to the server.
	 */
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	virtual bool InitClient( const FString& ServerName, const FString& Host, int32 Port, const FString& Password, 
							 const FString& NickName, const FString& Ident, const FString& RealName,
							 const TArray<FString>& AlternateNickNames );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	UKIRCServer* const GetServer() const { return Server; }

	// Convenience method to connect the server.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	void Connect() { if ( Server != NULL ) { Server->Connect(); } }

	// Convenience method to disconnect the server.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	void Disconnect() { if ( Server != NULL ) { Server->Disconnect(); } }

	// Returns the server user which represents this client on the network.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	UKIRCUser* const GetUser() const { return User; }

	// If the client is not registered, non-registration commands cannot be used.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasRegistered() const { return bRegistered; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	TArray<FString> GetMessageOfTheDayBP() const { return MOTD; }
	const TArray<FString>& GetMessageOfTheDay() const { return MOTD; }

	// Returns the list of nicknames this client attempts to use when connecting to a server.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	TArray<FString> GetNickNameListBP() const { return NickNameList; }
	const TArray<FString>& GetNickNameList() const { return NickNameList; }

	// Returns the nodes currently set on the server user.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	const TArray<UKIRCMode*> GetUserModesBP() const
	{
		TArray<UKIRCMode*> UserModeArray;

		for ( const UKIRCMode* Mode : UserModes )
			UserModeArray.Add( const_cast< UKIRCMode* >( Mode ) );

		return UserModeArray;
	}
	const TArray<const UKIRCMode*>& GetUserModes() const { return UserModes; }

	// Returns true if the server user has the given mode set.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasMode( const UKIRCMode* const Mode ) const { return Mode == NULL ? false : UserModes.Contains( Mode ); }

	// Returns true if the server user has the given mode set.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasModeString( const FString& ModeCharacter ) const;

	// Returns true if this user is in at least 1 channel with the client.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool CanSeeUser( const FString& Name ) const 
	{ 
		if ( Server == NULL )
			return false;

		return ( Server->GetUserByName( Name ) != NULL );
	}

	// Channel must start with a valid channel prefix.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool IsInChannel( const FString& Name ) const
	{ 
		if ( Server == NULL )
			return false;

		return ( Server->GetChannelByName( Name ) != NULL );
	}

	// When joining a channel with a key, the key is cached for later use. This returns the cached value.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	const FString& GetCachedKeyForChannel( const FString& Channel ) const;

	/**********
	 * Events *
	 **********/

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCServerConnected OnConnectedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCServerDisconnected OnDisconnectedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCConnectionError OnConnectionErrorDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCMOTDComplete OnMOTDCompleteDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserRegistered OnRegisteredDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserModeChange OnUserModeDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelInvite OnInvitedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserMessage OnMessageDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelJoin OnJoinDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelPart OnPartDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserQuit OnQuitDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelKick OnKickDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelModeChange OnChannelModeDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelUserModeChange OnChannelUserModeDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelTopicChange OnTopicChangeDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelBodyReceive OnTopicReceiveDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelDetailsReceive OnTopicDetailsDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserNickNameChange OnNickNameChangedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUnhandledNumeric OnUnhandledNumericDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCServerRaw OnUnhandledRawMessageDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCCommandResponse OnCommandResponseDelegate;


	/************
	 * Commands *
	 ************/

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Message( const FString& Target, EKIRCMessageType Type, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeUserMode( const UKIRCMode* const Mode, EKIRCModeChange Change );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool JoinChannel( const FString& Channel, const FString& Key = "" );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool PartChannel( const UKIRCChannel* const Channel, const FString& Message = "" );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool InviteUserToChannel( const FString& NickName, const UKIRCChannel* const Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool KickUserFromChannel( const UKIRCChannel* const Channel, const UKIRCUser* const User, const FString& Message = "" );

	// Call this to build up mode changes before flushing them.
	// All mode changes must be for the same channel.
	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual void StartModeChangeBuilder();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeChannelMode( const UKIRCChannel* const Channel, const UKIRCMode* const Mode, EKIRCModeChange ModeChange );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeChannelParamMode( const UKIRCChannel* const Channel, const UKIRCMode* const Mode, EKIRCModeChange ModeChange, const FString& Param );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeChannelUserMode( const UKIRCChannel* const Channel, const UKIRCMode* const Mode, EKIRCModeChange ModeChange, const UKIRCUser* const User );

	// Send built mode string.
	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual void FlushModeChanges( const UKIRCChannel* const Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool QueryObjectModes( const UKIRCObject* const Object );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool QueryChannelModeList( const UKIRCChannel* const Channel, const UKIRCMode* const Mode );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeNickname( const FString& NewName );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool QueryTopic( const FString& Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool SetTopic( const UKIRCChannel* const Channel, const FString& TopicBody );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool GetServerNameList();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ListChannels( const FString& Mask = "" );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool GetChannelNameList( const FString& Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Who( const FString& Mask );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool WhoIs( const FString& Name );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool WhoWas( const FString& Name );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Away( const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Return();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool AreUsersOnline( const TArray<FString>& NickNames );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Quit( const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool SendCommand( const FString& Command, UKIRCCommandResponseScanner* ResponseScanner = NULL );

	virtual bool SendCommandCallback( const FString& Command, TSubclassOf<UKIRCCommandResponseScanner> ScannerClass = NULL,
									  UObject* CallbackObject = NULL, FKIRCCommandResponseCallbackDelegate CallbackFunction = NULL,
									  const FString& Target = "" );


	/*********************
	 * Message Handling *
	 *********************/

	/* Handles a message direct from the server.
	 *
	 * Line: The full raw line received.
	 * Source: The entity that sent the message.
	 * Command: The first token following the source.
	 * Params: The tokens following the command, preceding the first semi-colon.
	 * Message: The string following the semi-colon.
	 *
	 * Example: source!ident@sourcehost COMMAND Param1 Param2 Param3 :Message here.
	 */
	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable, BlueprintNativeEvent )
	void HandleMessage( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	virtual void HandleMessage_Implementation( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	virtual void AddMessageHandler( const FString& Command, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void AddMessageHandler( int32 Numeric, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveMessageHandler( const FString& Command, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveMessageHandler( int32 Numeric, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	UKIRCBlueprintMessageHandler* const CreateMessageHandler( TSubclassOf<UKIRCBlueprintMessageHandler> MessageHandlerClass,
															   bool bAutoRegister = true, bool bStoreReference = true );

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable, BlueprintPure )
	static FKIRCNumerics GetNumericsBP();
	static const FKIRCNumerics& GetNumerics();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable, BlueprintPure )
	static FString NumericToString( int32 Numeric );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable, BlueprintPure )
	static FKIRCModes GetModesBP();
	static const FKIRCModes& GetModes();


	/*****************
	 * Miscellaneous *
	 *****************/

	// Removes invalid characters from a string of disallowed characters.
	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable, BlueprintPure )
	static FString CleanString( const FString& DisallowedCharactes, const FString& String, bool bAllowUpperOctet = true );

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable, BlueprintPure )
	static const FKIRCInvalidCharacters& GetInvalidCharacters();

protected:

	friend class UKIRCServer;
	friend class UKIRCUserModeCommandResponseScanner;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	TArray<FString> NickNameList;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	bool bRegistered;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	TArray<FString> MOTD;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	bool bBuildingModeString;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	EKIRCModeChange ModeChangeBuilderModeAction;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	FString ModeChangeBuilderModeList;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	FString ModeChangeBuilderParamList;

	TMap<FString, FKIRCIncomingMessageHandler> MessageHandlers;
	FCriticalSection CommandScannerQueueLock;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	UKIRCServer* Server;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	UKIRCUser* User;

	// The modes the client's user has set.
	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	TArray<const UKIRCMode*> UserModes;

	UPROPERTY()
	TMap<FString, FString> ChannelKeyCache;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	TArray<UKIRCCommandResponseScanner*> CommandResponseScanners;

public:

	// Will be automatically instantiated when the client is initialised and added to the BP 
	// Message Handlers var.
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintReadOnly, EditAnywhere )
	TArray<TSubclassOf<UKIRCBlueprintMessageHandler>> MessageHandlerClasses;

protected:

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly, BlueprintReadWrite )
	TArray<UKIRCBlueprintMessageHandler*> BlueprintMessageHandlers;

	virtual void Register();

	virtual bool SendToServer( const FString& Command );

	void OnConnected();
	void OnDisconnected( EKIRCServerDisconnectReason Reason );
	void OnConnectionError( const FString& Error );
	void OnRegister();

	virtual void SetupMessageHandlers();

	// Nick negotiation
	FDelegateHandle RegistrationNickErrorNoneGiven;
	FDelegateHandle RegistrationNickErrorNickInUse;
	FDelegateHandle RegistrationNickErrorUnavailableResource;
	FDelegateHandle RegistrationNickErrorErroneousNick;
	FDelegateHandle RegistrationNickErrorNickCollision;
	FDelegateHandle RegistrationNickErrorRestricted;

	void OnNickNegotiationFatalHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNickNegotaitionNextNickHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// Network info
	void OnNetworkWelcomeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNetworkInfoHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnServerSettingHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// MOTD
	void OnMOTDStartHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnMOTDLineHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnMOTDEndHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNoMOTDHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// Channel join
	void OnTopicBodyHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnTopicDetailsHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNameListHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnChannelModesHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnChannelCreatedHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// Trigger events
	void OnMessageHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNickChangeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnJoinHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnPartHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnModeChangeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnTopicChangeHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnKickHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );	
	void OnInviteHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnQuitHandler( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );


	/*************
	 * BP Events *
	 *************/

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnConnectedEvent( UKIRCServer* ServerConnected );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnDisconnectedEvent( UKIRCServer* ServerDisconnected, EKIRCServerDisconnectReason Reason );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnConnectionErrorEvent( UKIRCServer* ServerErrored, const FString& Error );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnMOTDCompleteEvent( const TArray<FString>& MOTDLines );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnRegisteredEvent();

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnUserModeEvent( UKIRCUser* UserChanged, const UKIRCMode* Mode, EKIRCModeChange ModeChange );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnInvitedEvent( UKIRCUser* Source, const FString& InvitedTo, const FString& UserInvited );

	// Channel is null if this is a private message to the client.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnMessageEvent( UKIRCUser* Source, UKIRCChannel* Channel, EKIRCMessageType MessageType, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnJoinEvent( UKIRCChannel* Channel, UKIRCUser* UserJoining );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnPartEvent( UKIRCChannel* Channel, UKIRCUser* UserParting, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnQuitEvent( UKIRCUser* QuittingUser, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnKickEvent( UKIRCChannel* Channel, UKIRCUser* Source, UKIRCUser* Target, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnChannelModeEvent( UKIRCChannel* Channel, UKIRCUser* Souce, const UKIRCMode* Mode, EKIRCModeChange ModeChange, const FString& Param );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnChannelUserModeEvent( UKIRCChannel* Channel, UKIRCUser* Source, const UKIRCMode* Mode, EKIRCModeChange ModeChange, UKIRCUser* Target );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnTopicChangeEvent( UKIRCChannel* Channel, UKIRCUser* Source, const FString& Topic );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnNickNameChangedEvent( UKIRCUser* UserChangingNick, const FString& OldName );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnUnhandledNumericEvent( UKIRCServer* ServerNotHandling, int32 Numeric, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnUnhandledRawMessageEvent( UKIRCServer* ServerNotHandling, const FString& Message );

};
