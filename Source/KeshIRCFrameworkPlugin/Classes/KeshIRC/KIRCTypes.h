// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KIRCTypes.generated.h"

#undef NULL
#define NULL nullptr
#define KIRCLog( Level, Text ) UE_LOG( LogKeshIRCFramework, Level, TEXT( Text ) )
#define KIRCLogF( Level, Format, ... ) UE_LOG( LogKeshIRCFramework, Level, TEXT( Format ), __VA_ARGS__ )

class UKIRCServer;
class UKIRCObject;
class UKIRCUser;
class UKIRCChannel;
class UKIRCMode;
class UKIRCClient;
class UKIRCCommandResponseScanner;
class UKIRCBlueprintMessageHandler;

// Global (except halfop..!)
#define MODE_USER_AWAY "a"
#define MODE_USER_INVISIBLE "i"
#define MODE_USER_REGISTERED_ONLY_MESSAGE "R"
#define MODE_USER_WALLOPS "w"
#define MODE_USER_HIDE_HOST "x"

#define MODE_CHANNEL_NO_CTCP "C"
#define MODE_CHANNEL_NO_COLOURS "c"
#define MODE_CHANNEL_INVITE_ONLY "i"
#define MODE_CHANNEL_KEY "k"
#define MODE_CHANNEL_USER_LIMIT "l"
#define MODE_CHANNEL_MODERATED "m"
#define MODE_CHANNEL_NO_EXTERNAL_MESSAGES "n"
#define MODE_CHANNEL_PRIVATE "p"
#define MODE_CHANNEL_BLOCK_UNREGISTERED "r"
#define MODE_CHANNEL_SECRET "s"
#define MODE_CHANNEL_ONLY_OPS_SET_TOPIC "t"

#define MODE_CHANNEL_LIST_BAN "b"

#define MODE_CHANNEL_USER_HALFOP "h"
#define MODE_CHANNEL_USER_OP "o"
#define MODE_CHANNEL_USER_VOICE "v"

#define TAG_USER_HALFOP '%'
#define TAG_USER_OP '@'
#define TAG_USER_VOICE '+'

// Quakenet
#define MODE_USER_DEAF "d"

#define MODE_CHANNEL_DELAYED_JOIN "D"
#define MODE_CHANNEL_HIDDEN_USERS "d"
#define MODE_CHANNEL_MODERATE_UNREGISTERED "M"
#define MODE_CHANNEL_SINGLE_CHANNEL_MESSAGES_ONLY "T"
#define MODE_CHANNEL_HIDE_LEAVE_MESSAGES "u"

// Freenode
#define MODE_USER_DEAF2 "D"
#define MODE_USER_MESSAGE_WHITELIST "g"
#define MODE_USER_NO_FORWARD "Q"

#define MODE_CHANNEL_JOIN_FORWARD "f"
#define MODE_CHANNEL_FORWARD_ENABLE "F"
#define MODE_CHANNEL_ANYONE_INVITE "g"
#define MODE_CHANNEL_JOIN_THROTTLE "j"
#define MODE_CHANNEL_LARGE_LISTS "L"
#define MODE_CHANNEL_PERMANENT "P"
#define MODE_CHANNEL_BLOCK_FORWARDS "Q"
#define MODE_CHANNEL_SSL_ONLY "S"
#define MODE_CHANNEL_REDUCED_MODERATION "z"

#define MODE_CHANNEL_LIST_BAN_EXEMPTION "e"
#define MODE_CHANNEL_LIST_INVITE_ONLY_EXEMPTION "I"
#define MODE_CHANNEL_LIST_QUIET "q"


USTRUCT( BlueprintType )
struct FKIRCChannelUserInfo
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly, Meta = ( DisplayName = "Channel" ) )
	UKIRCChannel* Channel;

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly, Meta = ( DisplayName = "User" ) )
	UKIRCUser* User;

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly, Meta = ( DisplayName = "Join Time" ) )
	FDateTime JoinTime;
		
	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly, Meta = ( DisplayName = "User Modes" ) )
	TArray<UKIRCMode*> Modes;

};

UENUM( BlueprintType )
enum class EKIRCMessageType : uint8
{
	T_Message UMETA( DisplayName = "Message" ),
	T_Emote   UMETA( DisplayName = "Emote" ),
	T_Notice  UMETA( DisplayName = "Notice" ),
	T_CTCP    UMETA( DisplayName = "CTCP" )
};

UENUM( BlueprintType )
enum class EKIRCServerDisconnectReason : uint8
{
	R_ByUser   UMETA( DisplayName = "By User" ),
	R_ByServer UMETA( DisplayName = "By Server" ),
	R_Socket   UMETA( DisplayName = "Network Error" )
};

UENUM( BlueprintType )
enum class EKIRCModeChange : uint8
{
	M_Add    UMETA( DisplayName = "Add Mode" ),
	M_Remove UMETA( DisplayName = "Remove Mode" )
};

