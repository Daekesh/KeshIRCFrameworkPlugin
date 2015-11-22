// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCServer.generated.h"


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

	// Factory method
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	static UKIRCServer* CreateServer( UObject* Outer, const FString& ServerName, const FString& Host, int32 Port,
									  const FString& Password, const FString& NickName, const FString& Ident, 
									  const FString& RealName, const TArray<FString>& AlternateNickNames,
									  TSubclassOf<UKIRCClient> ClientClass );

	// Initiate the connection to the server.
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual bool Connect();

	// Force an ungraceful disconnect from the server. User Quit for a graceful one.
	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual bool Disconnect();

	
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

	void SetNetworkName( const FString& NewName ) { NetworkName = NewName; }

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
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | Unary", BlueprintCallable )
	UKIRCMode* GetChannelUnaryMode( const FString& Mode ) const { return ChannelUnaryModes[ Mode ]; }

	// Returns true if the given channel mode is available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | Unary", BlueprintCallable )
	bool IsChannelUnaryModeAvailable( const FString& Mode ) const { return ChannelUnaryModes.Contains( Mode ); }

	// Returns a list (expensive) of the channel modes available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | Unary", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableChannelUnaryModes() const
	{
		TArray<UKIRCMode*> ChannelModeList;
		ChannelUnaryModes.GenerateValueArray( ChannelModeList );
		return ChannelModeList;
	}

	// Returns the mode object for the given mode character.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | List", BlueprintCallable )
	UKIRCMode* GetChannelListMode( const FString& Mode ) const { return ChannelListModes[ Mode ]; }

	// Returns true if the given channel mode is available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | List", BlueprintCallable )
	bool IsChannelListModeAvailable( const FString& Mode ) const { return ChannelListModes.Contains( Mode ); }

	// Returns a list (expensive) of the channel modes available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | List", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableChannelListModes() const
	{
		TArray<UKIRCMode*> ChannelListModeList;
		ChannelListModes.GenerateValueArray( ChannelListModeList );
		return ChannelListModeList;
	}

	// Returns the mode object for the given mode character.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | User", BlueprintCallable )
	UKIRCMode* GetChannelUserMode( const FString& Mode ) const { return ChannelUserModes[ Mode ]; }

	// Returns true if the given channel user mode is available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | User", BlueprintCallable )
	bool IsChannelUserModeAvailable( const FString& Mode ) const { return ChannelUserModes.Contains( Mode ); }

	// Returns a list (expensive) of the channel user modes available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel | User", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableChannelUserModes() const
	{
		TArray<UKIRCMode*> ChannelUserModeUser;
		ChannelUserModes.GenerateValueArray( ChannelUserModeUser );
		return ChannelUserModeUser;
	}

	// Returns the mode object for the given mode character.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel", BlueprintCallable )
	UKIRCMode* GetChannelMode( const FString& Mode ) const;	

	// Returns true if the given channel mode is available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel", BlueprintCallable )
	bool IsChannelModeAvailable( const FString& Mode ) const { return GetChannelMode( Mode ) != NULL; }

	// Returns a list (expensive) of the channel modes available on the server.
	UFUNCTION( Category = "KeshIRC | Model | Server | Modes | Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableChannelModes() const;


	/*****************
	 * Miscellaneous *
	 *****************/

	UFUNCTION( Category = "KeshIRC | Model | Server", BlueprintCallable )
	virtual void Command( const FString& Command );

public:

	static const int32 MaxCommandLength = 256;

protected:

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
	TMap<FString, UKIRCMode*> ChannelUnaryModes;

	UPROPERTY()
	TMap<FString, UKIRCMode*> ChannelListModes;

	UPROPERTY()
	TMap<FString, UKIRCMode*> ChannelUserModes;

	UKIRCServer( const class FObjectInitializer& ObjectInitializer );

	virtual void SetState( EKIRCServerState State ) { this->State = State; }

	virtual void ParseLine( const FString& Line );

	virtual void OnConnected();

	virtual void OnDisconnected( EKIRCServerDisconnectReason Reason );

	virtual void OnConnectionError( const FString& Reason );

};
