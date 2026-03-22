#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	auto startTriangle = pNavGraph->GetNavPolygon()->GetTriangleAtPosition(startPos, true);
	auto endTriangle = pNavGraph->GetNavPolygon()->GetTriangleAtPosition(endPos, true);
	
	if (startTriangle == nullptr || endTriangle == nullptr)
		return finalPath;
	if (startTriangle == endTriangle)
	{
		finalPath.push_back(startPos);
		finalPath.push_back(endPos);
		return finalPath;	
	}
	
	//We have valid start/end triangles and they are not the same
	//=> Start looking for a path
	//Copy the graph
	auto graph = pNavGraph->Clone();
	
	//Create Extra node for the Start Node (Agent's position
	auto startNode = std::make_unique<NavGraphNode>(startPos, -1);
	int startNodeId = graph->AddNode(std::move(startNode));
	
	auto edges = startTriangle->GetEdges();

	for (auto& edge : edges)
	{
		auto edgeIdx = pNavGraph->GetNavPolygon()->FindEdgeIndex(edge);

		if (edgeIdx.has_value())
		{
			int nodeId = graph->GetNodeIdFromEdgeIndex(edgeIdx.value());

			if (nodeId != Graphs::InvalidNodeId)
			{
				float cost = UE::Geometry::Distance(startPos, graph->GetNode(nodeId)->GetPosition());

				graph->AddConnection(std::make_unique<Connection>(startNodeId, nodeId));
				graph->FindConnection(startNodeId, nodeId)->SetWeight(cost);
			}
		}
	}
	
	//Create extra node for the endNode
	auto endNode = std::make_unique<NavGraphNode>(endPos, -1);
	int endNodeId = graph->AddNode(std::move(endNode));
	
	auto endEdges = endTriangle->GetEdges();

	for (auto& edge : endEdges)
	{
		auto edgeIdx = pNavGraph->GetNavPolygon()->FindEdgeIndex(edge);

		if (edgeIdx.has_value())
		{
			int nodeId = graph->GetNodeIdFromEdgeIndex(edgeIdx.value());

			if (nodeId != Graphs::InvalidNodeId)
			{
				float cost = UE::Geometry::Distance(endPos, graph->GetNode(nodeId)->GetPosition());

				graph->AddConnection(std::make_unique<Connection>(nodeId, endNodeId));
				graph->FindConnection(nodeId, endNodeId)->SetWeight(cost);
			}
		}
	}

	//Run A star on new graph
	AStar astar(graph.get(), HeuristicFunctions::Euclidean);
	Node* pStartNode = graph->GetNode(startNodeId).get();
	Node* pEndNode = graph->GetNode(endNodeId).get();
	
	auto nodePath = astar.FindPath(pStartNode, pEndNode);
	if (nodePath.empty())
		return finalPath;
	
	//Debug Visualisation
	for (auto node : nodePath)
	{
		FVector2D pos = node->GetPosition();
		finalPath.push_back(pos);
		debugNodePositions.push_back(pos);
	}
	
	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	// debugPortals = SSFA::FindPortals(nodes, *pNavGraph->GetNavPolygon());
	// finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}