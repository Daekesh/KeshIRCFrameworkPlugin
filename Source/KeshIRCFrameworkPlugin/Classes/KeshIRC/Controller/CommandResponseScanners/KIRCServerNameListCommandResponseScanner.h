// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCServerNameListCommandResponseScanner.generated.h"

class UKIRCClient;

USTRUCT( BlueprintType )
struct FKIRCServerNameListResponseUserInfo
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString User = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	bool bOp = false;

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	bool bHalfOp = false;

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	bool bVoice = false;
};

USTRUCT( BlueprintType )
struct FKIRCServerNameListResponseInfo
{
	GENERATED_BODY()

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	FString Channel = "";

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleAnywhere, BlueprintReadOnly )
	TArray<FKIRCServerNameListResponseUserInfo> Users;
};
	

/**
* Handles responses to the Names command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCServerNameListCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCServerNameListCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	TArray<FKIRCServerNameListResponseInfo> GetChannelsBP() const { return Channels; }
	const TArray<FKIRCServerNameListResponseInfo>& GetChannels() const { return Channels; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	TArray<FKIRCServerNameListResponseInfo> Channels;

};
