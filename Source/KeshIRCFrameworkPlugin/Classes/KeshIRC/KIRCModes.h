// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KIRCModes.generated.h"

USTRUCT( BlueprintType )
struct FKIRCUserModes
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Away = "a";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Invisible = "i";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString RegisteredUsersOnlyMessage = "R";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString WallOps = "w";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString HiddenHost = "x";

	// Quakenet

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_Deaf = "d";

	// Freenode

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_Deaf = "D";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_PrivateMessageWhiteList = "g";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_NoChannelForwarding = "Q";

};


USTRUCT( BlueprintType )
struct FKIRCChannelModes
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString BlockCTCP = "C";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString BlockColours = "c";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString InviteOnly = "i";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Key = "k";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString UserLimit = "l";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Moderated = "m";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString NoExternalMessages = "n";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Private = "p";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Secret = "s";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString OnlyOpsSetTopic = "t";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Ban = "b";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Voice = "v";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString VoicePrefix = "+";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString HalfOps = "h";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString HalfOpsPrefix = "%";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString Ops = "o";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString OpsPrefix = "@";

	// Quakenet
	
	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_DelayedJoin = "D";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_HiddenUsers = "d"; // Set by the server when D is on.

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_ModeratedUnregisteredUsers = "M";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_NoNotices = "N";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_NoMultichannelMessages = "T";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString QN_HidePartQuitMessages = "u";

	// Freenode

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_JoinForward = "f";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_EnableBanForwardWithoutOps = "F";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_EnableNonOpInvites = "g";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_JoinThrottle = "j";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_LargeModeLists = "L";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_Permanent = "P";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_BlockFowardedUsers = "Q";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_SSLOnly = "S";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_ReducedModeration = "z";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_BanExeption = "e";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_InviteExeption = "I";

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FString FN_QuietUser = "q";
};


USTRUCT( BlueprintType )
struct FKIRCModes
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FKIRCUserModes User;

	UPROPERTY( Category = "KeshIRC", VisibleAnywhere, BlueprintReadOnly )
	FKIRCChannelModes Channel;
};
