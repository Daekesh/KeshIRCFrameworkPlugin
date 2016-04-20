// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Model/KIRCObject.h"
#include "KIRCUser.generated.h"


/**
* Representation of a user on an IRC server. Adminning of channel/user relation is 
* done through the channel object.
*/
UCLASS( ClassGroup = "KeshIRC|Model|Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCUser : public UKIRCObject
{
	GENERATED_BODY()

public:

	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	const FString& GetIdent() const { return Ident; }

	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	const FString& GetHost() const { return Host; }

	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	const FString& GetRealName() const { return RealName; }

	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	const FString& GetDisplayName();

	// Returns the nick!ident@host version of the user's identity.
	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	const FString& GetHostMask() const { return HostMask; }

	// Get the number of channels that we share with the user.
	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	int32 GetChannelCount() const { return Channels.Num(); }

	// Returns true if we're in the given channel.
	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	bool IsInChannel( const UKIRCChannel* const Channel ) const { return Channel == NULL ? false : Channels.Contains( Channel ); }

	// Returns the Channel User Info for this user and the given channel.
	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	FKIRCChannelUserInfo GetChannelUserInfoBP( const UKIRCChannel* const Channel  ) const { return GetChannelUserInfo( Channel ); }
	const FKIRCChannelUserInfo& GetChannelUserInfo( const UKIRCChannel* const Channel ) const;

	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	TArray<UKIRCChannel*> GetChannelsBP() const { return Channels; }
	const TArray<UKIRCChannel*>& GetChannels() const { return Channels; }

	// Returns true if the this user object is the client's user.
	UFUNCTION( Category = "KeshIRC|Model|User", BlueprintCallable )
	bool IsClient() const;

	// Fires on public and private messages. On messages, actions, notices and ctcps.
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserMessage OnMessageDelegate;

	// This user is receiving the invite
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelInvite OnInvitedDelegate; 

	// This user is sending the invite
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelInvite OnInviteDelegate; 

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelJoin OnJoinedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelPart OnLeftDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserQuit OnQuitDelegate;

	// This user is getting kicked
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelKick OnKickedDelegate; 

	// This user is doing the kicking
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelKick OnKickDelegate; 

	// This user changes modes
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserModeChange OnUserModeDelegate; 

	// We changed the mode
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelModeChange OnChannelModeDelegate; 

	// This user is doing the mode change
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelUserModeChange OnChannelUserModeDelegate; 

	// This user is getting the mode change
	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelUserModeChange OnChannelUserModeChangedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCUserNickNameChange OnNickNameChangedDelegate;

	UPROPERTY( Category = "KeshIRC|Controller|Client", BlueprintAssignable )
	FKIRCChannelTopicChange OnTopicChangedDelegate;
	
	static void ParseHostMask( const FString& Mask, FString& NickName, FString& Ident, FString& Host );

protected:

	friend class UKIRCServer;
	friend class UKIRCClient;
	friend class UKIRCChannel;
	friend class UKIRCWhoCommandResponseScanner;
	friend class UKIRCWhoIsCommandResponseScanner;
	friend class UKIRCWhoWasCommandResponseScanner;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	FString NickName;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	FString Ident;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	FString Host;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	FString RealName;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	FString HostMask;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	FString DisplayName;

	UPROPERTY( Category = "KeshIRC|Model|User", VisibleInstanceOnly )
	TArray<UKIRCChannel*> Channels;

	UKIRCUser( const class FObjectInitializer& ObjectInitializer );

	// Called when first created to init the user
	virtual void InitUser( const FString& Name, const FString& Ident, const FString& Host );

	// Refreshes the display name of the user
	virtual void UpdateMask( const FString& Mask );

	// Refreshes the display name of the user
	virtual void SetName( const FString& Name ) override;

	// Updates the real name of this user.
	virtual void SetRealName( const FString& NewRealName ) { RealName = NewRealName; }

	// Method to add the channel to the internal list. Returns true if added.
	virtual bool JoinChannel( UKIRCChannel* const Channel );

	// Method to remove the channel from the internal list. Returns true if removed.
	virtual bool LeaveChannel( const UKIRCChannel* const Channel );

};
