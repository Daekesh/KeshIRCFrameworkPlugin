// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCObject.generated.h"


/**
* Base class of everything on irc. Channels and users.
*/
UCLASS( Category = "KeshIRC|Model|Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCObject : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION( Category = "KeshIRC|Model|Object", BlueprintCallable )
	UKIRCServer* const GetServer() const;

	UFUNCTION( Category = "KeshIRC|Model|Object", BlueprintCallable )
	const FString& GetName() const { return Name; }

protected:

	friend class UKIRCServer;
	friend class UKIRCChannel;
	friend class UKIRCUser;

	UPROPERTY( Category = "KeshIRC|Model|Object", VisibleInstanceOnly )
	FString Name;

	UKIRCObject( const class FObjectInitializer& ObjectInitializer );

	// Called when first created to init the user
	virtual void InitObject( const FString& Name );

	virtual void SetName( const FString& Name ) { this->Name = Name; }

};
