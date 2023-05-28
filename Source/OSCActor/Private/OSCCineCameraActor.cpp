// Fill out your copyright notice in the Description page of Project Settings.


#include "OSCCineCameraActor.h"

#include "OSCActorSubsystem.h"

void UOSCCineCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	DesiredView.OffCenterProjectionOffset.X = WindowXY.X;
	DesiredView.OffCenterProjectionOffset.Y = WindowXY.Y;
}

// ===================================================================================

AOSCCineCameraActor::AOSCCineCameraActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOSCCineCameraComponent>(TEXT("CameraComponent")))
{
	OSCCineCameraComponent = Cast<UOSCCineCameraComponent>(GetCineCameraComponent());
}

void AOSCCineCameraActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UOSCActorSubsystem* S = GEngine->GetEngineSubsystem<UOSCActorSubsystem>();
	S->UpdateActorReference(this);
}

void AOSCCineCameraActor::CopyCameraSettingToSceneCaptureComponent2D(USceneCaptureComponent2D* SceneCaptureComponent)
{
	UCameraComponent* Src = GetCameraComponent();
	USceneCaptureComponent2D* Dst = SceneCaptureComponent;

	if (Src && Dst)
	{
		Dst->SetWorldLocationAndRotation(Src->GetComponentLocation(), Src->GetComponentRotation());
		Dst->FOVAngle = Src->FieldOfView;

		FMinimalViewInfo CameraViewInfo;
		Src->GetCameraView(/*DeltaTime =*/0.0f, CameraViewInfo);

		const FPostProcessSettings& SrcPPSettings = CameraViewInfo.PostProcessSettings;
		FPostProcessSettings& DstPPSettings = Dst->PostProcessSettings;
		
		FWeightedBlendables DstWeightedBlendables = DstPPSettings.WeightedBlendables;

		// Copy all of the post processing settings
		DstPPSettings = SrcPPSettings;

		// Copy all of the post process materials
		DstPPSettings.WeightedBlendables = SrcPPSettings.WeightedBlendables;
	}
}
