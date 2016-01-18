// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCWhoWasCommandResponseScanner.generated.h"

class UKIRCClient;

USTRUCT( BlueprintType )
struct FKIRCWhoWasResponseInfo
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
	FString ServerHost = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString ServerInfo = "";
};

/**
* Handles responses to the WhoWas command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCWhoWasCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCWhoWasCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	FKIRCWhoWasResponseInfo GetUserInfoBP() const { return UserInfo; }
	const FKIRCWhoWasResponseInfo& GetUserInfo() const { return UserInfo; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	FKIRCWhoWasResponseInfo UserInfo;

};
