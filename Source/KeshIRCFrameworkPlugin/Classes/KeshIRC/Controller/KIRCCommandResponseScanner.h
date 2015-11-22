// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles the responses from the server for the commands sent.
*/
UCLASS( Category = "KeshIRC | Client", Abstract, Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCCommandResponseScanner : public UObject
{
	GENERATED_BODY()

public:

	UKIRCCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	FKIRCCommandResponseCallback OnScanComplete;

	virtual void StartScan( UKIRCClient* Server );

	UFUNCTION( Category = "KeshIRC | Client | Command Response Scanner", BlueprintCallable )
	virtual bool IsComplete() const { return bComplete;  }

	UFUNCTION()
	void CommandCallback( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC | Client | Command Response Scanner", BlueprintNativeEvent )
	void HandleCommand( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	virtual void HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC | Client | Command Response Scanner", BlueprintNativeEvent )
	void HandleNumeric( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message );
	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC | Client | Command Response Scanner", BlueprintNativeEvent )
	void ScanComplete();
	virtual void ScanComplete_Implementation();

protected:

	UPROPERTY( Category = "KeshIRC | Client | Command Response Scanner", EditInstanceOnly, BlueprintReadWrite )
	bool bComplete;

	// Default list of commands to register for callbacks on. Registers the HandleCommand function.
	UPROPERTY( Category = "KeshIRC | Client | Command Response Scanner", EditAnywhere, BlueprintReadOnly )
	TArray<FString> CommandCallbacks;

	UPROPERTY( Category = "KeshIRC | Client | Command Response Scanner", VisibleInstanceOnly, BlueprintReadOnly )
	UKIRCClient* Client;

	TMap<FString, FDelegateHandle> RegisteredCallbacks;

	virtual void RegisterCallbacks();

	FDelegateHandle RegisterCallback( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveRegisteredCallbacks();

	void UnregisterCallback( const FString& Command, FDelegateHandle Handle );

};
