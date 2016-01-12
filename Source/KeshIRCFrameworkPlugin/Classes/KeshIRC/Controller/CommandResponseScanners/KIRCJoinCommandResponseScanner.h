// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCJoinCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles responses to the Join command. Be careful. This command may block up
* the works if you use it to join a channel you're already in.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCJoinCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCJoinCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	virtual void HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message ) override;

};
