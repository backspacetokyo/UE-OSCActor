#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OSCServer.h"
#include "OSCActorServer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(AOSCActorServerStartedInternal, UOSCServer*)

UCLASS()
class OSCACTOR_API AOSCActorServer : public AActor
{
	GENERATED_BODY()
	
public:	

	AOSCActorServer();

protected:

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	int Port = 7000;

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	TArray<FString> ControllerIPs = { FString("127.0.0.1") };

	UPROPERTY(Category = "OSCActor", EditAnywhere, BlueprintReadWrite)
	int ControllerPort = 7001;

	UPROPERTY()
	class UOSCServer* OSCServer;
};
