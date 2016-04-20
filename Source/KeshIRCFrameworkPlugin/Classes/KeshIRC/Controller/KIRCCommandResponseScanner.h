// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCCommandResponseScanner.generated.h"

class UKIRCClient;

struct FKIRCCallback
{
	UObject* Object;
	FKIRCIncomingMessageHandlerDelegate Callback;
};

/**
* Handles the responses from the server for the commands sent.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Abstract, Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCCommandResponseScanner : public UObject
{
	GENERATED_BODY()

public:

	UKIRCCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	FKIRCCommandResponseCallback OnScanComplete;

	// Before this is run, the scanner will not check the server messages.
	virtual void StartScan( UKIRCClient* Server );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool IsWaiting() const { return State == EKIRCCommandState::S_Waiting; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool IsScanning() const { return State == EKIRCCommandState::S_Scanning; }

	// Returns true if 
	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool IsComplete() const { return State > EKIRCCommandState::S_Scanning; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	bool WasSuccessful() const { return State == EKIRCCommandState::S_Success; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	EKIRCCommandState GetState() const { return State; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetResult() const { return Result; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetCommand() const { return Command; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	void SetCommand( const FString& NewCommand ) { Command = NewCommand; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetTarget() const { return Target; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	void SetTarget( const FString& NewTarget ) { Target = NewTarget; }

	UFUNCTION()
	void CommandCallback( UKIRCUser* const Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintNativeEvent )
	void HandleCommand( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );
	virtual void HandleCommand_Implementation( UKIRCUser* Source, const FString& Command, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintNativeEvent )
	void HandleNumeric( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message );
	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintNativeEvent )
	void ScanComplete();
	virtual void ScanComplete_Implementation();

protected:

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleInstanceOnly, BlueprintReadOnly )
	FString Command;

	// Default list of commands to register for callbacks on. Registers the HandleCommand function.
	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", EditAnywhere, BlueprintReadOnly )
	TArray<FString> CommandCallbacks;

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleInstanceOnly, BlueprintReadOnly )
	UKIRCClient* Client;

	// The target of a scanner, e.g. the user that is being whoised or the channel having its topic set.
	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleInstanceOnly, BlueprintReadOnly )
	FString Target;	

private:

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleInstanceOnly )
	EKIRCCommandState State;

	UPROPERTY( Category = "KeshIRC|Controller|Command Response Scanner", VisibleInstanceOnly )
	FString Result;

	TMap<FString, FKIRCCallback> RegisteredCallbacks;

protected:

	virtual void RegisterCallbacks();

	bool RegisterCallback( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	virtual void RemoveRegisteredCallbacks();

	void UnregisterCallback( const FString& Command, UObject* CallbackObject, FKIRCIncomingMessageHandlerDelegate CallbackFunction );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual void Fail( const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual void Success();

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNoSuchNick( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNoSuchChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorUnAvailResource( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNotOnChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNeedMoreParams( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorChanOPrivsNeeded( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorTooManyChannels( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorTooManyTargets( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorChannelIsFull( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorInviteOnlyChan( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorBannedFromChan( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorBadChannelKey( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorUserOnChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorUserNotInChannel( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorErroneusNickname( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNickNameInUse( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNickCollision( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorWasNoSuchNick( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	virtual bool ErrorNoChanModes( UKIRCUser* Source, const TArray<FString>& Params, const FString& Message );

};
