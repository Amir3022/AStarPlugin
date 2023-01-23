// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGrid::AGrid()
{
	// Creating scene component object for the Default Scene component and setting it as root component
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Component"));
	RootComponent = DefaultSceneComponent;
	// Creating Procedural Mesh Component to represent the Grid Mesh, and attach it to root component
	GridMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Grid Procedural Mesh"));
	GridMesh->SetupAttachment(RootComponent);
	//// Using Consturctor helpers object finder to get the M_Color material from plugin folder
	ConstructorHelpers::FObjectFinder<UMaterial> MaterialInstance(TEXT("Material'/GridGeneratorWithAStarPathfinder/Materials/M_Color.M_Color'"));
	UMaterial* MaterialObject = MaterialInstance.Object;
	// Create Dynamic material instance from the found material, and set as the material of the mesh component
	GridMaterial = GridMesh->CreateDynamicMaterialInstance(0, MaterialObject);
	GridMesh->SetMaterial(0, GridMaterial);
}

void AGrid::OnConstruction(const FTransform& Transform)
{
	// Set the Dynamic material color and opacity with values from editor
	GridMaterial->SetVectorParameterValue(FName("Color"), GridMeshColor);
	GridMaterial->SetScalarParameterValue(FName("Opacity"), GridOpacity);
	// Set the value of GridWorldSize based on GridSizeX and GridSizeY set in editor
	float NodeDiameter = NodeRadius * 2;
	GridWorldSize.X = GridSizeX * NodeDiameter;
	GridWorldSize.Y = GridSizeY * NodeDiameter;
	// Create the 2D Grid mesh using the procedural mesh component
	CreateGridMesh();
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrid::CreateGrid()
{
	// Calculate the value of GridWorldSize based on GridSizeX and GridSizeY
	float NodeDiameter = NodeRadius * 2;
	GridWorldSize.X = GridSizeX * NodeDiameter;
	GridWorldSize.Y = GridSizeY * NodeDiameter;
	// Get the bottom left corner locaton of the grid, used to calculate the locations of all other nodes afterwards
	FVector BottomLeftLocation = GetActorLocation() + FVector(-1.f * GridWorldSize.X / 2.0f, -1.f * GridWorldSize.Y / 2.0f, GetActorLocation().Z);
	for (int y = 0; y < GridSizeY; y++)
	{
		for (int x = 0; x < GridSizeX; x++)
		{
			// Calculate center location of each new node to be added to the grid
			FVector SpawnLocation = BottomLeftLocation + FVector(x * NodeDiameter + NodeRadius, y * NodeDiameter + NodeRadius, GetActorLocation().Z);
			// Spawn new GridNode Actor in the calculated location
			AGridNode* NewNode = GetWorld()->SpawnActor<AGridNode>(SpawnLocation, FRotator(0.0f, 0.0f, 0.0f));
			// Set Different variables on the Node using the setNodeVariables function
			NewNode->SetVariables(NodeRadius, MaxAllowedHeight, bGridVisible, x, y);
			// Add the created Node to the GridNodes Array
			NodesArray.Add(NewNode);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of Nodes added: %i"), NodesArray.Num());
}

AGridNode* AGrid::NodeFromLocation(FVector WorldLocation)
{
	// Calculate the Bottom left corner and top right corner world locations 
	FVector BottomLeftLocation = GetActorLocation() + FVector(-1.f * GridWorldSize.X / 2.0f, -1.f * GridWorldSize.Y / 2.0f, GetActorLocation().Z);
	FVector TopRightLocation = GetActorLocation() + FVector(GridWorldSize.X / 2.0f, GridWorldSize.Y / 2.0f, GetActorLocation().Z);
	// Ensure the input location is withtin the bounds of bottom left and top right corner locations
	if ((WorldLocation.X >= BottomLeftLocation.X && WorldLocation.X <= TopRightLocation.X) && (WorldLocation.Y >= BottomLeftLocation.Y && WorldLocation.Y <= TopRightLocation.Y))
	{
		// Get the input locaton relative to the bottom left corner
		FVector RelativeLocation = WorldLocation - BottomLeftLocation;
		// Normalize the relative location with the distance between bottom left and top right corner in the X and Y directions
		FVector RelativeLocationFrac;
		RelativeLocationFrac.X = RelativeLocation.X / (TopRightLocation.X - BottomLeftLocation.X);
		RelativeLocationFrac.Y = RelativeLocation.Y / (TopRightLocation.Y - BottomLeftLocation.Y);
		// Multiply the fractions in each direction by the GridSizeX and GridSizeY respectively to get the X and Y indices on the GridArray
		float Xindex = UKismetMathLibrary::Round(RelativeLocationFrac.X * (GridSizeX - 1));
		float Yindex = UKismetMathLibrary::Round(RelativeLocationFrac.Y * (GridSizeY - 1));
		// Get the node from array with XIndex and YIndex
		AGridNode* ReturnNode = NodesArray[Yindex * GridSizeX + Xindex];
		// Check if GridNode is valid if so return it, else return nullptr
		if (ReturnNode)
			return ReturnNode;
		else
			return nullptr;
	}
	// If input location outside the bounds specified, return nullptr
	else
	{
		return nullptr;
	}
}

TArray<AGridNode*> AGrid::GetNeighborNodes(AGridNode* Node)
{
	// Create TArray of GridNodes to store neighbor nodes
	TArray<AGridNode*> NeighborNodes;
	// Set the start location as the X and Y indices of the input Node
	int StartX = Node->GetGridIndexX();
	int StartY = Node->GetGridIndexY();
	// Iterate from -1 to +1 from the start location in the X and Y direction to get the 8 neighbor nodes if valid
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			if (x == 0 && y == 0)
			{
				// If X=Y=0, we are pointing at the input node, we skip since it's not added to the neighbor nodes
				continue;
			}
			// Calculate IndexX and IndexY for each neighbor node, and ensure they are withing the range of valid indices
			int indexX = StartX + x;
			int indexY = StartY + y;
			if ((indexX >= 0 && indexX < GridSizeX) && (indexY >= 0 && indexY < GridSizeY))
			{
				// Add each neighbor node to the NeighborNodes array 
				int index = indexX + indexY * GridSizeX;
				NeighborNodes.Add(NodesArray[index]);
			}
		}
	}
	// return the NeighborNodes array
	return NeighborNodes;
}

void AGrid::CreateGridMesh()
{
	// Calculate the Bottomleft corner location of the Grid
	FVector BottomLeftLocation = GetActorLocation() + FVector(-1.f * GridWorldSize.X / 2.0f, -1.f * GridWorldSize.Y / 2.0f, GetActorLocation().Z);
	// Create TArray of FVector to store calculated Vertices
	TArray<FVector> Vertices;
	// Create TArray of int32 to store calculated Triangles
	TArray<int32> Triangles;
	// Iterate in range of GridSizeX to create nGridSizeX lines in the X direction each having length of GridSizeY * Node Length, the CreateLine function adds the vertices and triangles of created line to their respective arrays
	for (int32 i = 0; i <= GridSizeX; i++)
	{
		FVector StartPosition = BottomLeftLocation + FVector(i * NodeRadius * 2, 0.0f, 0.0f);
		FVector EndPosition = StartPosition + FVector(0.0f, GridSizeY * 2 * NodeRadius, 0.0f);
		CreateLine(StartPosition, EndPosition, LineThicknessNum, Vertices, Triangles);
	}
	// Iterate in range of GridSizeY to create nGridSizeY lines in the Y direction each having length of GridSizeX * Node Length, the CreateLine function adds the vertices and triangles of created line to their respective arrays
	for (int32 i = 0; i <= GridSizeY; i++)
	{
		FVector StartPosition = BottomLeftLocation + FVector(0.0f, i * NodeRadius * 2, 0.0f);
		FVector EndPosition = StartPosition + FVector(GridSizeX * 2 * NodeRadius, 0.0f, 0.0f);
		CreateLine(StartPosition, EndPosition, LineThicknessNum, Vertices, Triangles);
	}
	// Create empty arrays for Normals, UV0, VerticesColors, tangents to be used in the create mesh section function
	TArray<FVector> Normals;
	TArray<FVector2d> UV0;
	TArray<FColor> VerticesColors;
	TArray<FProcMeshTangent> Tangents;
	// Use the ProceduralMeshComponent to CreateMeshSection with vertices and triangles representing the vertical and horizontal lines in the X and Y directions to create the Grid Mesh
	GridMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VerticesColors, Tangents, false);
	GridMesh->SetRelativeLocation(-1.0f * GetActorLocation());
}

