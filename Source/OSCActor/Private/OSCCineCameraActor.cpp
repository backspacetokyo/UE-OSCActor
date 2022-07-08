// Fill out your copyright notice in the Description page of Project Settings.


#include "OSCCineCameraActor.h"

#include "OSCManager.h"

void AOSCCineCameraActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AOSCCineCameraActor::BeginPlay()
{
	Super::BeginPlay();

	OnTransformMessageReceivedEvent.Clear();
	OnCameraMessageReceivedEvent.Clear();

	AOSCActorServer* _OSCServer = OSCServer.LoadSynchronous();
	if (_OSCServer)
	{
		{
			OnTransformMessageReceivedEvent.BindUFunction(this, FName("OnTransformMessageReceived"));

			auto addr = FOSCAddress(FString::Format(TEXT("/tr/{0}"), { *ObjectName }));
			_OSCServer->OSCServer->BindEventToOnOSCAddressPatternMatchesPath(addr, OnTransformMessageReceivedEvent);
		}

		{
			OnCameraMessageReceivedEvent.BindUFunction(this, FName("OnCameraMessageReceived"));

			auto addr = FOSCAddress(FString::Format(TEXT("/cam/{0}"), { *ObjectName }));
			_OSCServer->OSCServer->BindEventToOnOSCAddressPatternMatchesPath(addr, OnCameraMessageReceivedEvent);
		}
	}
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

void AOSCCineCameraActor::OnTransformMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port)
{
	if (!Message.GetPacket().IsValid())
		return;

	TArray<float> OutValues;
	UOSCManager::GetAllFloats(Message, OutValues);

	const float* a = OutValues.GetData();
	FMatrix M = UOSCActorFunctionLibrary::TRSToMatrix(
		a[0], a[1], a[2],
		a[3], a[4], a[5],
		a[6], a[7], a[8]
	);

	M = UOSCActorFunctionLibrary::ConvertGLtoUE4Matrix(M);

	SetActorTransform(FTransform(M));
}

void AOSCCineCameraActor::OnCameraMessageReceived(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port)
{
	auto Method = Message.GetAddress().GetMethod();

	if (Method == "fov")
	{
		float Value = 0;
		if (UOSCManager::GetFloat(Message, 0, Value))
		{
			GetCineCameraComponent()->SetFieldOfView(Value);
		}
	}
}
