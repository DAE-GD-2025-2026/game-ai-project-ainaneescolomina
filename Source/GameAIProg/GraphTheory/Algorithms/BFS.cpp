#include "BFS.h"

#include <map>
#include <queue>
#include <set>

#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	std::vector<Node*> path;
	std::queue<Node*> queue[pStartNode];
	std::set<Node*> visited;
	std::map<Node*, Node*> parent;
	
	visited.insert(pStartNode);

	while (!queue->empty())
	{
		Node* node = queue->front();
		queue->pop();
		
		if (node == pDestinationNode)
		{
			break;
		}
		
		auto connections = pGraph->FindConnectionsFrom(node->GetId());

		for (Connection* connection : connections)
		{
			Node* neighbor = pGraph->GetNode(connection->GetToId()).get();

			if (visited.find(neighbor) == visited.end())
			{
				visited.insert(neighbor);
				parent[neighbor] = node;
				queue->push(neighbor);
			}
		}
	}
	
	
	if (parent.find(pDestinationNode) == parent.end())
		return path; // no path found

	Node* current = pDestinationNode;

	while (current != pStartNode)
	{
		path.push_back(current);
		current = parent[current];
	}

	path.push_back(pStartNode);

	std::reverse(path.begin(), path.end());
	
	return path; 
}
