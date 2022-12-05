// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OSCActor.generated.h"

class UInstancedStaticMeshComponent;

USTRUCT()
struct FChannelData
{
	GENERATED_BODY()

	TArray<float> Samples;
};

UCLASS()
class OSCACTOR_API AOSCActor : public AActor
{
	friend class UOSCActorSubsystem;
	
	GENERATED_BODY()
	
public:	

	AOSCActor();

protected:

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent, CallInEditor, Category = "OSCActor")
	void UpdateFromOSC();

public:	

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	float GetOSCParam(const FString& Key, float DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	const TArray<float>& GetOSCMultiSampleParam(const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh, TArray<FString> InCustomDataChannels);

private:

	TMap<FString, float> Params;
	TMap<FString, FChannelData> MultiSampleParams;
	int MultiSampleNum = 0;
};
