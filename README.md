# A* Pathfinding Algorithm in UE5.3 using C++

This Repo contains implementation of the “GridGeneratorWithAStarPathfinder” plugin which does the following: 
  * Generate a grid with tiles(nodes), detecting if a certain piece of terrain under the tile is walkable or not.
  *  Generate random blocking and nonblocking obstacles on the grid, to better test the pathfinder algorithm.
  *  Chooses two random points in the available grid, and finds the shortest path between these 2 points using the A* pathfinding algorithm.


## Implemented C++ Classes

*  __”GridNode”__: Actor C++ class, implementing logic for each individual node to be placed on the grid. Uses line trace to detect if it has ground below it. Uses a box trace to detect if it’s blocked by an obstacle.
*  __“Grid”__: Actor C++ class, creates the grid with size of GridSizeX * GridSizeY, spawns all nodes and places them on the 2D grid. All different variables of the grid can be changed from editor. Also used to find a node from a world location, and find all neighboring nodes to a certain node.
*  __“Pathfinder”__: Actor Component C++, can be added to any other actor class. Implements the A* pathfinder algorithm to find the shortest path between 2 nodes on the grid.
*  __MapGenerator”__: Actor C++ class, Spawns random blocking and non-blocking obstacles on the used grid, as well as choosing 2 random nodes on the grid to be used as start and target location for the path to be created. Uses the Pathfinder actor component to find the shortest path between start and target node.


## Test Instructions

* Press "SpaceBar" To choose 2 valid start and end points on the grid, ana generate an A* path between them


## Video Demonstration
![Video](Video/VideoDemo.mp4)
