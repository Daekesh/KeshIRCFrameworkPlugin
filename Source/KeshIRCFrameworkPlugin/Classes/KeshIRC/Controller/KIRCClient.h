// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/KIRCNumerics.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KIRCClient.generated.h"

/**
* Representation of an IRC server connection and the user using it.
*/
UCLASS( Category = "KeshIRC | Client", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCClient : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	UKIRCServer* GetServer() const { return Server; }

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	UKIRCUser* GetUser() const { return User; }

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	bool HasRegistered() const { return bRegistered; }

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	const TArray<FString>& GetMessageOfTheDay() const { return MOTD; }

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	const TArray<FString>& GetNickNameList() const { return NickNameList; }

	// Returns the nodes currently set on the server user.
	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	const TArray<UKIRCMode*>& GetUserModes() const { return UserModes; }

	// Returns true if the server user has the given mode set.
	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	bool HasMode( UKIRCMode* Mode ) const { return Mode == NULL ? false : UserModes.Contains( Mode ); }

	// Returns true if the server user has the given mode set.
	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	bool HasModeString( const FString& Mode ) const;

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	bool CanSeeUser( const FString& Name ) const 
	{ 
		if ( Server == NULL )
			return false;

		return ( Server->GetUserByName( Name ) != NULL );
	}

	// Channel must start with a valid channel prefix.
	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	bool IsInChannel( const FString& Name ) const
	{ 
		if ( Server == NULL )
			return false;

		return ( Server->GetChannelByName( Name ) != NULL );
	}

	UFUNCTION( Category = "KeshIRC | Client", BlueprintCallable )
	const FString& GetCachedKeyForChannel( UKIRCChannel* Channel ) const;

	/**********
	 * Events *
	 **********/

	FKIRCServerConnected OnServerConnectedDelegate;
	FKIRCServerDisconnected OnServerDisconnectedDelegate;
	FKIRCConnectionError OnConnectionErrorDelegate;
	FKIRCUserRegistered OnRegisteredDelegate;
	FKIRCUserModeChange OnUserModeChangedDelegate;
	FKIRCChannelInvite OnInvitedDelegate;
	FKIRCMOTDComplete OnMOTDCompleteDelegate;
	FKIRCUserMessage OnUserMessageDelegate;
	FKIRCChannelJoin OnUserJoinedChannelDelegate;
	FKIRCChannelPart OnUserLeftChannelDelegate;
	FKIRCUserQuit OnUserQuitDelegate;
	FKIRCChannelKick OnUserKickedFromChannelDelegate;
	FKIRCChannelModeChange OnChannelModeChangedDelegate;
	FKIRCChannelUserModeChange OnChannelUserModeChangedDelegate;
	FKIRCChannelTopicChange OnChannelTopicChangedDelegate;
	FKIRCChannelTopicDiscover OnChannelTopicDiscoveredDelegate;
	FKIRCChannelNameList OnChannelNamesListReceivedDelegate;
	FKIRCUserNickNameChange OnNickNameChangedDelegate;
	FKIRCUnhandledNumeric OnUnhandledNumericDelegate;
	FKIRCServerRaw OnUnhandledRawDelegate;


	/************
	 * Commands *
	 ************/

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool Message( UKIRCObject* Object, EKIRCMessageType Type, const FString& Message );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool UserMode( UKIRCMode* Mode, EKIRCModeChange Change );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool JoinChannel( UKIRCChannel* Channel, const FString& Key = "" );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool PartChannel( UKIRCChannel* Channel, const FString& Message = "" );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool InviteUserToChannel( UKIRCUser* User, UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool KickUserFromChannel( UKIRCChannel* Channel, UKIRCUser* User, const FString& Message = "" );

	// Call this to build up mode changes before flushing them.
	// All mode changes must be for the same channel.
	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual void StartModeChangeBuilder();

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool ChangeChannelMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool ChangeChannelParamMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange, const FString& Param );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool ChangeChannelUserMode( UKIRCChannel* Channel, UKIRCMode* Mode, EKIRCModeChange ModeChange, UKIRCUser* User );

	// Send built mode string.
	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual void FlushModeChanges( UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool QueryObjectModes( UKIRCObject* Object );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool QueryChannelModeList( UKIRCChannel* Channel, UKIRCMode* Mode );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool ChangeNickname( const FString& NewName );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool QueryTopic( UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool SetTopic( UKIRCChannel* Channel, const FString& TopicBody );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool GetServerNameList();

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool ChannelList( const FString& Mask = "" );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool UpdateChannelNameList( UKIRCChannel* Channel );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool Who( const FString& Mask );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool WhoIs( UKIRCUser* User );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool WhoWas( const FString& Name );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool Away( const FString& Message );

	virtual bool Return();

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual bool Quit( const FString& Message );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	virtual void Command( const FString& Command, UKIRCCommandResponseScanner* Scanner );

	virtual bool Command( const FString& Command, TSubclassOf<UKIRCCommandResponseScanner> ScannerClass = NULL,
						  UObject* CallbackObject = NULL, FKIRCCommandResponseCallbackDelegate CallbackFunction = NULL );


	/*********************
	 * Message Handling *
	 *********************/

	virtual void HandleMessage( const FString& Line, UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	virtual FDelegateHandle AddMessageHandler( int32 Numeric, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual FDelegateHandle AddMessageHandler( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveMessageHandler( const FString& Command, FDelegateHandle Handle );

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	static UKIRCBlueprintMessageHandler* CreateMessageHandler( UKIRCClient* Client, TSubclassOf<UKIRCBlueprintMessageHandler> MessageHandlerClass,
															   bool bAutoRegister = true, bool bStoreReference = true );

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	static const FKIRCNumerics& GetNumerics();

	UFUNCTION( Category = "KeshIRC | Client | Commands", BlueprintCallable )
	static FString NumericToString( int32 Numeric );

protected:

	friend class UKIRCServer;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	TArray<FString> NickNameList;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	bool bRegistered;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	TArray<FString> MOTD;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	bool bBuildingModeString;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	EKIRCModeChange ModeChangeBuilderModeAction;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	FString ModeChangeBuilderModeList;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	FString ModeChangeBuilderParamList;

	TMap<FString, FKIRCIncomingMessageHandler> MessageHandlers;
	FCriticalSection CommandScannerQueueLock;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	UKIRCServer* Server;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	UKIRCUser* User;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	TArray<UKIRCMode*> UserModes;

	UPROPERTY()
	TMap<UKIRCChannel*, FString> ChannelKeyCache;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly )
	TArray<UKIRCCommandResponseScanner*> CommandResponseScanners;

	UPROPERTY( Category = "KeshIRC | Client", VisibleInstanceOnly, BlueprintReadWrite )
	TArray<UKIRCBlueprintMessageHandler*> BlueprintMessageHandlers;

	UKIRCClient( const class FObjectInitializer& ObjectInitializer );

	virtual void InitClient( UKIRCServer* Server, const FString& NickName, const TArray<FString>& AlternateNickNames, const FString& Ident, const FString& RealName );

	virtual void Register();

	virtual bool SendToServer( const FString& Command );

	void OnConnected();
	void OnDisconnected();
	void OnConnectionError( const FString& Reason );
	void OnRegister();

	virtual void SetupMessageHandlers();

	// Set network name
	void OnNetworkWelcomeHandler( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

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

	// Fill in later

};
