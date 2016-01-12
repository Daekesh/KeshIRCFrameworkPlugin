// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KeshIRC/Controller/KIRCCommandResponseScanner.h"
#include "KIRCTopicQueryCommandResponseScanner.generated.h"

class UKIRCClient;


/**
* Handles responses to the TopicQuery command.
*/
UCLASS( ClassGroup = "KeshIRC|Controller", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCTopicQueryCommandResponseScanner : public UKIRCCommandResponseScanner
{
	GENERATED_BODY()

public:

	UKIRCTopicQueryCommandResponseScanner( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	bool HasTopic() const { return bHasTopic; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetTopicBody() const { return TopicBody; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetAuthorNickName() const { return AuthorNickName; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetAuthorIdent() const { return AuthorIdent; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	const FString& GetAuthorHost() const { return AuthorHost; }

	UFUNCTION( Category = "KeshIRC|Controller|Command Response Scanner", BlueprintCallable )
	FDateTime GetTopicDateBP() const { return TopicDate; }
	const FDateTime& GetTopicDate() const { return TopicDate; }

	virtual void HandleNumeric_Implementation( UKIRCUser* Source, int32 Numeric, const TArray<FString>& Params, const FString& Message ) override;

protected:

	bool bHasTopic;
	FString TopicBody;
	FDateTime TopicDate;
	FString AuthorNickName;
	FString AuthorIdent;
	FString AuthorHost;

};
