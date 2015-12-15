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
	S_Waiting UMETA( DisplayName = "Waiting" ),
	S_Success UMETA( DisplayName = "Success" ),
	S_Failure UMETA( DisplayName = "Failure" )
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FKIRCServerConnected, UKIRCServer* const, Server );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FKIRCUserRegistered, UKIRCClient* const, Client );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCServerDisconnected, UKIRCServer* const, Server, EKIRCServerDisconnectReason, Reason );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCConnectionError, UKIRCServer* const, Server, const FString&, ErrorString );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FKIRCUserModeChange, UKIRCUser* const, Source, UKIRCMode* const, Mode, EKIRCModeChange, ModeChange );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCServerRaw, UKIRCServer* const, Server, const FString&, RawLine );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCMOTDComplete, UKIRCClient* const, Client, const TArray<FString>&, MOTDLines );

// Source will be null if it's form the server (numerics)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FKIRCIncomingMessageHandler, UKIRCUser* const, Source, const FString&, Command, const TArray<FString>&, Params, const FString&, Message );
typedef void( __cdecl UObject::*FKIRCIncomingMessageHandlerDelegate )( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FKIRCUnhandledNumeric, UKIRCServer* const, Server, int32, Numeric, const TArray<FString>&, Params, const FString&, Message );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FKIRCUserMessage, UKIRCUser* const, Source, UKIRCChannel* const, Channel, EKIRCMessageType, MessageType, const FString&, Message );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCChannelInvite, UKIRCUser* const, Source, const FString&, ChannelName );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCChannelJoin, UKIRCChannel* const, Channel, UKIRCUser* const, Source );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FKIRCChannelPart, UKIRCChannel* const, Channel, UKIRCUser* const, Source, const FString&, Message );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FKIRCChannelKick, UKIRCChannel* const, Channel, UKIRCUser* const, Kicker, UKIRCUser* const, Kickee, const FString&, Reason );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams( FKIRCChannelModeChange, UKIRCChannel* const, Channel, UKIRCUser* const, Source, UKIRCMode* const, Mode, EKIRCModeChange, ModeChange, const FString&, Param );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams( FKIRCChannelUserModeChange, UKIRCChannel* const, Channel, UKIRCUser* const, Source, UKIRCMode* const, Mode, EKIRCModeChange, ModeChange, UKIRCUser* const, Target );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FKIRCChannelTopicChange, UKIRCChannel* const, Channel, UKIRCUser* const, Source, const FString&, TopicBody );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCChannelBodyReceive, UKIRCChannel* const, Channel, const FString&, TopicBody );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams( FKIRCChannelDetailsReceive, UKIRCChannel* const, Channel, const FString&, AuthorName, const FDateTime&, Date, const FString&, AuthorIdent, const FString&, AuthorHost, const FString&, TopicBody );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCChannelNameList, UKIRCChannel* const, Channel, const TArray<FKIRCChannelUserInfo>&, UserInfoList );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCUserNickNameChange, UKIRCUser* const, Source, const FString&, OldNickName );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCUserQuit, UKIRCUser* const, Source, const FString&, Message );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FKIRCCommandResponse, UKIRCClient* const, Client, const UKIRCCommandResponseScanner* const, CommandResponseScanner );

DECLARE_DYNAMIC_DELEGATE_OneParam( FKIRCCommandResponseCallback, UKIRCCommandResponseScanner* const, CommandResponseScanner );
typedef void( __cdecl UObject::*FKIRCCommandResponseCallbackDelegate )( UKIRCCommandResponseScanner* const CommandResponseScanner );
