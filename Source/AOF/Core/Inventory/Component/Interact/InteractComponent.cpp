// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractComponent.h"

#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Owner = GetOwner();
	if (Owner && Owner->GetRootComponent())
	{
		SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
		if (SphereCollision)
		{
			SphereCollision->InitSphereRadius(30.0f);
			SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &UInteractComponent::OnBeginOverlap);
			SphereCollision->OnComponentEndOverlap.AddDynamic(this, &UInteractComponent::OnEndOverlap);
			SphereCollision->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			
		}
	}
}


void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Owner = GetOwner();

	if (Owner && Owner->GetRootComponent())
	{
		UUserWidget* Widget = CreateWidget(GetWorld(), WidgetReference);
		if (Widget)
		{
			InteractionWidget = NewObject<UWidgetComponent>(Owner->GetRootComponent());
			InteractionWidget->RegisterComponent();
			InteractionWidget->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			InteractionWidget->SetWidget(Widget);
			InteractionWidget->SetDrawAtDesiredSize(true);
			InteractionWidget->SetDrawSize(FVector2D(263, 500));
			InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
			InteractionWidget->SetVisibility(false);
		}
	}
}

void UInteractComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Owner->GetOwner())
	{
		InteractionWidget->SetVisibility(false);
	} else if (OtherActor && OtherActor->Implements<UToPlayerInterface>() && InteractionWidget)
	{
		IToPlayerInterface::Execute_SetVisibilityButtonInteract(OtherActor, InteractionWidget, true);
	}
}

void UInteractComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Owner->GetOwner())
	{
		InteractionWidget->SetVisibility(false);
	} else if (OtherActor && OtherActor->Implements<UToPlayerInterface>() && InteractionWidget)
	{
		IToPlayerInterface::Execute_SetVisibilityButtonInteract(OtherActor, InteractionWidget, false);
	}
}