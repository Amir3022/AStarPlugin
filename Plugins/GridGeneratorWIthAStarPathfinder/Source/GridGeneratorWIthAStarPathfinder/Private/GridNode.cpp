// Fill out your copyright notice in the Description page of Project Settings.


#include "GridNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGridNode::AGridNode()
{
	// Creating scene component object for the Default Scene component and setting it as root component
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Component"));
	RootComponent = DefaultSceneComponent;
	// Creating NodeRepresentation as StaticMeshComponent and attaching it to root
	NodeRepresentation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NodeMesh"));
	NodeRepresentation->SetupAttachment(RootComponent);
	// Using Consturctor helpers object finder to get a plane static mesh and created material from editro, setting the Plane static mesh as the Node static mesh, and the M_Color material as it's color
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	UStaticMesh* Mesh = MeshAsset.Object;
	ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("Material'/GridGeneratorWithAStarPathfinder/Materials/M_Color.M_Color'"));
	UMaterial* Material = MaterialAsset.Object;
	NodeRepresentation->SetMaterial(0, Material);
	NodeRepresentation->SetStaticMesh(Mesh);
	// Scale the Node static mesh based on the node radius
	float scalePercenatage = (radius * 2 - 5.0f) / 100.0f;
	NodeRepresentation->SetWorldScale3D(UKismetMathLibrary::Vector_One() * scalePercenatage);
	// Set the node initially to be invisible with no collision
	NodeRepresentation->SetVisibility(false, false);
	NodeRepresentation->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Initalize Parent Node to be nullptr
	ParentNode = nullptr;
}

// Called when the game starts or when spawned
void AGridNode::BeginPlay()
{
	Super::BeginPlay();
	// Create dynamic material from the applied material to the static mesh component, and set the DMI as the new material for the static mesh 
	DynamicMaterial = NodeRepresentation->CreateDynamicMaterialInstance(0, NodeRepresentation->GetMaterial(0));
	NodeRepresentation->SetMaterial(0, DynamicMaterial);
}

void AGridNode::SetVariables(float in_radius, float in_height, bool in_bVisible, int gridX, int gridY)
{
	// Set private variables from the input parameters
	radius = in_radius;
	maxAllowableHeight = in_height;
	float scalePercenatage = (radius * 2 - 5.0f) / 100.0f;
	GridIndexX = gridX;
	GridIndexY = gridY;
	NodeRepresentation->SetWorldScale3D(UKismetMathLibrary::Vector_One() * scalePercenatage);
	// Change the color of the node based on whehter it's visible or not
	bVisible = in_bVisible;
	SetColorOnWalkable();
}

bool AGridNode::CheckForGround()
{
	// Creating HitResult variable to store the output of the line trace
	FHitResult outResult;
	// Creating start and end location for the line trace, starting from the node center, ending below the node with distance determined by GroundDetection 
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = GetActorLocation() + FVector(0.0f, 0.0f, 1.0f) * -1 * GroundDetection;
	// Use line trace by channel to check if there is ground under the node or not
	bool returnValue = GetWorld()->LineTraceSingleByChannel(outResult, StartLocation, EndLocation, ECollisionChannel::ECC_Camera, FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam);
	return returnValue;
}

bool AGridNode::CheckForObstaclesBox()
{
	// Creating HitResult variable to store the output of the box trace
	FHitResult outResult;
	// Start location to be above the node with max allowable height added to the radius of the used box for detection
	FVector StartLocation = GetActorLocation() + GetActorUpVector() * (radius + maxAllowableHeight);
	// Create a box with half size of radius
	FVector HalfSize = UKismetMathLibrary::Vector_One() * radius;
	TArray < AActor*> ActorsToIgnore;
	// Using Box trace to check if there is an obstacle of the node or not
	bool returnValue = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), StartLocation, StartLocation, HalfSize, FRotator(0.0f, 0.0f, 0.0f), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, outResult, true);
	return returnValue;
}

bool AGridNode::CheckWalkable()
{
	// Check if node is walkable or not by checking for ground and obstacles
	if (!CheckForGround() || CheckForObstaclesBox())
	{
		bWalkable = false;
	}
	else
	{
		bWalkable = true;
	}
	return bWalkable;
}

void AGridNode::SetColorOnWalkable()
{
	// Change the color and visibily of the node material based on CheckWalkable
	if (CheckWalkable())
	{
		ChangeColor(FColor::Blue, 0.5f);
		NodeRepresentation->SetVisibility(bVisible, false);
	}
	else
	{
		ChangeColor(FColor::Red, 0.5f);
		NodeRepresentation->SetVisibility(true, false);
	}
}

void AGridNode::ChangeColor(FColor in_Color, float in_Opacity)
{
	// Change the Dynamic Material vector parameter with the new color, and the scalar parameter "Opacity" with new opacity
	DynamicMaterial->SetVectorParameterValue(FName("Color"), in_Color);
	DynamicMaterial->SetScalarParameterValue(FName("Opacity"), in_Opacity);
}

int32 AGridNode::Getf_cost()
{
	// calculate the f_cost by adding g_cost and h_cost
	return g_cost + h_cost;
}

void AGridNode::setNodeVisibility(bool in_bVisible)
{
	// Set the node visibility based on the input parameter
	bVisible = in_bVisible;
	NodeRepresentation->SetVisibility(bVisible, false);
}

int32 AGridNode::GetGridIndexX() const
{
	return GridIndexX;
}

int32 AGridNode::GetGridIndexY() const
{
	return GridIndexY;
}

void AGridNode::Setg_cost(int32 inValue)
{
	g_cost = inValue;
}

int32 AGridNode::Getg_cost() const
{
	return g_cost;
}

void AGridNode::Seth_cost(int32 inValue)
{
	h_cost = inValue;
}

int32 AGridNode::Geth_cost() const
{
	return h_cost;
}

void AGridNode::SetParentNode(AGridNode* inValue)
{
	ParentNode = inValue;
}

AGridNode* AGridNode::GetParentNode() const
{
	return ParentNode;
}