// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AOF/Core/Player/AnimInstance/APlayerAnimInstance.h"
#include "Remove_MagazineNotify.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API URemove_MagazineNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override
	{
		if (auto* AnimInstance = Cast<UAPlayerAnimInstance>(MeshComponent->GetAnimInstance()))
		{
			AnimInstance->OnNotifyTriggered(ENotifyType::Remove_Magazine);
		}
	}
};
