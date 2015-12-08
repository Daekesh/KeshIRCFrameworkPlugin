// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/KIRCNumerics.h"
#include "KeshIRC/KIRCModes.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KIRCClient.generated.h"

#define Cmd( Format, ... ) SendCommand( FString::Printf( TEXT( Format ), __VA_ARGS__ ) )
#define CmdScan( ScannerClass, Format, ... ) SendCommandCallback( FString::Printf( TEXT( Format ), __VA_ARGS__ ), ScannerClass )
#define CmdScanCB( ScannerClass, Object, Function, Format, ... ) SendCommandCallback( FString::Printf( TEXT( Format ), __VA_ARGS__ ), ScannerClass, ScannerClass, Object )
#define CmdScanTgt( ScannerClass, Target, Format, ... ) SendCommandCallback( FString::Printf( TEXT( Format ), __VA_ARGS__ ), ScannerClass, NULL, NULL, Target )
#define CmdScanCbTgt( ScannerClass, Object, Function, Target, Format, ... ) SendCommandCallback( FString::Printf( TEXT( Format ), __VA_ARGS__ ), ScannerClass, ScannerClass, Object, Target )

/**
* Representation of an IRC server connection and the user using it.
*/
UCLASS( Category = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCClient : public UObject
{
	GENERATED_BODY()

public:

	UKIRCClient( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	virtual bool InitClient( const FString& ServerName, const FString& Host, int32 Port, const FString& Password, 
							 const FString& NickName, const FString& Ident, const FString& RealName,
							 const TArray<FString>& AlternateNickNames );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	UKIRCServer* const GetServer() const { return Server; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	UKIRCUser* const GetUser() const { return User; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasRegistered() const { return bRegistered; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	TArray<FString> GetMessageOfTheDayBP() const { return MOTD; }
	const TArray<FString>& GetMessageOfTheDay() const { return MOTD; }

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

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	const FString& GetCachedKeyForChannel( const FString& Channel ) const;

	/**********
	 * Events *
	 **********/

	FKIRCServerConnected OnConnectedDelegate;
	FKIRCServerDisconnected OnDisconnectedDelegate;
	FKIRCConnectionError OnConnectionErrorDelegate;
	FKIRCMOTDComplete OnMOTDCompleteDelegate;
	FKIRCUserRegistered OnRegisteredDelegate;
	FKIRCUserModeChange OnUserModeDelegate;
	FKIRCChannelInvite OnInvitedDelegate;
	FKIRCUserMessage OnMessageDelegate;
	FKIRCChannelJoin OnJoinDelegate;
	FKIRCChannelPart OnPartDelegate;
	FKIRCUserQuit OnQuitDelegate;
	FKIRCChannelKick OnKickDelegate;
	FKIRCChannelModeChange OnChannelModeDelegate;
	FKIRCChannelUserModeChange OnChannelUserModeDelegate;
	FKIRCChannelTopicChange OnTopicChangeDelegate;
	FKIRCChannelBodyReceive OnTopicReceiveDelegate;
	FKIRCChannelDetailsReceive OnTopicDetailsDelegate;
	FKIRCUserNickNameChange OnNickNameChangedDelegate;
	FKIRCUnhandledNumeric OnUnhandledNumericDelegate;
	FKIRCServerRaw OnUnhandledRawMessageDelegate;
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

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable, BlueprintNativeEvent )
	void HandleMessage( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	virtual void HandleMessage_Implementation( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	virtual FDelegateHandle AddMessageHandler( const FString& Command, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual FDelegateHandle AddMessageHandler( int32 Numeric, UObject* const CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveMessageHandler( const FString& Command, FDelegateHandle Handle );

	virtual void RemoveMessageHandler( int32 Numeric, FDelegateHandle Handle );

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
	void OnInvitedEvent( UKIRCUser* Source, const FString& InvitedTo );

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
