// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KIRCActor.generated.h"

class UKIRCActorComponent;

/**
* Actor that holds a Kesh IRC Actor Component
*/
UCLASS( Category = "KeshIRC", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API AKIRCActor : public AActor
{
	GENERATED_BODY()

public:

	AKIRCActor( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Actor", BlueprintCallable )
	UKIRCActorComponent* GetKeshIRCComponent() const;

protected:

	UPROPERTY( Category = "KeshIRC|Actor", VisibleAnywhere, BlueprintReadOnly )
	UKIRCActorComponent* IRCComponent;

};
