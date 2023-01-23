// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grid.h"
#include "Pathfinder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRIDGENERATORWITHASTARPATHFINDER_API UPathfinder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathfinder();
public:	
	// Find shortest path between 2 given locations
	void FindPath(FVector StartPos, FVector TargetPos);
	// Find shortest path between 2 gived GridNodes
	void FindPathNode(AGridNode* StartNode, AGridNode* TargetNode);
	// Get the distance between nodes on the Grid
	int32 GetDistanceBetweenNodes(const AGridNode* StartNode, const AGridNode* EndNode);
	// Return TArray of GridNodes containing the path from Start Node to Target Node
	TArray<AGridNode*> RetracePath(const AGridNode* StartNode, AGridNode* EndNode);
	// Reset the color and walkable state of the last calculated path
	void ResetLastPath();

public:
	// Pointer to Grid class this pathfinder class uses to draw the path
	UPROPERTY(EditAnywhere, Category = "Grid Reference")
		AGrid* Grid;

private:
	TArray<AGridNode*> CurrentPath;			 // TArray of GridNodes containing the path from Start Node to Target Node for the current calculations
};
