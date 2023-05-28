// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "OSCServer.h"
#include "OSCAddress.h"
#include "OSCMessage.h"
#include "OSCActorFunctionLibrary.h"
#include "OSCCineCameraActor.generated.h"


UCLASS()
class OSCACTOR_API UOSCCineCameraComponent : public UCineCameraComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCActor")
	FVector2f WindowXY;

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
};

UCLASS()
class OSCACTOR_API AOSCCineCameraActor : public ACineCameraActor
{
	GENERATED_BODY()

protected:

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
	virtual void Tick(float DeltaSeconds) override;
	
public:

	AOSCCineCameraActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSCActor")
	FString ObjectName;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void CopyCameraSettingToSceneCaptureComponent2D(USceneCaptureComponent2D* SceneCaptureComponent);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	UOSCCineCameraComponent* GetOSCCineCameraComponent() const { return OSCCineCameraComponent; }

private:

	class UOSCCineCameraComponent* OSCCineCameraComponent;
};
