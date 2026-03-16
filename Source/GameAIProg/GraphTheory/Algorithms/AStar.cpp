#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	// 1. Kickstart the loop
	std::vector<Node*> path{};
	std::vector<NodeRecord> openList;
	std::vector<NodeRecord> closedList;
	NodeRecord currentNodeRecord{};
	
	NodeRecord startRecord{};
	startRecord.pNode = pStartNode;
	startRecord.pConnection = nullptr;
	startRecord.costSoFar = 0.f;
	startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);

	openList.push_back(startRecord);
	
	// 2. The While Loop 
	while (!openList.empty())
	{
		// A. Get record from the open list with lowest F-score
		auto currentRecord = std::min_element(openList.begin(), openList.end());
		currentNodeRecord = *currentRecord;
		openList.erase(currentRecord);

		// B. Check if that record refers to the end node
		if (currentNodeRecord.pNode == pGoalNode)
		{
			closedList.push_back(currentNodeRecord);
			break;
		}

		// C. Else, we get all the connections of the NodeRecord’s node
		auto connections = pGraph->FindConnectionsFrom(currentNodeRecord.pNode->GetId());
		for (Connection* connection : connections)
		{
			Node* pNextNode = pGraph->GetNode(connection->GetToId()).get();
			float gCost = currentNodeRecord.costSoFar + connection->GetWeight();
        
			// D. Check if the connection leads to a node already on the closedlist
			auto closedIt = std::find_if(closedList.begin(), closedList.end(),
										 [pNextNode](const NodeRecord& r){ return r.pNode == pNextNode; });

			if (closedIt != closedList.end())
			{
				if (closedIt->costSoFar <= gCost)
					continue;
				else
					closedList.erase(closedIt);
			}

			// E. Check if the connection leads to a node already on the openlist
			auto openIt = std::find_if(openList.begin(), openList.end(),
									   [pNextNode](const NodeRecord& r){ return r.pNode == pNextNode; });

			if (openIt != openList.end())
			{
				if (openIt->costSoFar <= gCost)
					continue; // already have a cheaper path
				else
					openList.erase(openIt); // remove, we found cheaper
			}

			// F. We create a new NodeRecord and add it to the openList.
			NodeRecord nextRecord{};
			nextRecord.pNode = pNextNode;
			nextRecord.pConnection = connection;
			nextRecord.costSoFar = gCost;
			nextRecord.estimatedTotalCost = gCost + GetHeuristicCost(pNextNode, pGoalNode);
			openList.push_back(nextRecord);
		}
		
		// G. remove the currentNodeRecord from the openList and add it to the closedList
		openList.erase(std::remove(openList.begin(), openList.end(), currentNodeRecord), openList.end()); // remove from openList correctly
		closedList.push_back(currentNodeRecord);
	}
	
	// 3. Backtracking	
	if (currentNodeRecord.pNode == pGoalNode)
	{
		NodeRecord* record = &currentNodeRecord;

		while(record->pConnection != nullptr)
		{
			path.push_back(record->pNode);

			auto it = std::find_if(closedList.begin(), closedList.end(),
								   [record](const NodeRecord& r)
								   {
									   return r.pNode->GetId() == record->pConnection->GetFromId();
								   });

			if (it != closedList.end())
				record = &(*it);
			else
				break;
		}
	}
	
	path.push_back(startRecord.pNode);
	std::reverse(path.begin(), path.end());
	
	return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}