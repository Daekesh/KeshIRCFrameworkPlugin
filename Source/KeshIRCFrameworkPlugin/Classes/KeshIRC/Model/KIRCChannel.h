// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KIRCChannel.generated.h"


/**
* Representation of a channel on an IRC server.
*/
UCLASS( Category = "KeshIRC|Model|Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCChannel : public UKIRCObject
{
	GENERATED_BODY()

public:

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable, BlueprintPure )
	static bool HasChannelPrefix( const FString& ObjectName );

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	const FString& GetTopicBody() const { return TopicBody; }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	const FString& GetTopicAuthor() const { return TopicAuthor; }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	FDateTime GetTopicDateBP() const { return TopicDate; }
	const FDateTime& GetTopicDate() const { return TopicDate; }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool HasTopic() const { return TopicDate.GetTicks() == 0; }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	FDateTime GetCreationDateBP() const { return Created; }
	const FDateTime& GetCreationDate() const { return Created; }

	// Get the number of users in this channel
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetUserCount() const { return Users.Num(); }

	// Returns true if the given user is in the channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool HasUser( const UKIRCUser* const User ) const { return User == NULL ? false : Users.Contains( User ); }

	// Returns the Channel User Info for this channel and the given user.
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	FKIRCChannelUserInfo GetChannelUserInfoBP( const UKIRCUser* const User ) const { return GetChannelUserInfo( User ); }
	const FKIRCChannelUserInfo& GetChannelUserInfo( const UKIRCUser* const User ) const;

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool DoesUserHaveJoinTime( const UKIRCUser* const User ) const
	{
		if ( !Users.Contains( User ) )
			return false;

		return ( Users[ User ].JoinTime.GetTicks() != 0 );
	}

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<UKIRCUser*> GetUsers() const 
	{ 
		TArray<UKIRCUser*> UserList;
		Users.GetKeys( UserList );
		return UserList;
	}

	// Checks if the channel has the +k mode
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool IsJoinKeySet() const;

	// Returns the channel key, if known.
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	const FString& GetJoinKey() const { return JoinKey; }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool HasUserLimit() const;

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetUserLimit() const { return Limit; }

	// Get the number of modes set without parameters set on this channel
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetChannelModeCount() const { return Modes.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool IsChannelModeSet( const UKIRCMode* const Mode ) const { return Mode == NULL ? false : Modes.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetChannelModesBP() const
	{
		TArray<UKIRCMode*> ModeArray;

		for ( const UKIRCMode* const Mode : Modes )
			ModeArray.Add( const_cast< UKIRCMode* >( Mode ) );

		return ModeArray;
	}
	const TArray<const UKIRCMode*>& GeChannelModes() const { return Modes; }

	// Get the number of modes set without parameters set on this channel
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetChannelModeListCount() const { return ModeLists.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool HasChannelModeList( const UKIRCMode* const Mode ) const { return Mode == NULL ? false : ModeLists.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetChannelModeListModes() const
	{
		TArray<UKIRCMode*> ModeList;
		ModeLists.GetKeys( ModeList );
		return ModeList;
	}

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<FString> GetChannelModeListValuesBP( const UKIRCMode* const Mode ) const { return GetChannelModeListValues( Mode ); }
	const TArray<FString>& GetChannelModeListValues( const UKIRCMode* const Mode ) const;

	const TMap<UKIRCMode*, FKIRCModeListContainer>& GetChannelModeLists() const { return ModeLists; }

	FKIRCUserMessage OnMessageDelegate;
	FKIRCChannelJoin OnJoinedDelegate;
	FKIRCChannelPart OnPartedDelegate;
	FKIRCChannelKick OnKickedDelegate;
	FKIRCChannelModeChange OnChannelModeDelegate;
	FKIRCChannelUserModeChange OnChannelUserModeDelegate;
	FKIRCChannelTopicChange OnTopicChangedDelegate;
	FKIRCChannelInvite OnInvitedDelegate;
	FKIRCChannelBodyReceive OnTopicReceiveDelegate;
	FKIRCChannelDetailsReceive OnTopicDetailsDelegate;

protected:

	friend class UKIRCServer;
	friend class UKIRCClient;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	FString TopicBody;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	FString TopicAuthor;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	FDateTime TopicDate;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	FDateTime Created;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	TArray<const UKIRCMode*> Modes;

	UPROPERTY( /*Category = "KeshIRC|Model|Mode", VisibleInstanceOnly*/ )
	TMap<UKIRCMode*, FKIRCModeListContainer> ModeLists;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	int32 Limit;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	FString JoinKey;

	UPROPERTY( /*Category = "KeshIRC|Model|Mode", VisibleInstanceOnly*/)
	TMap<UKIRCUser*, FKIRCChannelUserInfo> Users;

	UKIRCChannel( const class FObjectInitializer& ObjectInitializer );

	virtual void InitChannel( const FString& Name );

	virtual void SetTopicBody( const FString& Body ) { TopicBody = Body; }

	virtual void SetTopicAuthor( const FString& Author ) { TopicAuthor = Author; }

	virtual void SetTopicDate( const FDateTime& Date ) { TopicDate = Date; }

	virtual void SetCreated( const FDateTime& Date ) { Created = Date; }

	virtual void UserJoined( UKIRCUser* const User, bool bZeroTime = false );

	virtual void UserLeft( UKIRCUser* const User );

	virtual void AddUnaryMode( const UKIRCMode* const Mode );

	virtual void RemoveUnaryMode( const UKIRCMode* const Mode );

	virtual void AddListModeEntry( const UKIRCMode* const Mode, const FString& Entry );
	
	virtual void RemoveListModeEntry( const UKIRCMode* const Mode, const FString& Entry );

	virtual void SetLimit( int32 Limit ) { this->Limit = Limit; }

	virtual void SetJoinKey( const FString& JoinKey ) { this->JoinKey = JoinKey; }

	virtual void AddUserMode( const UKIRCUser* const User, const UKIRCMode* const Mode );

	virtual void RemoveUserMode( const UKIRCUser* const User, const UKIRCMode* const Mode );

};
