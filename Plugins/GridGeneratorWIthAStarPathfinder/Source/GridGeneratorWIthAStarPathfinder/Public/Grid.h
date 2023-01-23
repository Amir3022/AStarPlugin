// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "ProceduralMeshComponent.h"
#include "Grid.generated.h"

UCLASS()
class GRIDGENERATORWITHASTARPATHFINDER_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Function called after construction and before begin play to use variable set in the editor to change different aspects of this class behavior
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

public:
	// Spawn GridNodes in the Grid formation and make them check for obstacles and set them as walkable or not
	void CreateGrid();
	// Get pointer to GridNode on the Grid from input location
	AGridNode* NodeFromLocation(FVector WorldLocation);
	// Get all neighbor nodes if exist to the input node, used in pathfinding
	TArray<AGridNode*> GetNeighborNodes(AGridNode* Node);
	// Get Grid Size in actual world units
	FVector2D GetGridWorldSize();

	//Create 2D Grid Mesh 
	void CreateGridMesh();
	void CreateLine(FVector StartLocation, FVector EndLocation, float LineThickness, TArray<FVector>& Vertices, TArray<int32>& Triangles);

// Public variables can all be set from editor used to determine the Grid Size, nodes size, Grid mesh color, opacity and lines thickness, and whether nodes are visible or not
public:	
	UPROPERTY(EditAnywhere, Category = "Grid Components")
		int32 GridSizeX = 10;								// Number of nodes in the X direction of the Grid

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		int32 GridSizeY = 10;								// Number of nodes in the Y direction of the Grid

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		float NodeRadius = 25.0f;							// The radius of each Node, determines the size of the Grid as well

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		float MaxAllowedHeight = 25.0f;						// Max allowable hight for objects to not be considered as obstacles		

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		bool bGridVisible = false;							// bool determines if the nodes on the grid are visible or not

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		float LineThicknessNum = 5.0f;						// The line thickness of the created 2D Grid mesh

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		FColor GridMeshColor = FColor::Green;				// Color of the created grid mesh

	UPROPERTY(EditAnywhere, Category = "Grid Components")
		float GridOpacity = 1.0f;							// Opacity of the created grid mesh

private:
	USceneComponent* DefaultSceneComponent;					// Scene component used as root component for the class
	TArray<AGridNode*> NodesArray;							// TArray of GridNodes to held pointers to all created Nodes 
	FVector2D GridWorldSize;								// FVector2D to hold the size of the Created Grid in world units
	UProceduralMeshComponent* GridMesh;						// ProceduralMeshComponent used to create the 2D Grid Mesh representing the location of each node
	UMaterialInstanceDynamic* GridMaterial;					// Dynamic Material instance for the grid mesh 
};