UENUM( BlueprintType )
enum class EKIRCModeType : uint8
{
	T_None          UMETA( DisplayName = "Not Set" ),
	T_User          UMETA( DisplayName = "User" ),
	T_Channel_Unary UMETA( DisplayName = "Channel" ),
	T_Channel_Param UMETA( DisplayName = "Channel Param" ),
	T_Channel_List  UMETA( DisplayName = "Channel List" ),
	T_Channel_User  UMETA( DisplayName = "Channel User" )
};

UENUM( BlueprintType )
enum class EKIRCServerState : uint8
{
	S_Disconnected UMETA( DisplayName = "Disconnected" ),
	S_Connecting   UMETA( DisplayName = "Connecting" ),
	S_Connected    UMETA( DisplayName = "Connected" ),
	S_Error        UMETA( DisplayName = "Error" )
};

USTRUCT( BlueprintType )
struct FKIRCModeListContainer
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly )
	TArray<FString> List;
};

USTRUCT( BlueprintType )
struct FKIRCInvalidCharacters
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly )
	FString NickName = "\r\n\0 @!+%#\"$&*+";

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly )
	FString Ident = "\r\n\0@!+%#\"$&*+";

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly )
	FString RealName = "\r\n\0@!+%#\"$&*+";

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly )
	FString Command = "\r\n";
};


DECLARE_MULTICAST_DELEGATE_OneParam( FKIRCServerConnected, UKIRCServer* )
DECLARE_MULTICAST_DELEGATE_OneParam( FKIRCUserRegistered, UKIRCClient* )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCServerDisconnected, UKIRCServer*, EKIRCServerDisconnectReason );
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCConnectionError, UKIRCServer*, const FString& )
DECLARE_MULTICAST_DELEGATE_ThreeParams( FKIRCUserModeChange, UKIRCUser*, UKIRCMode*, EKIRCModeChange )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCServerRaw, UKIRCServer*, const FString& )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCMOTDComplete, UKIRCClient*, const TArray<FString>& )

DECLARE_DELEGATE_OneParam( FKIRCCommandResponseCallback, UKIRCCommandResponseScanner* )
typedef void( __cdecl UObject::*FKIRCCommandResponseCallbackDelegate )( UKIRCCommandResponseScanner* Scanner );

// Source will be null if it's form the server (numerics)
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCIncomingMessageHandler, UKIRCUser* /*Source*/, const FString& /*Command*/, const TArray<FString>& /*Params*/, const FString& /*Message*/ )
typedef void( __cdecl UObject::*FKIRCIncomingMessageHandlerDelegate )( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

// TArray is the space-delimitered list of tokens before the first colon
// FString is everything after the first colon
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCUnhandledNumeric, UKIRCServer*, int32, const TArray<FString>&, const FString& )

// Source of the message
// Channel message sent to (null if a pm)
// Type of message
// Text of the message
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCUserMessage, UKIRCUser*, UKIRCChannel*, EKIRCMessageType, const FString& )

// Source of the invite
// Channel being invited to (string, not obj)
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelInvite, UKIRCUser*, const FString& )

// User joining the channel
// Channel joined
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelJoin, UKIRCChannel*, UKIRCUser* )

// User parting the channel
// Channel parted
// Part message
DECLARE_MULTICAST_DELEGATE_ThreeParams( FKIRCChannelPart, UKIRCChannel*, UKIRCUser*, const FString& )

// Channel being kicked from
// User doing the kicking
// User being kicked
// Reason
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCChannelKick, UKIRCChannel*, UKIRCUser*, UKIRCUser*, const FString& )

// Channel changing mode
// User adding mode
// Mode added
// Param string
DECLARE_MULTICAST_DELEGATE_FiveParams( FKIRCChannelModeChange, UKIRCChannel*, UKIRCUser*, UKIRCMode*, EKIRCModeChange, const FString& )

// Channel 
// User adding mode
// Mode added
// User changing mode
DECLARE_MULTICAST_DELEGATE_FiveParams( FKIRCChannelUserModeChange, UKIRCChannel*, UKIRCUser*, UKIRCMode*, EKIRCModeChange, UKIRCUser* )

// Channel changing topic
// User changing the topic
// New topic
DECLARE_MULTICAST_DELEGATE_ThreeParams( FKIRCChannelTopicChange, UKIRCChannel*, UKIRCUser*, const FString& )

// Channel
// New topic
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelBodyReceive, UKIRCChannel*, const FString& )

// Channel
// New topic
DECLARE_MULTICAST_DELEGATE_SixParams( FKIRCChannelDetailsReceive, UKIRCChannel*, const FString&, FDateTime, const FString&, const FString&, const FString& )

// Channel
// Name/most list
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelNameList, UKIRCChannel*, const TArray<FKIRCChannelUserInfo>& )

// User changing name
// Old nickname
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCUserNickNameChange, UKIRCUser*, const FString& )

// User quiting
// Quit message
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCUserQuit, UKIRCUser*, const FString& )
