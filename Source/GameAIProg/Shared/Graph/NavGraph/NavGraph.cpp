#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
	const auto& edges = pNavPoly->GetEdges();
	const auto& triangles = pNavPoly->GetTriangles();

	for (int edgeIdx = 0; edgeIdx < edges.size(); ++edgeIdx)
	{
		const auto& edge = edges[edgeIdx];

		int triangleCount = 0;

		for (const auto& tri : triangles)
		{
			if (tri.HasEdge(edge))
				triangleCount++;
		}

		if (triangleCount >= 2)
		{
			FVector p1 = edge.GetP1(*pNavPoly);
			FVector p2 = edge.GetP2(*pNavPoly);

			FVector2D middle = FVector2D((p1 + p2) * 0.5f);

			// Create node here
			AddNode(std::make_unique<NavGraphNode>(middle, edgeIdx));
		}
	}
	
	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
	for (const auto& tri : triangles)
	{
		std::vector<int> nodeIds;

		auto triEdges = tri.GetEdges();

		for (const auto& edge : triEdges)
		{
			auto edgeIdx = pNavPoly->FindEdgeIndex(edge);

			if (edgeIdx.has_value())
			{
				int nodeId = GetNodeIdFromEdgeIndex(edgeIdx.value());

				if (nodeId != Graphs::InvalidNodeId)
					nodeIds.push_back(nodeId);
			}
		}

		if (nodeIds.size() == 2)
		{
			AddConnection(nodeIds[0], nodeIds[1]);
		}
		else if (nodeIds.size() == 3)
		{
			AddConnection(nodeIds[0], nodeIds[1]);
			AddConnection(nodeIds[1], nodeIds[2]);
			AddConnection(nodeIds[2], nodeIds[0]);
		}
	}
	
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
}
