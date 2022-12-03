// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OSCActorFunctionLibrary.generated.h"

UCLASS()
class OSCACTOR_API UOSCActorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	static bool FloatArrayToMatrix(const TArray<float>& InArray, FMatrix& OutMatrix);

	UFUNCTION(BlueprintCallable, Category = "OSCActor")
	static FMatrix TRSToMatrix(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz);

	UFUNCTION(BlueprintCallable, Category = "OSCActor", meta = (DisplayName = "Convert GL to UE4 Matrix"))
	static FMatrix ConvertGLtoUE4Matrix(const FMatrix& InMatrix);
};
