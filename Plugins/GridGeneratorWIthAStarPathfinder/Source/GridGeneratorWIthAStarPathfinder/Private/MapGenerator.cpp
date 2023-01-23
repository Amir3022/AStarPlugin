// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
	// Creating scene component object for the Default Scene component and setting it as root component
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Component"));
	RootComponent = DefaultSceneComponent;
	// Creating the Pathfinder Actor Component and adding as an owned component to this class
	PathfinderComponent = CreateDefaultSubobject<UPathfinder>(TEXT("Pathfinder Component"));
	AddOwnedComponent(PathfinderComponent);
	// Using ConstructorHelperd ObjectFinder to get the basic shapes meshes, and add them to the ObstacleShapes array
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	ObstacleShapes.Add(SphereAsset.Object);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	ObstacleShapes.Add(CubeAsset.Object);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	ObstacleShapes.Add(ConeAsset.Object);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	ObstacleShapes.Add(CylinderAsset.Object);
}

void AMapGenerator::OnConstruction(const FTransform& Transform)
{
	// Set the Pathfinder Grid to be the grid setup in this class
	PathfinderComponent->Grid = Grid;
}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	// Ensure Grid doesn't equal nullpointer
	if (!Grid)
	{
		UE_LOG(LogTemp, Error, TEXT("Need to set the Grid variable in editor"));
		return;
	}
	// Start by spawning the obstacles on the grid mesh
	SpawnObstacles();
	// Create TimerHandle variable to start the CreateGrid method from Grid class after a delay
	FTimerHandle TH_Delay;
	GetWorld()->GetTimerManager().SetTimer(TH_Delay, this, &AMapGenerator::CreateGridAfterDelay, 0.2f, false, 0.2f);
}

void AMapGenerator::CreateGridAfterDelay()
{
	// Spawn the GridNodes on the Grid Mesh after a delay
	Grid->CreateGrid();
	UE_LOG(LogTemp, Warning, TEXT("Grid Created"));
}

void AMapGenerator::PathBetween2RandomPoints()
{
	// Ensure Grid is set in the editor
	if (!Grid)
	{
		UE_LOG(LogTemp, Error, TEXT("Need to set the Grid variable in editor"));
		return;
	}
	// Get the bottom left corner and top right corner locations using GridWorldSize, and create variable for min and max allowable values for random x and y postions
	FVector2D GridWorldSize = Grid->GetGridWorldSize();
	FVector BottomLeftLocation = Grid->GetActorLocation() + FVector(-1.f * GridWorldSize.X / 2.0f, -1.f * GridWorldSize.Y / 2.0f, Grid->GetActorLocation().Z);
	FVector TopRightLocation = Grid->GetActorLocation() + FVector(GridWorldSize.X / 2.0f, GridWorldSize.Y / 2.0f, Grid->GetActorLocation().Z);
	float minX = BottomLeftLocation.X;
	float maxX = TopRightLocation.X;
	float minY = BottomLeftLocation.Y;
	float maxY = TopRightLocation.Y;
	float locZ = Grid->GetActorLocation().Z;
	// Initialize walkable with false, create pointer for startNode and TargetNode
	bool bWalkable = false;
	AGridNode* StartNode = nullptr;
	AGridNode* TargetNode = nullptr;
	// Get random start location and target location in the bounds of bottom left corner and top right corner
	// Use Start and Target Locations to get Start and End Nodes
	// Check if both Start and Target Node are walkable, use the output to set the bWalkable variable
	// if bWalkable is false, repeat the whole process till we get valid start and target nodes
	while (!bWalkable)
	{
		FVector StartLocation = FVector(UKismetMathLibrary::RandomFloatInRange(minX, maxX), UKismetMathLibrary::RandomFloatInRange(minY, maxY), locZ);
		FVector TargetLocation = FVector(UKismetMathLibrary::RandomFloatInRange(minX, maxX), UKismetMathLibrary::RandomFloatInRange(minY, maxY), locZ);
		StartNode = Grid->NodeFromLocation(StartLocation);
		TargetNode = Grid->NodeFromLocation(TargetLocation);
		bWalkable = StartNode->CheckWalkable() && TargetNode->CheckWalkable();
	}
	// Ensure grid in the pathfinder actor component
	if (!PathfinderComponent->Grid)
	{
		UE_LOG(LogTemp, Error, TEXT("Grid variable not set in the pathfinder component"));
		return;
	}
	// Call the method on Pathfinder to get the shortest path between Start and Target Node
	PathfinderComponent->FindPathNode(StartNode, TargetNode);
}

void AMapGenerator::SpawnObstacles()
{
	// Get the bottom left corner and top right corner locations using GridWorldSize, and create variable for min and max allowable values for random x and y postions
	FVector2D GridWorldSize = Grid->GetGridWorldSize();
	FVector BottomLeftLocation = Grid->GetActorLocation() + FVector(-1.f * GridWorldSize.X / 2.0f, -1.f * GridWorldSize.Y / 2.0f, Grid->GetActorLocation().Z);
	FVector TopRightLocation = Grid->GetActorLocation() + FVector(GridWorldSize.X / 2.0f, GridWorldSize.Y / 2.0f, Grid->GetActorLocation().Z);
	float minX = BottomLeftLocation.X;
	float maxX = TopRightLocation.X;
	float minY = BottomLeftLocation.Y;
	float maxY = TopRightLocation.Y;
	float locZ = Grid->GetActorLocation().Z;
	// Spawn number of static mesh actors to be used as obstacles equal to nObstacles
	for (int32 i = 0; i < nObstacles; i++)
	{
		// Get random spawn location in the bounds of bottom left and top right locations
		FVector SpawnLocation = FVector(UKismetMathLibrary::RandomFloatInRange(minX, maxX), UKismetMathLibrary::RandomFloatInRange(minY, maxY), locZ);
		// Spawn static mesh actor in the random spawn location
		AStaticMeshActor* SpawnedMesh = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnLocation, FRotator(0.0f, 0.0f, 0.0f));
		// If Static Mesh actor successully spawned randomize it's scale in the x and y directions
		if (SpawnedMesh)
		{
			FVector MeshScale = FVector(UKismetMathLibrary::RandomFloatInRange(1, 5), UKismetMathLibrary::RandomFloatInRange(1, 5), 3.0f);
			// Set static mesh to be one of the static meshed in ObstacleMeshes (Currently only Cube is used from the array which has index = 1)
			SpawnedMesh->GetStaticMeshComponent()->SetStaticMesh(ObstacleShapes[1]);
			SpawnedMesh->GetStaticMeshComponent()->SetWorldScale3D(MeshScale);
			// Add the spawed static mesh actor to the SpawnedMeshes array
			SpawnedMeshes.Add(SpawnedMesh);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Spawn mesh"));
		}
	}
}



