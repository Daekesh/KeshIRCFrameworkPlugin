// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCServer.generated.h"

class FResolveInfo;
class FInternetAddr;
class FSocket;

/**
* Representation of an IRC server connection.
*/
UCLASS( Category = "KeshIRC | Model | Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCServer : public UObject
{
	GENERATED_BODY()

public:

	/********************
	 * Server Functions *
	 ********************/

	// Initiate the connection to the server.
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual bool Connect();

	// Force an ungraceful disconnect from the server. User Quit for a graceful one.
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual bool Disconnect();

	// Resets the server so it can be connected again.
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual void Reset();


	/******************
	 * Server Details *
	 ******************/

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetName() const { return Name; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetHost() const { return Host; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	int32 GetPort() const { return Port; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetPassword() const { return Password; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetNetworkName() const { return NetworkName; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetHostActual() const { return HostActual; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetVersion() const { return Version; }

	// Setting name is converted to upper case
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	bool HasSetting( const FString& Setting ) const { return Settings.Contains( Setting.ToUpper() ); }

	// Setting name is converted to upper case
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	const FString& GetSetting( const FString& Setting ) const { return Settings[ Setting.ToUpper() ]; }

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	EKIRCServerState GetState() const { return State; }

	
	/**********************
	 * Channels and Users *
	 **********************/

	UFUNCTION( Category = "KeshIRC | Model | Server | Channels", BlueprintCallable )
	int32 GetChannelCount() const { return Channels.Num(); }

	// Returns the channel by name. Channel must start with a valid channel prefix.
	UFUNCTION( Category = "KeshIRC | Model | Server | Channels", BlueprintCallable )
	UKIRCChannel* GetChannelByName( const FString& Name ) const;

	// If the channel doesn't exist, a skeleton channel is created and returned.
	UFUNCTION( Category = "KeshIRC | Model | Server | Channels", BlueprintCallable )
	UKIRCChannel* EnsureChannel( const FString& Name );

	// Returns all the channels the user is currently in.
	UFUNCTION( Category = "KeshIRC | Model | Server | Channels", BlueprintCallable )
	TArray<UKIRCChannel*> GetChannels() const
	{
		TArray<UKIRCChannel*> ChannelList;
		Channels.GenerateValueArray( ChannelList );
		return ChannelList;
	}

	UFUNCTION( Category = "KeshIRC | Model | Server | Users", BlueprintCallable )
	int32 GetUserCount() const { return Users.Num(); }

	UFUNCTION( Category = "KeshIRC | Model | Server | Users", BlueprintCallable )
	UKIRCUser* GetUserByName( const FString& Name ) const;

	// If the user doesn't exist, a skeleton user is created and returned.
	UFUNCTION( Category = "KeshIRC | Model | Server | Users", BlueprintCallable )
	UKIRCUser* EnsureUser( const FString& Name, const FString& Ident = "", const FString& Host = "" );

	// Returns all the users currently visible to the user.
	UFUNCTION( Category = "KeshIRC | Model | Server | Users", BlueprintCallable )
	TArray<UKIRCUser*> GetUsers() const
	{
		TArray<UKIRCUser*> UserList;
		Users.GenerateValueArray( UserList );
		return UserList;
	}

	// Changes the user in the Users map to a new username.
	UFUNCTION( Category = "KeshIRC | Model | Server | Users", BlueprintCallable )
	void RenameUser( UKIRCUser* User, const FString& NewName );

	// Removes user from the user map.
	UFUNCTION( Category = "KeshIRC | Model | Server | Users", BlueprintCallable )
	void RemoveUser( UKIRCUser* User );

	// Removes channel from the channel map.
	UFUNCTION( Category = "KeshIRC | Model | Server | Channels", BlueprintCallable )
	void RemoveChannel( UKIRCChannel* Channel );


	/****************
	 * Server Modes *
	 ****************/

	// Returns the mode object for the given mode character.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | User", BlueprintCallable )
	UKIRCMode* GetUserMode( const FString& Mode ) const { return UserModes[ Mode ]; }

	// Returns true if the given user mode is available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | User", BlueprintCallable )
	bool IsUserModeAvailable( const FString& Mode ) const { return UserModes.Contains( Mode ); }

	// Returns a list (expensive) of the user modes available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | User", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableUserModes() const
	{
		TArray<UKIRCMode*> UserModeList;
		UserModes.GenerateValueArray( UserModeList );
		return UserModeList;
	}

	// Returns the mode object for the given mode character.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel", BlueprintCallable )
	UKIRCMode* GetChannelMode( const FString& Mode ) const { return ChannelModes[ Mode ]; }

	// Returns true if the given channel mode is available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel", BlueprintCallable )
	bool IsChannelModeAvailable( const FString& Mode ) const { return ChannelModes.Contains( Mode ); }

	// Returns a list (expensive) of the channel modes available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | User", BlueprintCallable )
		TArray<UKIRCMode*> GetAvailableChannelModes() const
	{
		TArray<UKIRCMode*> ChannelModesArray;
		ChannelModes.GenerateValueArray( ChannelModesArray );
		return ChannelModesArray;
	}


	/*****************
	 * Miscellaneous *
	 *****************/

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual bool Send( const FString& Command );

public:

	static const int32 MaxCommandLength = 256;

protected:

	friend class UKIRCClient;

	class FKIRCServerTicket : public FTickableGameObject
	{		
		UKIRCServer* Server;
	public:
		FKIRCServerTicket( UKIRCServer* Server ) { this->Server = Server; }		
		virtual bool IsTickableWhenPaused() const override { return true; }
		virtual bool IsTickableInEditor() const override { return true; }
		virtual bool IsTickable() const override { return Server != NULL; }
		virtual void Tick( float DeltaTime ) { Server->Tick(); }
		virtual TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT( FKIRCServerTicket, STATGROUP_Tickables ); }
	};


	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	FString Name; 

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	FString Host;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	int32 Port;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	FString Password;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	FString NetworkName;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	FString Version;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	FString HostActual;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	EKIRCServerState State;

	UPROPERTY( Category = "KeshIRC | Model | Server", VisibleInstanceOnly )
	UKIRCClient* Client;

	UPROPERTY()
	TMap<FString, UKIRCChannel*> Channels;

	UPROPERTY()
	TMap<FString, UKIRCUser*> Users;

	UPROPERTY()
	TMap<FString, UKIRCMode*> UserModes;

	UPROPERTY()
	TMap<FString, UKIRCMode*> ChannelModes;

	UPROPERTY()
	TMap<FString, FString> Settings;

	FKIRCServerTicket* Ticker;
	FResolveInfo* HostResolver;
	TSharedPtr<FInternetAddr> HostAddr;
	FSocket* Socket;
	uint8 SocketBuffer[ 1024 ];
	FString ReadBuffer;

	UKIRCServer( const class FObjectInitializer& ObjectInitializer );
	~UKIRCServer();

	bool InitServer( const FString& ServerName, const FString& Host, int32 Port, const FString& Password );

	void SetNetworkName( const FString& NewName ) { NetworkName = NewName; }

	void SetVersion( const FString& NewVersion ) { Version = NewVersion; }

	void SetHostActual( const FString& NewHost ) { HostActual = NewHost; }

	void SetSetting( const FString& Setting, const FString& Value ) { Settings[ Setting ] = Value; }
	
	virtual void SetState( EKIRCServerState State ) { this->State = State; }

	virtual void Tick();

	UKIRCMode* AddUserMode( const FString& ModeCharacter );

	UKIRCMode* AddChannelMode( const FString& ModeCharacter, EKIRCModeType ModeType );

	virtual void ParseLine( const FString& Line );

	virtual void OnConnected();

	virtual void OnDisconnected( EKIRCServerDisconnectReason Reason );

	virtual void OnConnectionError( const FString& Reason );

};
