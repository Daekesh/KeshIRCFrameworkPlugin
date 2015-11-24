// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/KIRCNumerics.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KIRCClient.generated.h"

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
	UKIRCServer* GetServer() const { return Server; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	UKIRCUser* GetUser() const { return User; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasRegistered() const { return bRegistered; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	const TArray<FString>& GetMessageOfTheDay() const { return MOTD; }

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	const TArray<FString>& GetNickNameList() const { return NickNameList; }

	// Returns the nodes currently set on the server user.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	const TArray<UKIRCMode*>& GetUserModes() const { return UserModes; }

	// Returns true if the server user has the given mode set.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasMode( UKIRCMode* Mode ) const { return Mode == NULL ? false : UserModes.Contains( Mode ); }

	// Returns true if the server user has the given mode set.
	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintCallable )
	bool HasModeString( const FString& Mode ) const;

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


	/************
	 * Commands *
	 ************/

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Message( const FString& Target, EKIRCMessageType Type, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool UserMode( UKIRCMode* Mode, EKIRCModeChange Change );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool JoinChannel( const FString& Channel, const FString& Key = "" );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool PartChannel( UKIRCChannel* Channel, const FString& Message = "" );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool InviteUserToChannel( UKIRCUser* User, UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool KickUserFromChannel( UKIRCChannel* Channel, UKIRCUser* User, const FString& Message = "" );

	// Call this to build up mode changes before flushing them.
	// All mode changes must be for the same channel.
	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual void StartModeChangeBuilder();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeChannelMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeChannelParamMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange, const FString& Param );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeChannelUserMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange, UKIRCUser* User );

	// Send built mode string.
	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual void FlushModeChanges( UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool QueryObjectModes( UKIRCObject* Object );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool QueryChannelModeList( UKIRCChannel* Channel, UKIRCMode* Mode );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChangeNickname( const FString& NewName );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool QueryTopic( UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool SetTopic( UKIRCChannel* Channel, const FString& TopicBody );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool GetServerNameList();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool ChannelList( const FString& Mask = "" );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool UpdateChannelNameList( UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Who( const FString& Mask );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool WhoIs( UKIRCUser* User );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool WhoWas( const FString& Name );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Away( const FString& Message );

	virtual bool Return();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool Quit( const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	virtual bool SendCommand( const FString& Command, UKIRCCommandResponseScanner* ResponseScanner = NULL );

	virtual bool SendCommandCallback( const FString& Command, TSubclassOf<UKIRCCommandResponseScanner> ScannerClass = NULL,
						  UObject* CallbackObject = NULL, FKIRCCommandResponseCallbackDelegate CallbackFunction = NULL );


	/*********************
	 * Message Handling *
	 *********************/

	virtual void HandleMessage( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	virtual FDelegateHandle AddMessageHandler( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual FDelegateHandle AddMessageHandler( int32 Numeric, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveMessageHandler( const FString& Command, FDelegateHandle Handle );

	virtual void RemoveMessageHandler( int32 Numeric, FDelegateHandle Handle );

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	static UKIRCBlueprintMessageHandler* CreateMessageHandler( UKIRCClient* Client, TSubclassOf<UKIRCBlueprintMessageHandler> MessageHandlerClass,
															   bool bAutoRegister = true, bool bStoreReference = true );

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	static const FKIRCNumerics& GetNumerics();

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	static FString NumericToString( int32 Numeric );


	/*****************
	 * Miscellaneous *
	 *****************/

	UFUNCTION( Category = "KeshIRC|Controller|Client|Commands", BlueprintCallable )
	static FString CleanString( const FString& DisallowedCharactes, const FString& String, bool bAllowUpperOctet = true );

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	static const FKIRCInvalidCharacters& GetInvalidCharacters();

protected:

	friend class UKIRCServer;

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
	TArray<UKIRCMode*> UserModes;

	UPROPERTY()
	TMap<FString, FString> ChannelKeyCache;

	UPROPERTY( Category = "KeshIRC|Controller|Client", VisibleInstanceOnly )
	TArray<UKIRCCommandResponseScanner*> CommandResponseScanners;

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

	void OnNickNegotiationFatalHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNickNegotaitionNextNickHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// Network info
	void OnNetworkWelcomeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNetworkInfoHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnServerSettingHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// MOTD
	void OnMOTDStartHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnMOTDLineHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnMOTDEndHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNoMOTDHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// Channel join
	void OnTopicBodyHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnTopicDetailsHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNameListHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	// Trigger events
	void OnMessageHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnNickChangeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnJoinHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnPartHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnModeChangeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnTopicChangeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnKickHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );	
	void OnInviteHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	void OnQuitHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );


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
	void OnUserModeEvent( UKIRCUser* UserChanged, UKIRCMode* Mode, EKIRCModeChange ModeChange );

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
	void OnChannelModeEvent( UKIRCChannel* Channel, UKIRCUser* Souce, UKIRCMode* Mode, EKIRCModeChange ModeChange, const FString& Param );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnChannelUserModeEvent( UKIRCChannel* Channel, UKIRCUser* Source, UKIRCMode* Mode, EKIRCModeChange ModeChange, UKIRCUser* Target );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnTopicChangeEvent( UKIRCChannel* Channel, UKIRCUser* Source, const FString& Topic );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnNickNameChangedEvent( UKIRCUser* UserChangingNick, const FString& OldName );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnUnhandledNumericEvent( UKIRCServer* ServerNotHandling, int32 Numeric, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Client", BlueprintImplementableEvent )
	void OnUnhandledRawMessageEvent( UKIRCServer* ServerNotHandling, const FString& Message );
};
