  #include "C2MActor.h"
#include "HismActor.h"
#include "Engine/StaticMeshActor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
UHierarchicalInstancedStaticMeshComponent* C2MActor::PlaceInstancedStaticMesh(UStaticMesh* InStaticMesh, TArray<FTransform> ModelPlacement, UWorld* currentWorld,  bool bRegisterWithWorld)
{
	AHismActor* MyNewActor = currentWorld->SpawnActor<AHismActor>(AHismActor::StaticClass());
	auto CompActor = MyNewActor->GetStaticMeshComponent();
	// Set the static mesh for the component
	CompActor->AddInstances(ModelPlacement, false);
	CompActor->SetStaticMesh(InStaticMesh);
	MyNewActor->SetActorLabel(InStaticMesh->GetName());
	return CompActor;
}
AStaticMeshActor* C2MActor::PlaceStaticMesh(UStaticMesh* InStaticMesh, FTransform ModelPlacement, UWorld* currentWorld, FString ActorName, bool bRegisterWithWorld)
{
	//if ((ModelPlacement.GetLocation().X == 0 && ModelPlacement.GetLocation().Y == 0 && ModelPlacement.GetLocation().Z == 0))
	//{
		//return nullptr;
	//}
	AStaticMeshActor* MyNewActor = currentWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	MyNewActor->SetMobility(EComponentMobility::Movable);
	MyNewActor->SetActorTransform(ModelPlacement);
	MyNewActor->SetActorLabel(*ActorName);
	UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(InStaticMesh);
		// if (bRegisterWithWorld)
		// 	MeshComponent->RegisterComponentWithWorld(currentWorld);
	}

	return MyNewActor;
}

AActor* C2MActor::PlaceEmptyActor(FTransform ModelPlacement, UWorld* currentWorld, FString ActorName, bool bRegisterWithWorld)
{
	AActor* MyNewActor = currentWorld->SpawnActor<AActor>(AActor::StaticClass());
	//MyNewActor->SetMobility(EComponentMobility::Movable);
	MyNewActor->SetActorTransform(ModelPlacement);
	MyNewActor->SetActorLabel(*ActorName);

	return MyNewActor;
}