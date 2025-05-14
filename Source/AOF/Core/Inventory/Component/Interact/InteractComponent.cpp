// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractComponent.h"

#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
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
			SphereCollision->InitSphereRadius(60.0f);
			SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
			
			SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
			
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
			InteractionWidget = NewObject<UWidgetComponent>(this);
			InteractionWidget->RegisterComponent();
			InteractionWidget->SetWidget(Widget);
			InteractionWidget->SetDrawAtDesiredSize(true);
			InteractionWidget->SetDrawSize(FVector2D(263, 500));
			InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
			InteractionWidget->SetCollisionResponseToAllChannels(ECR_Ignore);
			InteractionWidget->SetVisibility(false);

			if (USkeletalMeshComponent* SkeletalMeshComponent = Owner->FindComponentByClass<USkeletalMeshComponent>())
			{
				InteractionWidget->AttachToComponent(
					SkeletalMeshComponent,
					FAttachmentTransformRules::KeepRelativeTransform,
					FName("skt_Interact"));
			} else
			{
				InteractionWidget->AttachToComponent(
					GetOwner()->GetRootComponent(),
					FAttachmentTransformRules::KeepRelativeTransform);
			}
		}
	}
}

void UInteractComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Owner) return;
	if (Owner->GetOwner())
	{
		InteractionWidget->SetVisibility(false);
	} else if (OtherActor && OtherActor->Implements<UToPlayerInterface>() && InteractionWidget)
	{
		IToPlayerInterface::Execute_SetVisibilityUIWidget(OtherActor, InteractionWidget, true);
	}
}

void UInteractComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Owner) return;
	if (Owner->GetOwner())
	{
		InteractionWidget->SetVisibility(false);
	} else if (OtherActor && OtherActor->Implements<UToPlayerInterface>() && InteractionWidget)
	{
		IToPlayerInterface::Execute_SetVisibilityUIWidget(OtherActor, InteractionWidget, false);
	}
}