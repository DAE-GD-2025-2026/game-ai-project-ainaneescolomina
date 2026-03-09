#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail

		// TODO Count nodes with odd degree 

		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian

		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes

		// TODO A connected graph with no odd nodes is Eulerian
		
		return Eulerianity::notEulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		// TODO Check if there can be an Euler path
		int oddDegree = 0;
		for (int i = 0; i < Nodes.size(); ++i)
		{
			int degree = graphCopy.FindConnectionsFrom(i).size();
			if (degree % 2 != 0)
			{
				oddDegree++;
				//if (currentNodeId == Graphs::InvalidNodeId) currentNodeId = i;
				currentNodeId = i;
			}
		}
		
		if (oddDegree == 0)
		{
			eulerianity = Eulerianity::eulerian;
			currentNodeId = 0;
		}
		else if (oddDegree == 2)
		{
			eulerianity = Eulerianity::semiEulerian;
		}
		else
		{
			// TODO If this graph is not eulerian, return the empty path
			eulerianity = Eulerianity::notEulerian;
			return Path;
		}
		
		// TODO Start algorithm loop
		std::stack<int> nodeStack;
		
		while (!nodeStack.empty() || graphCopy.FindConnectionsFrom(currentNodeId).size() > 0)
		{
			auto connections = graphCopy.FindConnectionsFrom(currentNodeId);

			if (connections.size() > 0)
			{
				nodeStack.push(currentNodeId);
				int nextNode = connections[0]->GetToId();
				graphCopy.RemoveConnection(currentNodeId, nextNode);

				currentNodeId = nextNode;
			}
			else
			{
				Path.push_back(m_pGraph->GetNode(currentNodeId).get());
				currentNodeId = nodeStack.top();
				nodeStack.pop();
			}
			
		}
		
		Path.push_back(m_pGraph->GetNode(currentNodeId).get());
		
		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		visited[startIndex] = true;
		
		// TODO Ask the graph for the connections from that node
		auto connections = m_pGraph->FindConnectionsFrom(startIndex);
		
		// TODO recursively visit any valid connected nodes that were not visited before
		// TODO Tip: use an index-based for-loop to find the correct index
		for (int i = 0; i < connections.size(); ++i)
		{
			int nodeId = connections[i]->GetToId();

			if (!visited[nodeId])
				VisitAllNodesDFS(Nodes, visited, nodeId);
		}	
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.size() == 0)
			return false;

		// TODO choose a starting node
		std::vector<bool> visitedNodes(Nodes.size(), false);
		int startingNode = -1;

		for (int i = 0; i < Nodes.size(); ++i)
		{
			if (!m_pGraph->FindConnectionsFrom(i).empty())
			{
				startingNode = i;
				break;
			}
		}

		if (startingNode == -1) return true;
		
		// TODO start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, visitedNodes, startingNode );
		
		// TODO if a node was never visited, this graph is not connected
		for (bool visitedNode : visitedNodes)
		{
			if (!visitedNode) return false;
		}
		
		return true;
	}
}