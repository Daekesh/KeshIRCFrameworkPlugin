// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCPartCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles responses to the Part command.
*/
UCLASS( Category = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCPartCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCPartCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	virtual void HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message ) override;

};
