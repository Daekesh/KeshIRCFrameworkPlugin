// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCBlueprintMessageHandler.generated.h"

class UKIRCClient;


/**
* Handles the responses from the server for any messages that the client receives.
* Must set the Command string in the class defaults. 3 numbers for a numeric.
* 001, 501, 163, etc.
*/
UCLASS( Category = "KeshIRC|Controller", Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCBlueprintMessageHandler : public UObject
{
	GENERATED_BODY()

public:

	UKIRCBlueprintMessageHandler( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetCommand() const { return Command; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	void RegisterHandler( UKIRCClient* Client );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	void UnregisterHandler();

	UFUNCTION()
	void CommandCallback( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintImplementableEvent )
	void HandleCommand( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintImplementableEvent )
	void HandleNumeric( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message );

protected:

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleInstanceOnly, BlueprintReadOnly )
	UKIRCClient* Client;

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", EditDefaultsOnly, BlueprintReadOnly )
	FString Command;

};
