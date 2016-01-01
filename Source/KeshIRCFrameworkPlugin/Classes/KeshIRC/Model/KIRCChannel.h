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
	const FString& GetJoinKey() const;

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool HasUserLimit() const;

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetUserLimit() const { return Limit; }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool IsChannelModeSet( const UKIRCMode* const Mode ) const { return ( IsChannelUnaryModeSet( Mode ) || IsChannelParamModeSet( Mode ) ); }

	// Get the number of modes set without parameters set on this channel
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetChannelUnaryModeCount() const { return UnaryModes.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool IsChannelUnaryModeSet( const UKIRCMode* const Mode ) const { return Mode == NULL ? false : UnaryModes.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetChannelUnaryModesBP() const
	{
		TArray<UKIRCMode*> ModeArray;

		for ( const UKIRCMode* const Mode : UnaryModes )
			ModeArray.Add( const_cast< UKIRCMode* >( Mode ) );

		return ModeArray;
	}
	const TArray<const UKIRCMode*>& GeChannelUnaryModes() const { return UnaryModes; }

	// Get the number of modes with params set on this channel
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetChannelParamModeCount() const { return ParamModes.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool IsChannelParamModeSet( const UKIRCMode* const Mode ) const { return Mode == NULL ? false : ParamModes.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetChannelParamModeModes() const
	{
		TArray<UKIRCMode*> ModeParam;
		ParamModes.GetKeys( ModeParam );
		return ModeParam;
	}

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	const FString& GetChannelParamModeValue( const UKIRCMode* const Mode ) const;

	const TMap<UKIRCMode*, FString>& GetChannelParamModes() const { return ParamModes; }

	// Get the number of mode lists known on this channel
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	int32 GetChannelListModeCount() const { return ListModes.Num(); }

	// Returns true if the given mode is set on this channel;
	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	bool HasChannelListMode( const UKIRCMode* const Mode ) const { return Mode == NULL ? false : ListModes.Contains( Mode ); }

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<UKIRCMode*> GetChannelListModeModes() const
	{
		TArray<UKIRCMode*> ModeList;
		ListModes.GetKeys( ModeList );
		return ModeList;
	}

	UFUNCTION( Category = "KeshIRC|Model|Channel", BlueprintCallable )
	TArray<FString> GetChannelListModeEntriesBP( const UKIRCMode* const Mode ) const { return GetChannelListModeEntries( Mode ); }
	const TArray<FString>& GetChannelListModeEntries( const UKIRCMode* const Mode ) const;

	const TMap<UKIRCMode*, FKIRCModeListContainer>& GetChannelListModes() const { return ListModes; }

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCUserMessage OnMessageDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelJoin OnJoinedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelPart OnPartedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelKick OnKickedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelModeChange OnChannelModeDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelUserModeChange OnChannelUserModeDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelTopicChange OnTopicChangedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelInvite OnInvitedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelBodyReceive OnTopicReceiveDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Channel", BlueprintAssignable )
	FKIRCChannelDetailsReceive OnTopicDetailsDelegate;

protected:

	friend class UKIRCServer;
	friend class UKIRCClient;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	FString TopicBody;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	FString TopicAuthor;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	FDateTime TopicDate;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	FDateTime Created;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	TArray<const UKIRCMode*> UnaryModes;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	TMap<UKIRCMode*, FString> ParamModes;

	UPROPERTY( /*Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly*/ )
	TMap<UKIRCMode*, FKIRCModeListContainer> ListModes;

	UPROPERTY( Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly )
	int32 Limit;

	UPROPERTY( /*Category = "KeshIRC|Channel|Channel", VisibleInstanceOnly*/)
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

	virtual void SetParamMode( const UKIRCMode* const Mode, const FString& Value );

	virtual void RemoveParamMode( const UKIRCMode* const Mode );

	virtual void AddListModeEntry( const UKIRCMode* const Mode, const FString& Entry );
	
	virtual void RemoveListModeEntry( const UKIRCMode* const Mode, const FString& Entry );

	virtual void SetLimit( int32 Limit ) { this->Limit = Limit; }

	virtual void AddUserMode( const UKIRCUser* const User, const UKIRCMode* const Mode );

	virtual void RemoveUserMode( const UKIRCUser* const User, const UKIRCMode* const Mode );

};
