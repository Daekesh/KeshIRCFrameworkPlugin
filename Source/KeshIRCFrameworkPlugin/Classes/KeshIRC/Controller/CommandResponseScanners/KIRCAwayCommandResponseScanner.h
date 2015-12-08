// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCAwayCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles responses to the Away command.
*/
UCLASS( Category = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCAwayCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCAwayCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

};
