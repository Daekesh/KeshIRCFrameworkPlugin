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

USTRUCT( BlueprintType )
struct FKIRCModeListContainer
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Model|Channel", VisibleInstanceOnly, BlueprintReadOnly )
	TArray<FString> List;
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
	S_Disconnected        UMETA( DisplayName = "Disconnected" ),
	S_ConnectionRequested UMETA( DisplayName = "Connection Requested" ),
	S_Connecting          UMETA( DisplayName = "Connecting" ),
	S_Connected           UMETA( DisplayName = "Connected" ),
	S_Error               UMETA( DisplayName = "Error" )
};

UENUM( BlueprintType )
enum class EKIRCModeParamRequired : uint8
{
	R_Never      UMETA( DisplayName = "Never" ),
	R_AddingOnly UMETA( DisplayName = "Adding Only" ),
	R_Always     UMETA( DisplayName = "Always" )
};

UENUM( BlueprintType )
enum class EKIRCCommandState : uint8
{
	S_Waiting UENUM( DisplayName = "Waiting" ),
	S_Success UENUM( DisplayName = "Success" ),
	S_Failure UENUM( DisplayName = "Failure" )
};

DECLARE_MULTICAST_DELEGATE_OneParam( FKIRCServerConnected, UKIRCServer* const )
DECLARE_MULTICAST_DELEGATE_OneParam( FKIRCUserRegistered, UKIRCClient* const )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCServerDisconnected, UKIRCServer* const, EKIRCServerDisconnectReason )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCConnectionError, UKIRCServer* const, const FString& )
DECLARE_MULTICAST_DELEGATE_ThreeParams( FKIRCUserModeChange, UKIRCUser* const, const UKIRCMode* const, EKIRCModeChange )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCServerRaw, UKIRCServer* const, const FString& )
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCMOTDComplete, UKIRCClient* const, const TArray<FString>& )

DECLARE_DELEGATE_OneParam( FKIRCCommandResponseCallback, const UKIRCCommandResponseScanner* const )
typedef void( __cdecl UObject::*FKIRCCommandResponseCallbackDelegate )( const UKIRCCommandResponseScanner* const Scanner );

// Source will be null if it's form the server (numerics)
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCIncomingMessageHandler, UKIRCUser* const /*Source*/, const FString& /*Command*/, const TArray<FString>& /*Params*/, const FString& /*Message*/ )
typedef void( __cdecl UObject::*FKIRCIncomingMessageHandlerDelegate )( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

// TArray is the space-delimitered list of tokens before the first colon
// FString is everything after the first colon
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCUnhandledNumeric, UKIRCServer* const, int32, const TArray<FString>&, const FString& )

// Source of the message
// Channel message sent to (null if a pm)
// Type of message
// Text of the message
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCUserMessage, UKIRCUser* const, UKIRCChannel* const, EKIRCMessageType, const FString& )

// Source of the invite
// Channel being invited to (string, not obj)
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelInvite, UKIRCUser* const, const FString& )

// User joining the channel
// Channel joined
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelJoin, UKIRCChannel* const, UKIRCUser* const )

// User parting the channel
// Channel parted
// Part message
DECLARE_MULTICAST_DELEGATE_ThreeParams( FKIRCChannelPart, UKIRCChannel* const, UKIRCUser* const, const FString& )

// Channel being kicked from
// User doing the kicking
// User being kicked
// Reason
DECLARE_MULTICAST_DELEGATE_FourParams( FKIRCChannelKick, UKIRCChannel* const, UKIRCUser* const, UKIRCUser* const, const FString& )

// Channel changing mode
// User adding mode
// Mode added
// Param string
DECLARE_MULTICAST_DELEGATE_FiveParams( FKIRCChannelModeChange, UKIRCChannel* const, UKIRCUser* const, const UKIRCMode* const, EKIRCModeChange, const FString& )

// Channel 
// User adding mode
// Mode added
// User changing mode
DECLARE_MULTICAST_DELEGATE_FiveParams( FKIRCChannelUserModeChange, UKIRCChannel* const, UKIRCUser* const, const UKIRCMode* const, EKIRCModeChange, UKIRCUser* const )

// Channel changing topic
// User changing the topic
// New topic
DECLARE_MULTICAST_DELEGATE_ThreeParams( FKIRCChannelTopicChange, UKIRCChannel* const, UKIRCUser* const, const FString& )

// Channel
// New topic
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelBodyReceive, UKIRCChannel* const, const FString& )

// Channel
// New topic
DECLARE_MULTICAST_DELEGATE_SixParams( FKIRCChannelDetailsReceive, UKIRCChannel* const, const FString&, const FDateTime&, const FString&, const FString&, const FString& )

// Channel
// Name/most list
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCChannelNameList, UKIRCChannel* const, const TArray<FKIRCChannelUserInfo>& )

// User changing name
// Old nickname
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCUserNickNameChange, UKIRCUser* const, const FString& )

// User quiting
// Quit message
DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCUserQuit, UKIRCUser* const, const FString& )

DECLARE_MULTICAST_DELEGATE_TwoParams( FKIRCCommandResponse, UKIRCClient* const, const UKIRCCommandResponseScanner* const )
