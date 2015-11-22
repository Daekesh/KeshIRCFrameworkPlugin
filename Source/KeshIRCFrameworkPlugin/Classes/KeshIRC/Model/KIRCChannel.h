// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KIRCChannel.generated.h"

USTRUCT( BlueprintType )
struct FKIRCModeListContainer
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly, BlueprintReadOnly )
	TArray<FString> List;

};


/**
* Representation of a channel on an IRC server.
*/
UCLASS( Category = "KeshIRC | Model | Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCChannel : public UKIRCObject
{
	GENERATED_BODY()

public:

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	static bool HasChannelPrefix( const FString& ObjectName );

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const FString& GetTopicBody() const { return TopicBody; }

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const FString& GetTopicAuthor() const { return TopicAuthor; }

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const FDateTime& GetTopicDate() const { return TopicDate; }

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	bool HasTopic() const { return TopicDate.GetTicks() == 0; }

	// Get the number of users in this channel
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	int32 GetUserCount() const { return Users.Num(); }

	// Returns true if the given user is in the channel;
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	bool HasUser( UKIRCUser* User ) const { return User == NULL ? false : Users.Contains( User ); }

	// Returns the Channel User Info for this channel and the given user.
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const FKIRCChannelUserInfo& GetChannelUserInfo( UKIRCUser* User );

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	TArray<UKIRCUser*> GetUsers() const 
	{ 
		TArray<UKIRCUser*> UserList;
		Users.GetKeys( UserList );
		return UserList;
	}

	// Checks if the channel has the +k mode
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	bool IsJoinKeySet() const;

	// Returns the channel key, if known.
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const FString& GetJoinKey() const { return JoinKey; }

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	bool HasUserLimit() const;

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	int32 GetUserLimit() const { return Limit; }

	// Get the number of modes set without parameters set on this channel
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	int32 GetChannelModeCount() const { return Modes.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	bool IsChannelModeSet( UKIRCMode* Mode ) const { return Mode == NULL ? false : Modes.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const TArray<UKIRCMode*>& GeChannelModes() const { return Modes; }

	// Get the number of modes set without parameters set on this channel
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	int32 GetChannelModeListCount() const { return ModeLists.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	bool HasChannelModeList( UKIRCMode* Mode ) const { return Mode == NULL ? false : ModeLists.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetChannelModeListModes() const
	{
		TArray<UKIRCMode*> ModeList;
		ModeLists.GetKeys( ModeList );
		return ModeList;
	}

	UFUNCTION( Category = "KeshIRC | Model | Channel", BlueprintCallable )
	const TArray<FString>& GetChannelModeListValues( UKIRCMode* Mode ) const;

	const TMap<UKIRCMode*, FKIRCModeListContainer>& GetChannelModeLists() const { return ModeLists; }

	FKIRCUserMessage OnUserMessage;
	FKIRCChannelJoin OnUserJoinedChannel;
	FKIRCChannelPart OnUserLeftChannel;
	FKIRCChannelKick OnUserKickedFromChannel;
	FKIRCChannelModeChange OnChannelModeChanged;
	FKIRCChannelUserModeChange OnChannelUserModeChanged;
	FKIRCChannelTopicChange OnChannelTopicChanged;
	FKIRCChannelTopicDiscover OnChannelTopicDiscovered;
	FKIRCChannelNameList OnChannelNamesListReceived;


protected:

	friend class UKIRCServer;
	friend class UKIRCClient;

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly )
	FString TopicBody;

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly )
	FString TopicAuthor;

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly )
	FDateTime TopicDate;

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly )
	TArray<UKIRCMode*> Modes;

	UPROPERTY( /*Category = "KeshIRC | Model | Mode", VisibleInstanceOnly*/ )
	TMap<UKIRCMode*, FKIRCModeListContainer> ModeLists;

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly )
	int32 Limit;

	UPROPERTY( Category = "KeshIRC | Model | Mode", VisibleInstanceOnly )
	FString JoinKey;

	UPROPERTY( /*Category = "KeshIRC | Model | Mode", VisibleInstanceOnly*/)
	TMap<UKIRCUser*, FKIRCChannelUserInfo> Users;

	UKIRCChannel( const class FObjectInitializer& ObjectInitializer );

	virtual void InitChannel( const FString& Name );

	virtual void SetTopicBody( const FString& Body ) { TopicBody = Body; }

	virtual void SetTopicAuthor( const FString& Author ) { TopicAuthor = Author; }

	virtual void SetTopicDate( const FDateTime& Date ) { TopicDate = Date; }

	virtual void UserJoined( UKIRCUser* User );

	virtual void UserLeft( UKIRCUser* User );

	virtual void AddUnaryMode( UKIRCMode* Mode );

	virtual void RemoveUnaryMode( UKIRCMode* Mode );

	virtual void AddListModeEntry( UKIRCMode* Mode, const FString& Entry );
	
	virtual void RemoveListModeEntry( UKIRCMode* Mode, const FString& Entry );

	virtual void SetLimit( int32 Limit ) { this->Limit = Limit; }

	virtual void SetJoinKey( const FString& JoinKey ) { this->JoinKey = JoinKey; }

	virtual void AddUserMode( UKIRCUser* User, UKIRCMode* Mode );

	virtual void RemoveUserMode( UKIRCUser* User, UKIRCMode* Mode );

};
