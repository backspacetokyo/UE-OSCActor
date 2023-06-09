#include "OSCActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "OSCActorSubsystem.h"

static float getSample(const TArray<float>& c, int index, float default_value = 0)
{
	if (c.Num() == 0) return default_value;
	return c[index];
}

// ===================================================================================

UOSCActorComponent::UOSCActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
	bTickInEditor = true;
}

void UOSCActorComponent::BeginDestroy()
{
	UOSCActorSubsystem* S = GEngine->GetEngineSubsystem<UOSCActorSubsystem>();
	if (S)
		S->RemoveActorReference(this);

	Super::BeginDestroy();
}

void UOSCActorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UOSCActorSubsystem* S = GEngine->GetEngineSubsystem<UOSCActorSubsystem>();
	if (S)
		S->UpdateActorReference(this);
}

float UOSCActorComponent::GetOSCParam(const FString& Key, float DefaultValue)
{
	if (!Params.Contains(Key))
		return DefaultValue;

	return Params[Key];
}

const TArray<float>& UOSCActorComponent::GetOSCMultiSampleParam(const FString& Key)
{
	auto Iter = MultiSampleParams.Find(Key);
	if (!Iter)
	{
		static const TArray<float> a;
		return a;
	}

	const auto& s = *Iter;
	return s.Samples;
}

