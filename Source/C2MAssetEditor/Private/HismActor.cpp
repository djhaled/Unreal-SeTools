// Fill out your copyright notice in the Description page of Project Settings.


#include "HismActor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
// Sets default values
AHismActor::AHismActor()
{
	HismComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent0"));
	HismComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	HismComponent->Mobility = EComponentMobility::Static;
	HismComponent->SetGenerateOverlapEvents(false);
	HismComponent->bUseDefaultCollision = true;

}

// Called when the game starts or when spawned
void AHismActor::BeginPlay()
{
	Super::BeginPlay();
	
}

