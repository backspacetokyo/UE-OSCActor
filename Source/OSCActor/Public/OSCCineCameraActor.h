// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "OSCServer.h"
#include "OSCAddress.h"
#include "OSCMessage.h"
#include "OSCActorServer.h"
#include "OSCActorFunctionLibrary.h"
#include "OSCCineCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class OSCACTOR_API AOSCCineCameraActor : public ACineCameraActor
{
	GENERATED_BODY()

protected:

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
public:
	
	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<AOSCActorServer> OSCServer;

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void CopyCameraSettingToSceneCaptureComponent2D(USceneCaptureComponent2D* SceneCaptureComponent);

protected:

	FOSCDispatchMessageEventBP OnTransformMessageReceivedEvent;
	FOSCDispatchMessageEventBP OnCameraMessageReceivedEvent;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void OnTransformMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void OnCameraMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);

};
