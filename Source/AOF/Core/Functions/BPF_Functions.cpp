// Fill out your copyright notice in the Description page of Project Settings.


#include "BPF_Functions.h"

#include "Kismet/GameplayStatics.h"

void UBPF_Functions::GetLookDirection(float Length, UObject* WorldContextObject, FVector& StartDirection, FVector& EndDirection)
{
	if (!WorldContextObject) return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PlayerController) return;
	
	int32 ViewportX, ViewportY;
	PlayerController->GetViewportSize(ViewportX, ViewportY);


	FVector WorldPosition;
	FVector WorldDirection;

	if (PlayerController->DeprojectScreenPositionToWorld(ViewportX / 2.0f, ViewportY / 2.0f, WorldPosition, WorldDirection))
	{
		StartDirection = WorldPosition;
		EndDirection = WorldDirection * Length;
	}
}