void AGrid::CreateLine(FVector StartLocation, FVector EndLocation, float LineThickness, TArray<FVector>& Vertices, TArray<int32>& Triangles)
{
	// Caculate line half thickness from input thickness
	float halfThickness = LineThickness / 2.0f;
	// Caculate the direction of thickness by Cross Product of Line Direction with Up Unit Vector
	FVector LineDirection = EndLocation - StartLocation;
	LineDirection.Normalize();
	FVector ThicknessDirection = LineDirection.Cross(FVector(0.0f, 0.0f, 1.0f));
	// Calculate triangles for the line to be created and append them to the Triangles TArray
	int32 VerticesLen = Vertices.Num();
	TArray<int32> TrianglesSubArray;
	TrianglesSubArray.Add(VerticesLen + 2);
	TrianglesSubArray.Add(VerticesLen + 1);
	TrianglesSubArray.Add(VerticesLen + 0);
	TrianglesSubArray.Add(VerticesLen + 2);
	TrianglesSubArray.Add(VerticesLen + 3);
	TrianglesSubArray.Add(VerticesLen + 1);
	Triangles.Append(TrianglesSubArray);
	// Calculate vertices for the line to be created and append them to the Vertices TArray
	TArray<FVector> VerticesSubArray;
	VerticesSubArray.Add(StartLocation + ThicknessDirection * halfThickness);
	VerticesSubArray.Add(EndLocation + ThicknessDirection * halfThickness);
	VerticesSubArray.Add(StartLocation - ThicknessDirection * halfThickness);
	VerticesSubArray.Add(EndLocation - ThicknessDirection * halfThickness);
	Vertices.Append(VerticesSubArray);
}

FVector2D AGrid::GetGridWorldSize()
{
	// Calculate the value of GridWorldSize based on GridSizeX and GridSizeY
	float NodeDiameter = NodeRadius * 2;
	GridWorldSize.X = GridSizeX * NodeDiameter;
	GridWorldSize.Y = GridSizeY * NodeDiameter;
	return GridWorldSize;
}

