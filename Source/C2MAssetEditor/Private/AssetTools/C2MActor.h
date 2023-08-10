#pragma once
class UHierarchicalInstancedStaticMeshComponent;
/* C2M Actor functions
 * Represents all actor related functions for C2M
 */
class C2MActor
{
public:
	static UHierarchicalInstancedStaticMeshComponent* PlaceInstancedStaticMesh(UStaticMesh* InStaticMesh, TArray<FTransform> ModelPlacement, UWorld* currentWorld, bool bRegisterWithWorld);
	static AStaticMeshActor* PlaceStaticMesh(UStaticMesh* InStaticMesh, FTransform ModelPlacement, UWorld* currentWorld, FString ActorName, bool bRegisterWithWorld);
	static AActor* PlaceEmptyActor(FTransform ModelPlacement, UWorld* currentWorld, FString ActorName, bool bRegisterWithWorld);
};
