// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "Interface/ToGameInstanceInterface.h"
#include "AGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API UAGameInstance : public UGameInstance, public IToGameInstanceInterface
{
	GENERATED_BODY()

public:
	UAGameInstance();

protected:
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);
	virtual void OnFindSessionsComplete(bool Succeeded);

	virtual void CreateServer_Implementation() override;
	virtual void JoinServer_Implementation() override;

};