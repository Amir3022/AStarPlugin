// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GridNode.generated.h"

UCLASS()
class GRIDGENERATORWITHASTARPATHFINDER_API AGridNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridNode();

public:
	// Setting the variables of the Node, called after spawning the nodes in the Grid class
	void SetVariables(float in_radius, float in_height, bool bVisible, int gridX, int gridY);
	// Check if there is ground below the node
	bool CheckForGround();
	// Check if the node is obstruced by any obstacle
	bool CheckForObstaclesBox();
	// Check if the node is walkable, used in pathfinding
	bool CheckWalkable();
	// Change the color of node to red if it's unwalkable
	void SetColorOnWalkable();
	// Change the color and the opacity of the node material
	void ChangeColor(FColor in_Color, float in_Opacity);
	// Calculate the f_cost of the node from g_cost and h_cost
	int32 Getf_cost();
	// Set the node to be visible or only a grid mesh 
	void setNodeVisibility(bool in_bVisible);
	// Get the X index of the node in the Grid 
	int32 GetGridIndexX() const;
	// Get the Y index of the node in the Grid
	int32 GetGridIndexY() const;

	// Setters and Getters for private variables
	void Setg_cost(int32 inValue);
	int32 Getg_cost() const;
	void Seth_cost(int32 inValue);
	int32 Geth_cost() const;
	void SetParentNode(AGridNode* inValue);
	AGridNode* GetParentNode() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool bWalkable;                                         // bool if node is walkable or not
	float radius = 25.0f;									// Used to determine the size of each node on the grid, thus determining the size of the grid
	float maxAllowableHeight = 25.0f;						// Max allowable hight for objects to not be considered as obstacles
	float GroundDetection = 50.0f;							// Distance under the node to check for ground, if distance by the line trace is larger than this value, no ground detected
	USceneComponent* DefaultSceneComponent;					// Scene component used as root of the actor
	UStaticMeshComponent* NodeRepresentation;				// Static mesh representation of the mesh, Plane static mesh used for this
	UMaterialInstanceDynamic* DynamicMaterial;				// Dynamic material instance used to change the node colors in runtime								
	bool bVisible = false;									// bool if node visible or not
	int32 g_cost;											
	int32 h_cost;
	int32 GridIndexX;										// X index of this node on the 2D Grid
	int32 GridIndexY;										// Y index of this node on the 2D Grid
	AGridNode* ParentNode;									// Pointer to the parent node that was used to get to this node, used in pathfinding
};
