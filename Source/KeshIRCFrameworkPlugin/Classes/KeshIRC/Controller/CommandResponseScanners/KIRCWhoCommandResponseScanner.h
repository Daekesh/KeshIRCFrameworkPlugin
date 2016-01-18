// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCWhoCommandResponseScanner.generated.h"

class UKIRCClient;

USTRUCT( BlueprintType )
struct FKIRCWhoResponseInfo
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString NickName = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString Ident = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString Host = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString RealName = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString Server = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	int32 Hops = 0;

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString CommonChannel = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString Flags = "";
};

/**
* Handles responses to the Who command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCWhoCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCWhoCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	TArray<FKIRCWhoResponseInfo> GetUserInfoBP() const { return UserInfo; }
	const TArray<FKIRCWhoResponseInfo>& GetUserInfo() const { return UserInfo; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	TArray<FKIRCWhoResponseInfo> UserInfo;

};
