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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateFromOSCDelegate);

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class OSCACTOR_API UOSCActorComponent : public UActorComponent
{
	friend class UOSCActorSubsystem;
	
	GENERATED_BODY()
	
public:
	
	UOSCActorComponent();
	
	virtual void BeginDestroy() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	virtual bool IsComponentTickEnabled() const override { return true; }
	
	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	float GetOSCParam(const FString& Key, float DefaultValue = 0);
	
	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	const TArray<float>& GetOSCMultiSampleParam(const FString& Key);
	
	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh, TArray<FString> InCustomDataChannels);

	UPROPERTY(BlueprintAssignable, DisplayName="Update From OSC", Category = "OSCActor")
	FUpdateFromOSCDelegate UpdateFromOSC;
	
private:

	TMap<FString, float> Params;
	TMap<FString, FChannelData> MultiSampleParams;
	int MultiSampleNum = 0;
};

// ===================================================================================

UCLASS()
class OSCACTOR_API AOSCActor : public AActor
{
	friend class UOSCActorSubsystem;
	
	GENERATED_BODY()
	
public:	

	AOSCActor(const FObjectInitializer& ObjectInitializer);

protected:
	
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	// Deprecated: Should be use UOSCActorComponent::UpdateFromOSC
	// UFUNCTION(CallInEditor, BlueprintCallable, BlueprintImplementableEvent, Category = "OSCActor")
	// void UpdateFromOSC();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "OSCActor")
	class UOSCActorComponent* OSCActorComponent;
	
	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	float GetOSCParam(const FString& Key, float DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	const TArray<float>& GetOSCMultiSampleParam(const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	void UpdateInstancedStaticMesh(UInstancedStaticMeshComponent* InstancedStaticMesh, TArray<FString> InCustomDataChannels);
};
