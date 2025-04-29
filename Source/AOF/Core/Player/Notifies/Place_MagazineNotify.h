// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AOF/Core/Player/AnimInstance/APlayerAnimInstance.h"
#include "Place_MagazineNotify.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API UPlace_MagazineNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override
	{
		if (auto* AnimInstance = Cast<UAPlayerAnimInstance>(MeshComponent->GetAnimInstance()))
		{
			AnimInstance->OnNotifyTriggered(ENotifyType::Place_Magazine);
		}
	}
};
