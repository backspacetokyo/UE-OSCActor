// Fill out your copyright notice in the Description page of Project Settings.


#include "OSCActorFunctionLibrary.h"

static const FMatrix GL_TO_UE4(FPlane(0, 0, -1, 0), FPlane(1, 0, 0, 0), FPlane(0, 1, 0, 0), FPlane(0, 0, 0, 1));
static const FMatrix GL_TO_UE4_T = GL_TO_UE4.GetTransposed();

bool UOSCActorFunctionLibrary::FloatArrayToMatrix(const TArray<float>& InArray, FMatrix& OutMatrix)
{
	if (InArray.Num() != 16)
		return false;

	FMatrix M;

	const float* Src = InArray.GetData();

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			M.M[x][y] = *Src++;
		}
	}

	OutMatrix = M;

	return true;
}

FMatrix UOSCActorFunctionLibrary::TRSToMatrix(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz)
{
	FMatrix T = FMatrix::Identity;

	T *= FScaleMatrix::Make(FVector(sx, sy, sz));
	T *= FRotationMatrix::Make(FRotator(-ry, rz, -rx));
	T *= FTranslationMatrix::Make(FVector(tx, ty, tz));

	return T;
}

FMatrix UOSCActorFunctionLibrary::ConvertGLtoUE4Matrix(const FMatrix& InMatrix)
{
	FMatrix M = InMatrix;
	M.SetOrigin(M.GetOrigin() * 100);
	return GL_TO_UE4 * M * GL_TO_UE4_T;
}
