// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCWhoIsCommandResponseScanner.generated.h"

class UKIRCClient;

USTRUCT( BlueprintType )
struct FKIRCWhoIsResponseChannelInfo
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString Channel = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bOp = false;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bHalfOp = false;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bVoice = false;
};

USTRUCT( BlueprintType )
struct FKIRCWhoIsResponseInfo
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString NickName = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString Ident = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString Host = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString RealName = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	TArray<FKIRCWhoIsResponseChannelInfo> Channels;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString ServerHost = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString ServerName = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString AuthName = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bAway = false;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString AwayMessage = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bIdle = false;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	int32 SecondsIdle = 0;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bOperator = false;
};


/**
* Handles responses to the WhoIs command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCWhoIsCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCWhoIsCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	FKIRCWhoIsResponseInfo GetUserInfoBP() const { return UserInfo; }
	const FKIRCWhoIsResponseInfo& GetUserInfo() const { return UserInfo; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	FKIRCWhoIsResponseInfo UserInfo;

};
