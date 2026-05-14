#pragma once
#include <vector>
#include <algorithm>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
    class ArticulationPoints final
    {
    public:
        ArticulationPoints(Graph* const pGraph) : m_pGraph(pGraph) {}

        std::vector<int> FindArticulationPointIds()
        {
            if (!m_pGraph || m_pGraph->GetNodeCount() == 0) return {};

            const int nodeCount = m_pGraph->GetNodeCount();
            std::vector<int> discoveryTime(nodeCount, -1);
            std::vector<int> lowestReachableTime(nodeCount, -1);
            std::vector<bool> isArticulationPoint(nodeCount, false);
            
            int currentTime = 0;            
            for (auto* node : m_pGraph->GetActiveNodes())
            {
                const int id = node->GetId();
                if (discoveryTime[id] == -1)
                {
                    ArticulationDFS(id, -1, currentTime, discoveryTime, lowestReachableTime, isArticulationPoint);
                }
            }

            std::vector<int> result;
            for (int i = 0; i < nodeCount; ++i)
            {
                if (isArticulationPoint[i])
                    result.push_back(i);
            }
            return result;
        }

    private:
        void ArticulationDFS(int currentNodeId, int parentNodeId, int& timer, std::vector<int>& discoveryTime, std::vector<int>& lowestReachableTime, std::vector<bool>& isArticulationPoint)
        {
            discoveryTime[currentNodeId] = ++timer;
            lowestReachableTime[currentNodeId] = timer;
            int children = 0;

            auto connections = m_pGraph->FindConnectionsFrom(currentNodeId);
            for (auto* conn : connections)
            {
                int neighborNodeId = conn->GetToId();
                if (neighborNodeId == parentNodeId) continue;

                if (discoveryTime[neighborNodeId ] != -1)
                {
                    lowestReachableTime[currentNodeId] = std::min(lowestReachableTime[currentNodeId], discoveryTime[neighborNodeId ]);
                }
                else
                {
                    children++;
                    ArticulationDFS(neighborNodeId , currentNodeId, timer, discoveryTime, lowestReachableTime, isArticulationPoint);
                    lowestReachableTime[currentNodeId] = std::min(lowestReachableTime[currentNodeId], lowestReachableTime[neighborNodeId ]);
                    
                    // Non-Root condition
                    if (parentNodeId != -1 && lowestReachableTime[neighborNodeId ] >= discoveryTime[currentNodeId])
                        isArticulationPoint[currentNodeId] = true;
                }
            }
            
            // Root condition
            if (parentNodeId == -1 && children > 1)
                isArticulationPoint[currentNodeId] = true;
        }

        Graph* m_pGraph;
    };
}