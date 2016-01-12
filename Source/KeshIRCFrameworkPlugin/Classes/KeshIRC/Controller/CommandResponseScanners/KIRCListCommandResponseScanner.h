// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCListCommandResponseScanner.generated.h"

class UKIRCClient;

USTRUCT(BlueprintType)
struct FKIRCListResponseInfo
{
	GENERATED_BODY()
	
	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString Channel;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	int32 VisibleUsers;

	UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
	FString Topic;
};

/**
* Handles responses to the List command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCListCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCListCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	TArray<FKIRCListResponseInfo> GetChannelsBP() const { return Channels; }
	const TArray<FKIRCListResponseInfo>& GetChannels() const { return Channels; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	TArray<FKIRCListResponseInfo> Channels;

};
