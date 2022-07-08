// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OSCServer.h"
#include "OSCAddress.h"
#include "OSCMessage.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "OSCActorServer.h"
#include "OSCActorFunctionLibrary.h"
#include "OSCActor.generated.h"

USTRUCT()
struct FChannelData
{
	GENERATED_BODY()

	TArray<float> Samples;
};

UCLASS()
class OSCACTOR_API AOSCActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AOSCActor();

protected:

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:	

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<AOSCActorServer> OSCServer;

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	float GetParam(const FString& k, float DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	const TArray<float>& GetMultiSampleParam(const FString& k);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh, TArray<FString> InCustomDataChannels);

private:

	TMap<FString, float> Params;
	TMap<FString, FChannelData> MultiSampleParams;
	TMap<FString, FChannelData> MultiSampleParams_back;
	int MultiSampleNum = 0;

	FOSCDispatchMessageEventBP OnStateMessageReceivedEvent;
	FOSCDispatchMessageEventBP OnTransformMessageReceivedEvent;
	FOSCDispatchMessageEventBP OnParameterMessageReceivedEvent;
	FOSCDispatchMessageEventBP OnMultiSampleParameterMessageReceivedEvent;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void OnStateMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void OnTransformMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void OnParameterMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void OnMultiSampleParameterMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);
};