void UOSCActorComponent::UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh,
	TArray<FString> InCustomDataChannels)
{
TArray<FInstancedStaticMeshInstanceData> InstanceData;

	const TArray<float>& tx = GetOSCMultiSampleParam("tx");
	const TArray<float>& ty = GetOSCMultiSampleParam("ty");
	const TArray<float>& tz = GetOSCMultiSampleParam("tz");

	const TArray<float>& rx = GetOSCMultiSampleParam("rx");
	const TArray<float>& ry = GetOSCMultiSampleParam("ry");
	const TArray<float>& rz = GetOSCMultiSampleParam("rz");

	const TArray<float>& sx = GetOSCMultiSampleParam("sx");
	const TArray<float>& sy = GetOSCMultiSampleParam("sy");
	const TArray<float>& sz = GetOSCMultiSampleParam("sz");

	const TArray<float>& vx = GetOSCMultiSampleParam("vx");
	const TArray<float>& vy = GetOSCMultiSampleParam("vy");
	const TArray<float>& vz = GetOSCMultiSampleParam("vz");

	const TArray<float>& ltx = GetOSCMultiSampleParam("ltx");
	const TArray<float>& lty = GetOSCMultiSampleParam("lty");
	const TArray<float>& ltz = GetOSCMultiSampleParam("ltz");

	const TArray<float>& lrx = GetOSCMultiSampleParam("lrx");
	const TArray<float>& lry = GetOSCMultiSampleParam("lry");
	const TArray<float>& lrz = GetOSCMultiSampleParam("lrz");

	const TArray<float>& lsx = GetOSCMultiSampleParam("lsx");
	const TArray<float>& lsy = GetOSCMultiSampleParam("lsy");
	const TArray<float>& lsz = GetOSCMultiSampleParam("lsz");

	TArray<TArray<float>> SrcCustomDataChannels;
	for (int i = 0; i < InCustomDataChannels.Num(); i++)
	{
		const TArray<float>& a = GetOSCMultiSampleParam(InCustomDataChannels[i]);
		
		if (a.Num() != MultiSampleNum)
		{
			// UE_LOG(LogTemp, Log, TEXT("Invalid Channel Name: %s"), *InCustomDataChannels[i]);
			continue;
		}

		SrcCustomDataChannels.Add(a);
	}

	const bool hasDirection = (vx.Num() > 0) && (vy.Num() > 0) && (vz.Num() > 0);
	const bool hasLocalTranslation = (ltx.Num() > 0) || (lty.Num() > 0) || (ltz.Num() > 0);
	const bool hasLocalRotation = (lrx.Num() > 0) || (lry.Num() > 0) || (lrz.Num() > 0);
	const bool hasLocalScale = (lsx.Num() > 0) || (lsy.Num() > 0) || (lsz.Num() > 0);

	InstanceData.SetNum(MultiSampleNum);

	InstancedStaticMesh->SetSimulatePhysics(false);
	InstancedStaticMesh->DestroyPhysicsState();
	
	if (InstancedStaticMesh->GetInstanceCount() != MultiSampleNum)
	{
		while (InstancedStaticMesh->GetInstanceCount() < MultiSampleNum) {
			FTransform T;
			InstancedStaticMesh->AddInstance(T);
		}

		while (InstancedStaticMesh->GetInstanceCount() > MultiSampleNum)
		{
			InstancedStaticMesh->RemoveInstance(0);
		}
	}

	InstancedStaticMesh->NumCustomDataFloats = InCustomDataChannels.Num();
	InstancedStaticMesh->PerInstanceSMCustomData.SetNum(InstancedStaticMesh->NumCustomDataFloats * MultiSampleNum);

	float* CustomData = InstancedStaticMesh->PerInstanceSMCustomData.GetData();

	for (int i = 0; i < MultiSampleNum; i++)
	{
		FMatrix T = FMatrix::Identity;

		if (hasLocalScale)
		{
			T *= FScaleMatrix::Make(FVector(
				getSample(lsx, i, 1), 
				getSample(lsy, i, 1), 
				getSample(lsz, i, 1)
			));
		}
		
		if (hasLocalTranslation)
		{
			T *= FTranslationMatrix::Make(FVector(
				getSample(ltx, i),
				getSample(lty, i),
				getSample(ltz, i)
			));
		}

		if (hasLocalRotation)
		{
			T *= FRotationMatrix::Make(FRotator(
				-getSample(lry, i),
				getSample(lrz, i),
				-getSample(lrx, i)
			));
		}

		T *= FScaleMatrix::Make(FVector(
			getSample(sx, i, 1), 
			getSample(sy, i, 1), 
			getSample(sz, i, 1)
		));

		if (hasDirection)
		{
			FQuat RR = FQuat::FindBetweenNormals(
				FVector(0, 0, -1),
				FVector(
					getSample(vx, i, 0),
					getSample(vy, i, 0),
					getSample(vz, i, 0)
				).GetUnsafeNormal()
			);
			T *= FRotationMatrix::Make(RR);
		}
		else
		{
			T *= FRotationMatrix::Make(FRotator(
				-getSample(ry, i),
				getSample(rz, i),
				-getSample(rx, i)
			));
		}
		
		T *= FTranslationMatrix::Make(FVector(
			getSample(tx, i),
			getSample(ty, i),
			getSample(tz, i)
		));

		static const FMatrix ROT_YAW_90 = FRotationMatrix::Make(FRotator(0, -90, 0));
		static const FMatrix ROT_YAW_90_T = FRotationMatrix::Make(FRotator(0, 90, 0));
		
		InstanceData[i].Transform = ROT_YAW_90_T * UOSCActorFunctionLibrary::ConvertGLtoUE4Matrix(T) * ROT_YAW_90;

		for (int n = 0; n < InstancedStaticMesh->NumCustomDataFloats; n++)
		{
			*CustomData = SrcCustomDataChannels[n][i];
			CustomData++;
		}
	}

	InstancedStaticMesh->BatchUpdateInstancesData(0, MultiSampleNum, InstanceData.GetData(), true);
}

// ===================================================================================

AOSCActor::AOSCActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OSCActorComponent(CreateDefaultSubobject<UOSCActorComponent>(TEXT("OSCActorComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
}

float AOSCActor::GetOSCParam(const FString& Key, float DefaultValue)
{
	return OSCActorComponent->GetOSCParam(Key, DefaultValue);
}

const TArray<float>& AOSCActor::GetOSCMultiSampleParam(const FString& Key)
{
	return OSCActorComponent->GetOSCMultiSampleParam(Key);
}

void AOSCActor::UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh, TArray<FString> InCustomDataChannels)
{
	OSCActorComponent->UpdateInstancedStaticMesh(InstancedStaticMesh, InCustomDataChannels);
}
