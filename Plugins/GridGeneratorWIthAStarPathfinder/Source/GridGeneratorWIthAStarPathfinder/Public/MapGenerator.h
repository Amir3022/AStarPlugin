// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "Pathfinder.h"
#include "MapGenerator.generated.h"

UCLASS()
class GRIDGENERATORWITHASTARPATHFINDER_API AMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Function called after construction and before begin play to use variable set in the editor to change different aspects of this class behavior
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transfrom) override;
#endif

public:
	// Blueprint callable function, to generate shortest path on grid between 2 random points
	UFUNCTION(BlueprintCallable)
		void PathBetween2RandomPoints();
	// Function to Randomly spawn obstacles on the grid 
	void SpawnObstacles();
	// Function to create to spawn the GridNodes on the Grid after delay, to ensure all obstacles were already created
	void CreateGridAfterDelay();

public:
	// Editor changable variable, to set the Grid used in this class
	UPROPERTY(EditAnywhere, Category = "Grid")
		AGrid* Grid;
	// Editor changable variable, defining the number of spawned blocking objects
	UPROPERTY(EditAnywhere, Category = "Objects Generation")
		int32 nBlockingObstacles = 1;
	// Editor changable variable, defining the number of spawned nonblocking objects
	UPROPERTY(EditAnywhere, Category = "Objects Generation")
		int32 nNonblockingObstacles = 1;

private:
	USceneComponent* DefaultSceneComponent;				// Scene Component to be used root component for this class
	UPathfinder* PathfinderComponent;					// Pointer to Pathfinder actor component to be added to this actor class
	UStaticMesh* BlockingObstacleShape;					// Blocking Static mesh model to be spawned by the SpawnObstacles function, to be set as cube that totally blocks path
	UStaticMesh* NonBlockingObstacleShape;				// Nonblocking Static mesh model to be spawned by the SpawnObstacles function, to be set as wall with open entrance that doesn't block path
	TArray<AActor*> SpawnedMeshes;						// TArray holding the spawned Static Mesh actors
};
