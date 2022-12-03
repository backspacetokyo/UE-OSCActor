#include "OSCActorSubsystem.h"

#include "Editor.h"
#include "OSCActor.h"
#include "OSCActorModule.h"
#include "OSCCineCameraActor.h"
#include "OSCManager.h"

UOSCActorSettings::UOSCActorSettings(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UOSCActorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UOSCActorSettings* Settings = GetDefault <UOSCActorSettings>();

	OSCServer = NewObject<UOSCServer>(this, FName("OSCActorServer"));
	OSCServer->SetAddress(FString("0.0.0.0"), Settings->OSCReceivePort);
	OSCServer->SetTickInEditor(true);
	OSCServer->Listen();
	
	OSCServer->OnOscBundleReceived.AddDynamic(this, &UOSCActorSubsystem::OnOscBundleReceived);
}

void UOSCActorSubsystem::Deinitialize()
{
	OSCServer->OnOscBundleReceived.RemoveDynamic(this, &UOSCActorSubsystem::OnOscBundleReceived);

	OSCServer->Stop();
	OSCServer->ConditionalBeginDestroy();

	Super::Deinitialize();
}

void UOSCActorSubsystem::UpdateActorReference(AActor* Actor_)
{
	if (AOSCCineCameraActor* Camera = Cast<AOSCCineCameraActor>(Actor_))
	{
		OSCCameraMap.Add(Camera->ObjectName, Camera);
	}
	else if (AOSCActor* Actor = Cast<AOSCActor>(Actor_))
	{
		OSCActorMap.Add(Actor->ObjectName, Actor);
	}
}

void UOSCActorSubsystem::OnOscBundleReceived(const FOSCBundle& Bundle, const FString& IPAddress, int32 Port)
{
	static const FMatrix ROT_YAW_90 = FRotationMatrix::Make(FRotator(0, 90, 0));
	
	const UOSCActorSettings* Settings = GetDefault <UOSCActorSettings>();
	
	auto Messages = UOSCManager::GetMessagesFromBundle(Bundle);

	for (auto Iter : OSCActorMap)
	{
		auto O = Iter.Value;
		O->Params.Reset();
		O->MultiSampleParams.Reset();
	}
	
	for (auto Message : Messages)
	{
		auto Address = Message.GetAddress().GetFullPath();

		TArray<FString> Comp;
		if (Address.ParseIntoArray(Comp, TEXT("/"), true))
		{
			auto Name = Comp[1];
			
			if (Comp[0] == "obj")
			{
				auto It = OSCActorMap.Find(Name);
				if (!It)
					continue;

				AOSCActor* Actor = *It;
				
				auto Type = Comp[2];

				if (Type == "active")
				{
					bool Value = false;
					UOSCManager::GetBool(Message, 0, Value);

					Actor->SetActorHiddenInGame(!Value);
#if WITH_EDITOR
					Actor->SetIsTemporarilyHiddenInEditor(!Value);
#endif
				}
				else if (Type == "TRS")
				{
					TArray<float> OutValues;
					UOSCManager::GetAllFloats(Message, OutValues);

					const float* a = OutValues.GetData();
					FMatrix M = UOSCActorFunctionLibrary::TRSToMatrix(
						a[0], a[1], a[2],
						a[3], a[4], a[5],
						a[6], a[7], a[8]
					);

					M = UOSCActorFunctionLibrary::ConvertGLtoUE4Matrix(M);
					M = ROT_YAW_90 * M;
					
					Actor->SetActorTransform(FTransform(M));
				}
				else if (Type == "ss")
				{
					auto ParName = Comp[3];

					TArray<float> OutValues;
					UOSCManager::GetAllFloats(Message, OutValues);

					float v = OutValues.Last();
					
					Actor->Params.Add(ParName, v);
				}
				else if (Type == "ms")
				{
					auto ParName = Comp[3];

					FChannelData Data;
					UOSCManager::GetAllFloats(Message, Data.Samples);

					Actor->MultiSampleParams.Add(ParName, Data);
				}
			}
			else if (Comp[0] == "cam")
			{
				auto It = OSCCameraMap.Find(Name);
				if (!It)
					continue;

				AOSCCineCameraActor* Camera = *It;

				auto Type = Comp[2];
				
				if (Type == "active")
				{
					bool Value = false;
					UOSCManager::GetBool(Message, 0, Value);

					Camera->SetActorHiddenInGame(!Value);
#if WITH_EDITOR
					Camera->SetIsTemporarilyHiddenInEditor(!Value);
#endif
				}
				else if (Type == "TRS")
				{
					TArray<float> OutValues;
					UOSCManager::GetAllFloats(Message, OutValues);

					const float* a = OutValues.GetData();
					FMatrix M = UOSCActorFunctionLibrary::TRSToMatrix(
						a[0], a[1], a[2],
						a[3], a[4], a[5],
						a[6], a[7], a[8]
					);

					M = UOSCActorFunctionLibrary::ConvertGLtoUE4Matrix(M);

					Camera->SetActorTransform(FTransform(M));
				}
				else if (Type == "focal")
				{
					float Value;
					UOSCManager::GetFloat(Message, 0, Value);
					Camera->GetCineCameraComponent()->SetCurrentFocalLength(Value);
				}
				else if (Type == "aperture")
				{
					float Value;
					UOSCManager::GetFloat(Message, 0, Value);
					FCameraFilmbackSettings FilmbackSettings;
					FilmbackSettings.SensorWidth = Value;
					FilmbackSettings.SensorHeight = Value / Settings->SensorAspectRatio; 
					FilmbackSettings.SensorAspectRatio = Settings->SensorAspectRatio; 
					
					Camera->GetCineCameraComponent()->SetFilmback(FilmbackSettings);
				}
			}
			else if (Comp[0] == "sys")
			{
				auto Type = Comp[1];

				if (Type == "frame_number")
				{
					int Value;
					UOSCManager::GetInt32(Message, 0, Value);
					FrameNumber = Value;
				}
			}
		}
	}

	// Update MultiSampleNum to minimum amount of Samples
	for (auto Iter : OSCActorMap)
	{
		auto O = Iter.Value;
		int MultiSampleNum = 100000000;
		
		for (auto It : O->MultiSampleParams)
		{
			int n = It.Value.Samples.Num();
			if (n > 0)
				MultiSampleNum = std::min(n, MultiSampleNum); 
		}
		
		if (MultiSampleNum == 100000000)
			MultiSampleNum = 0;

		O->MultiSampleNum = MultiSampleNum;
		O->UpdateFromOSC();
	}
}
