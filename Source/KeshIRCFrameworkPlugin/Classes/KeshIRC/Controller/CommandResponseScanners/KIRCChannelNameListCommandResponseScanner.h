// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCChannelNameListCommandResponseScanner.generated.h"

class UKIRCClient;

USTRUCT(BlueprintType)
struct FKIRCChannelNameListResponseInfo
{
	GENERATED_BODY()
	
	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString User = "";

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bOp = false;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bHalfOp = false;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	bool bVoice = false;
};

/**
* Handles responses to the Names command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCChannelNameListCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCChannelNameListCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	TArray<FKIRCChannelNameListResponseInfo> GetUsersBP() const { return Users; }
	const TArray<FKIRCChannelNameListResponseInfo>& GetUsers() const { return Users; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	TArray<FKIRCChannelNameListResponseInfo> Users;

};
