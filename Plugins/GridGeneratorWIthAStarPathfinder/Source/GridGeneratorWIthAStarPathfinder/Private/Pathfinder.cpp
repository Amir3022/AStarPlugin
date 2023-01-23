// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinder.h"
#include "Kismet/KismetMathLibrary.h"
#include "Algo/Reverse.h"

// Sets default values for this component's properties
UPathfinder::UPathfinder()
{
	// Initialize Grid to be null pointer
	Grid = nullptr;
}

void UPathfinder::FindPath(FVector StartPos, FVector TargetPos)
{
	// Calculate Start and target nodes from start and target positions
	AGridNode* StartNode = Grid->NodeFromLocation(StartPos);
	AGridNode* TargetNode = Grid->NodeFromLocation(TargetPos);
	// If both start and end nodes are valid, call the FindPathNode with them
	if (StartNode != nullptr && TargetNode != nullptr)
	{
		FindPathNode(StartNode, TargetNode);
	}
}

void UPathfinder::FindPathNode(AGridNode* StartNode, AGridNode* TargetNode)
{
	// Get start time in milliseconds by getting time in seconds and multiplying by 1000
	double startTime = FPlatformTime::Seconds() * 1000.0f;
	// Reset previous path using the function for that
	ResetLastPath();
	// Ensure Grid isn't nullptr before operation
	if (Grid == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grid Variable not set"));
		return;
	}
	// Create TArrays of GridNodes to store OpenNodes and Analyzed nodes
	TArray<AGridNode*> OpenNodes;
	TArray<AGridNode*> AnalyzedNodes;
	// Set g_cost and h_cost of the start node and add it to OpenNodes array
	StartNode->Setg_cost(0);
	StartNode->Seth_cost(GetDistanceBetweenNodes(StartNode, TargetNode));
	OpenNodes.Add(StartNode);
	// Iteratre while there are still OpenNodes available in the array
	while (!OpenNodes.IsEmpty())
	{
		// Initalize curretnNode to be first node in OpenNodes
		AGridNode* CurrentNode = OpenNodes[0];
		// Iterate over all OpenNodes and get node with the smallest f_cost, if more than one node have same f_cost, get the one with the smallest h_cost, set it as currentNode
		for (auto& Node : OpenNodes)
		{
			if (Node->Getf_cost() < CurrentNode->Getf_cost() || (Node->Getf_cost() == CurrentNode->Getf_cost() && Node->Geth_cost() < CurrentNode->Geth_cost()))
			{
				CurrentNode = Node;
			}
		}
		// Remove CurrentNode from OpenNodes and add it to AnalyzedNode
		OpenNodes.Remove(CurrentNode);
		AnalyzedNodes.Add(CurrentNode);
		// If CurrentNode equals TargetNode, we have reached the target so we can start retracing the path
		if (CurrentNode == TargetNode)
		{
			// Set the CurrentPath Array
			CurrentPath = RetracePath(StartNode, TargetNode);
			// Change the color of StartNode to green, TargetNode to yellow, all path nodes to black and set the nodes visible
			for (auto& Node : CurrentPath)
			{
				if (Node == StartNode)
				{
					Node->ChangeColor(FColor::Green, 1.0f);
				}
				else if (Node == TargetNode)
				{
					Node->ChangeColor(FColor::Yellow, 1.0f);
				}
				else
				{
					Node->ChangeColor(FColor::Black, 1.0f);
				}
				Node->setNodeVisibility(true);
			}
			// Get time after algorithm finished executing, print to log the time it took to find the path from start to target, and then return from this function
			double endTime = FPlatformTime::Seconds() * 1000.0f;
			UE_LOG(LogTemp, Warning, TEXT("Total Time taken by Algorithm in milliseconds: %f"), (endTime - startTime));
			return;
		}
		// Get all neighbor nodes using the Grid method, as input we use the CurrentNode
		TArray<AGridNode*> NeighborNodes = Grid->GetNeighborNodes(CurrentNode);
		for (auto& neighbor : NeighborNodes)
		{
			// If a neighbor node is unwalkable or already analyzed, skip it
			if (!neighbor->CheckWalkable() || AnalyzedNodes.Contains(neighbor))
			{
				continue;
			}
			// Calculate new g_cost for each neighbor node from CurrentNode
			int32 g_costNeighborNew = GetDistanceBetweenNodes(CurrentNode, neighbor) + CurrentNode->Getg_cost();
			// If calculated g_cost less than old g_cost for neighbor node or neighbor node not in OpenNodes, Change the g_cost of the neighbor node to the calculated one, calculate h_cost for the node, set currentNode to be its parent Node
			if (g_costNeighborNew < neighbor->Getg_cost() || !OpenNodes.Contains(neighbor))
			{
				neighbor->Setg_cost(g_costNeighborNew);
				neighbor->Seth_cost(GetDistanceBetweenNodes(neighbor, TargetNode));
				neighbor->SetParentNode(CurrentNode);
				// If neighbor node no in OpenNodes, add it to OpenNodes array 
				if (!OpenNodes.Contains(neighbor))
				{
					OpenNodes.Add(neighbor);
				}
			}
		}
	}
}

int32 UPathfinder::GetDistanceBetweenNodes(const AGridNode* StartNode, const AGridNode* EndNode)
{
	
	// Get distance between the 2 nodes in the X direction
	int32 DistanceX = UKismetMathLibrary::Abs_Int(EndNode->GetGridIndexX() - StartNode->GetGridIndexX());
	// Get distance between the 2 nodes in the Y direction 
	int32 DistanceY = UKismetMathLibrary::Abs_Int(EndNode->GetGridIndexY() - StartNode->GetGridIndexY());
	int32 distance;
	// The direct unobstructed path between 2 nodes will be number of diagonal moves equal to min(DistanceX,DistanceY) then either vertical or horizontal moves equal to max(DistanceX,DistanceY) - min(DistanceX,DistanceY) 
	// Each diagonal move has a cost of 14 and each horizontal or vertical move has a cost of 10 (Assuming square of length = 1, Distance in V or H direction = 1, Distance in Diagonal direction = sqrt(2) =~ 1.4, for simplicity are made to be 10 and 14 respectively)
	if (DistanceX < DistanceY)
	{
		distance = DistanceX * 14 + (DistanceY - DistanceX) * 10;
	}
	else
	{
		distance = DistanceY * 14 + (DistanceX - DistanceY) * 10;
	}
	return distance;
}

TArray<AGridNode*> UPathfinder::RetracePath(const AGridNode* StartNode, AGridNode* EndNode)
{
	// Create TArray to store GridNodes on path
	TArray<AGridNode*> PathNodes;
	// Add the end node to the PathNodes array 
	PathNodes.Add(EndNode);
	// Set initialy the currentnode pointer to point at EndNode
	AGridNode* CurrentNode = EndNode;
	// Change CurrentNode to its parent as long as it doens't equal startNode
	while (CurrentNode != StartNode)
	{
		CurrentNode = CurrentNode->GetParentNode();
		// Add each node in the path to the PathArray
		PathNodes.Add(CurrentNode);
	}
	// Reverse the PathArray to be in the correct order from StartNode to EndNode
	Algo::Reverse(PathNodes);
	return PathNodes;
}

void UPathfinder::ResetLastPath()
{
	// Iterate over all nodes on current path array, and set set to invisible and change their color to the default color
	for (auto& Node : CurrentPath)
	{
		Node->setNodeVisibility(false);
		Node->SetColorOnWalkable();
	}
}

