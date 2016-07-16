// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "Runtime/Core/Public/Misc/DateTime.h"
#include "KIRCServer.generated.h"

class FResolveInfo;
class FInternetAddr;
class FSocket;

/**
* Representation of an IRC server connection.
*/
UCLASS( ClassGroup = "KeshIRC|Model|Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCServer : public UObject
{
	GENERATED_BODY()

public:

	/********************
	 * Server Functions *
	 ********************/

	// Initiate the connection to the server.
	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	virtual bool Connect();

	// Force an ungraceful disconnect from the server. User Quit for a graceful one.
	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	virtual bool Disconnect();

	// Resets the server so it can be connected again.
	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	virtual void Reset();


	/******************
	 * Server Details *
	 ******************/

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetName() const { return Name; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetHost() const { return Host; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	int32 GetPort() const { return Port; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetPassword() const { return Password; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetNetworkName() const { return NetworkName; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetHostActual() const { return HostActual; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetVersion() const { return Version; }

	// Setting name is converted to upper case
	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	bool HasSetting( const FString& Setting ) const { return Settings.Contains( Setting.ToUpper() ); }

	// Setting name is converted to upper case
	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	const FString& GetSetting( const FString& Setting ) const;

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	EKIRCServerState GetState() const { return State; }

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	UKIRCClient* const GetClient() const { return Client; }

	
	/**********************
	 * Channels and Users *
	 **********************/

	UFUNCTION( Category = "KeshIRC|Model|Server|Channels", BlueprintCallable )
	int32 GetChannelCount() const { return Channels.Num(); }

	// Returns the channel by name. Channel must start with a valid channel prefix.
	UFUNCTION( Category = "KeshIRC|Model|Server|Channels", BlueprintCallable )
	UKIRCChannel* const GetChannelByName( const FString& Name ) const;

	// If the channel doesn't exist, a skeleton channel is created and returned.
	UFUNCTION( Category = "KeshIRC|Model|Server|Channels", BlueprintCallable )
	UKIRCChannel* const EnsureChannel( const FString& Name );

	// Returns all the channels the user is currently in.
	UFUNCTION( Category = "KeshIRC|Model|Server|Channels", BlueprintCallable )
	TArray<UKIRCChannel*> GetChannels() const
	{
		TArray<UKIRCChannel*> ChannelList;
		Channels.GenerateValueArray( ChannelList );
		return ChannelList;
	}

	UFUNCTION( Category = "KeshIRC|Model|Server|Users", BlueprintCallable )
	int32 GetUserCount() const { return Users.Num(); }

	UFUNCTION( Category = "KeshIRC|Model|Server|Users", BlueprintCallable )
	UKIRCUser* const GetUserByName( const FString& Name ) const;

	// If the user doesn't exist, a skeleton user is created and returned.
	UFUNCTION( Category = "KeshIRC|Model|Server|Users", BlueprintCallable )
	UKIRCUser* const EnsureUser( const FString& Name, const FString& Ident = "", const FString& Host = "" );

	// Returns all the users currently visible to the user.
	UFUNCTION( Category = "KeshIRC|Model|Server|Users", BlueprintCallable )
	TArray<UKIRCUser*> GetUsers() const
	{
		TArray<UKIRCUser*> UserList;
		Users.GenerateValueArray( UserList );
		return UserList;
	}

	// Changes the user in the Users map to a new username.
	UFUNCTION( Category = "KeshIRC|Model|Server|Users", BlueprintCallable )
	void OnUserNameChange( UKIRCUser* const User, const FString& NewName );

	// Removes user from the user map.
	UFUNCTION( Category = "KeshIRC|Model|Server|Users", BlueprintCallable )
	void RemoveUser( const UKIRCUser* const User );

	// Removes channel from the channel map.
	UFUNCTION( Category = "KeshIRC|Model|Server|Channels", BlueprintCallable )
	void RemoveChannel( UKIRCChannel* const Channel );


	/****************
	 * Server Modes *
	 ****************/

	// Returns the mode object for the given mode character.
	// Returned object is not const. Die BP, die!
	UFUNCTION( Category = "KeshIRC|Model|Server|Modes|User", BlueprintCallable )
	UKIRCMode* const GetUserModeBP( const FString& ModeCharacter ) const { return const_cast<UKIRCMode*>( GetUserMode( ModeCharacter ) ); }
	const UKIRCMode* const GetUserMode( const FString& ModeCharacter ) const
	{ 
		if ( !UserModes.Contains( ModeCharacter ) )
			return NULL; 
		
		return const_cast<UKIRCMode*>( UserModes[ ModeCharacter ] );
	}

	// Returns true if the given user mode is available on the server.
	UFUNCTION( Category = "KeshIRC|Model|Server|Modes|User", BlueprintCallable )
	bool IsUserModeAvailable( const FString& ModeCharacter ) const { return UserModes.Contains( ModeCharacter ); }

	// Returns a list (expensive) of the user modes available on the server.
	// Returned object is not const. Die BP, die!
	UFUNCTION( Category = "KeshIRC|Model|Server|Modes|User", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableUserModesBP() const
	{
		TArray<const UKIRCMode*> UserModesArrayConst = GetAvailableUserModes();
		TArray<UKIRCMode*> UserModesArray;

		for ( const UKIRCMode* const Mode : UserModesArrayConst )
			UserModesArray.Add( const_cast< UKIRCMode* >( Mode ) );

		return UserModesArray;
	}

	TArray<const UKIRCMode*> GetAvailableUserModes() const
	{
		TArray<const UKIRCMode*> UserModesArrayConst;
		UserModes.GenerateValueArray( UserModesArrayConst );
		return UserModesArrayConst;
	}

	// Returns the mode object for the given mode character.
	// Returned object is not const. Die BP, die!
	UFUNCTION( Category = "KeshIRC|Model|Server|Modes|Channel", BlueprintCallable )
	UKIRCMode* const GetChannelModeBP( const FString& ModeCharacter ) const { return const_cast<UKIRCMode*>( GetChannelMode( ModeCharacter ) ); }
	const UKIRCMode* const GetChannelMode( const FString& ModeCharacter ) const
	{ 
		if ( !ChannelModes.Contains( ModeCharacter ) )
			return NULL; 
		
		return ChannelModes[ ModeCharacter ];
	}

	// Returns true if the given channel mode is available on the server.
	UFUNCTION( Category = "KeshIRC|Model|Server|Modes|Channel", BlueprintCallable )
	bool IsChannelModeAvailable( const FString& ModeCharacter ) const { return ChannelModes.Contains( ModeCharacter ); }

	// Returns a list (expensive) of the channel modes available on the server.
	// Returned object is not const. Die BP, die!
	UFUNCTION( Category = "KeshIRC|Model|Server|Modes|Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetAvailableChannelModesBP() const
	{
		TArray<const UKIRCMode*> ChannelModesArrayConst = GetAvailableChannelModes();
		TArray<UKIRCMode*> ChannelModesArray;

		for ( const UKIRCMode* const Mode : ChannelModesArrayConst )
			ChannelModesArray.Add( const_cast< UKIRCMode* >( Mode ) );

		return ChannelModesArray;
	}

	TArray<const UKIRCMode*> GetAvailableChannelModes() const
	{
		TArray<const UKIRCMode*> ChannelModesArrayConst;
		ChannelModes.GenerateValueArray( ChannelModesArrayConst );
		return ChannelModesArrayConst;
	}


	/*****************
	 * Miscellaneous *
	 *****************/

	UFUNCTION( Category = "KeshIRC|Model|Server", BlueprintCallable )
	virtual bool Send( const FString& Command );

public:

	static const int32 MaxCommandLength = 256;

protected:

	friend class UKIRCClient;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	FString Name; 

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	FString Host;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	int32 Port;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	FString Password;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	FString NetworkName;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	FString Version;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	FString HostActual;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	EKIRCServerState State;

	UPROPERTY( Category = "KeshIRC|Model|Server", VisibleInstanceOnly )
	UKIRCClient* Client;

	UPROPERTY()
	TMap<FString, UKIRCChannel*> Channels;

	UPROPERTY()
	TMap<FString, UKIRCUser*> Users;

	UPROPERTY()
	TMap<FString, const UKIRCMode*> UserModes;

	UPROPERTY()
	TMap<FString, const UKIRCMode*> ChannelModes;

	UPROPERTY()
	TMap<FString, FString> Settings;

	FDelegateHandle dgCoreTickerDelegate;
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

	void SetSetting( const FString& Setting, const FString& Value ) { Settings.Emplace( Setting.ToUpper(), Value ); }
	
	virtual void SetState( EKIRCServerState NewState ) { State = NewState; }

	virtual bool CoreTick( float DeltaSeconds );
	virtual void Tick();

	const UKIRCMode* const AddUserMode( const FString& ModeCharacter );

	const UKIRCMode* const AddChannelMode( const FString& ModeCharacter, EKIRCModeType ModeType, EKIRCModeParamRequired ParamRequired );

	virtual void ParseLine( const FString& Line );

	virtual void OnConnected();

	virtual void OnDisconnected( EKIRCServerDisconnectReason Reason );

	virtual void OnConnectionError( const FString& Reason );

};
