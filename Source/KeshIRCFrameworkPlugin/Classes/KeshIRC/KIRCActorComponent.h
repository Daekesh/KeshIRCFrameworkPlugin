// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KIRCActorComponent.generated.h"

class UKIRCClient;

/**
* Actor component that houses an irc connection.
*/
UCLASS( Category = "KeshIRC", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UKIRCActorComponent( const class FObjectInitializer& ObjectInitializer );

	// Cosmetic Only.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	FString ServerName;

	// IP or hostname. Required.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	FString Host;

	// Required.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	int32 Port;

	// Leave blank if not required
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	FString ServerPassword;

	// Primary nickname. Required.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	FString NickName;

	// Alternate nicknames if your primary one is not available.
	// If we're unable to register with your primary nick name, this list
	// will be used. If it runs out a random KIRC##### name will be used.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	TArray<FString> AlternateNickNames;

	// Required.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	FString Ident;

	// Not required. Helps identify you on the IRC server.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	FString RealName;

	// Required.
	UPROPERTY( Category = "KeshIRC | Actor Component", EditAnywhere )
	TSubclassOf<UKIRCClient> ClientClass;

	// Our IRC Client. Woo!
	UPROPERTY( Category = "KeshIRC | Actor Component", VisibleInstanceOnly, BlueprintReadOnly, Transient )
	UKIRCClient* Client;

	// Call this function to create the client. One that's done, use the Connect()
	// function to start your session. Disconnect() to end.
	UFUNCTION( Category = "KeshIRC | Actor Component", BlueprintCallable )
	virtual bool Initialize();

};
