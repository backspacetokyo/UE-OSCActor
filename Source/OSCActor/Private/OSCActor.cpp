#include "OSCActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "OSCActorSubsystem.h"

AOSCActor::AOSCActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOSCActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UOSCActorSubsystem* S = GEngine->GetEngineSubsystem<UOSCActorSubsystem>();
	S->UpdateActorReference(this);
}

float AOSCActor::GetParam(const FString& k, float DefaultValue)
{
	if (!Params.Contains(k))
		return DefaultValue;

	return Params[k];
}

static float getSample(const TArray<float>& c, int index, float default_value = 0)
{
	if (c.Num() == 0) return default_value;
	return c[index];
}

void AOSCActor::UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh, TArray<FString> InCustomDataChannels)
{
	TArray<FInstancedStaticMeshInstanceData> InstanceData;

	const TArray<float>& tx = GetMultiSampleParam("tx");
	const TArray<float>& ty = GetMultiSampleParam("ty");
	const TArray<float>& tz = GetMultiSampleParam("tz");

	const TArray<float>& rx = GetMultiSampleParam("rx");
	const TArray<float>& ry = GetMultiSampleParam("ry");
	const TArray<float>& rz = GetMultiSampleParam("rz");

	const TArray<float>& sx = GetMultiSampleParam("sx");
	const TArray<float>& sy = GetMultiSampleParam("sy");
	const TArray<float>& sz = GetMultiSampleParam("sz");

	const TArray<float>& vx = GetMultiSampleParam("vx");
	const TArray<float>& vy = GetMultiSampleParam("vy");
	const TArray<float>& vz = GetMultiSampleParam("vz");

	const TArray<float>& ltx = GetMultiSampleParam("ltx");
	const TArray<float>& lty = GetMultiSampleParam("lty");
	const TArray<float>& ltz = GetMultiSampleParam("ltz");

	const TArray<float>& lrx = GetMultiSampleParam("lrx");
	const TArray<float>& lry = GetMultiSampleParam("lry");
	const TArray<float>& lrz = GetMultiSampleParam("lrz");

	const TArray<float>& lsx = GetMultiSampleParam("lsx");
	const TArray<float>& lsy = GetMultiSampleParam("lsy");
	const TArray<float>& lsz = GetMultiSampleParam("lsz");

	TArray<TArray<float>> SrcCustomDataChannels;
	for (int i = 0; i < InCustomDataChannels.Num(); i++)
	{
		const TArray<float>& a = GetMultiSampleParam(InCustomDataChannels[i]);
		
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
			FQuat RR = FQuat::FindBetween(FVector(0, 0, 1), FVector(
				getSample(vx, i, 0),
				getSample(vy, i, 0),
				getSample(vz, i, 0))
			);
			T *= FRotationMatrix::Make(RR);
		}
		
		T *= FRotationMatrix::Make(FRotator(
			-getSample(ry, i),
			getSample(rz, i),
			-getSample(rx, i)
		));

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

const TArray<float>& AOSCActor::GetMultiSampleParam(const FString& k)
{
	auto Iter = MultiSampleParams.Find(k);
	if (!Iter)
	{
		static const TArray<float> a;
		return a;
	}

	const auto& s = *Iter;
	return s.Samples;
}
