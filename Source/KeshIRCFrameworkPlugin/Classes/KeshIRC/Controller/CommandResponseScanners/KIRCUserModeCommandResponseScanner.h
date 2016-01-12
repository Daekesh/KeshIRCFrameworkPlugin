// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCUserModeCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles responses to the UserMode command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCUserModeCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCUserModeCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const TArray<UKIRCMode*> GetModesBP() const
	{
		TArray<UKIRCMode*> ModeArray;
		
		for ( const UKIRCMode* const Mode : Modes )
			ModeArray.Add( const_cast< UKIRCMode* >( Mode ) );

		return ModeArray;
	}
	const TArray<const UKIRCMode*>& GetModes() const { return Modes; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	TArray<const UKIRCMode*> Modes;

};
