// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCInviteCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles responses to the Invite command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCInviteCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCInviteCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

};
