#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	{
		//Container
	    std::vector<NavLine> Portals = {};
		if (Path.size() < 2) 
			return Portals;
		
		FVector2D startPos = Path.front()->GetPosition();
		Portals.push_back({startPos, startPos});

	    //For each node received, get it's corresponding line
	    for (size_t i = 1; i < Path.size() - 1; ++i)
	    {
	        Node* CurrentNode = Path[i];
	        Node* NextNode = Path[i + 1];

	    	int EdgeIdx = static_cast<NavGraphNode*>(CurrentNode)->GetEdgeIdx();
	    	if (EdgeIdx == -1)
	    	{
	    		Portals.push_back({ CurrentNode->GetPosition(), CurrentNode->GetPosition() });
	    		continue;
	    	}
	    	
	    	const auto& Edge = NavPoly.GetEdges()[EdgeIdx];
	    	FVector2D P1(Edge.GetP1(NavPoly));
	    	FVector2D P2(Edge.GetP2(NavPoly));

	    	FVector2D forward = NextNode->GetPosition() - CurrentNode->GetPosition();

	    	FVector2D RightPoint, LeftPoint;
	    	if (Cross(forward, P1 - CurrentNode->GetPosition()) > 0.f)
	    	{
	    		RightPoint = P1;
	    		LeftPoint  = P2;
	    	}
	    	else
	    	{
	    		RightPoint = P2;
	    		LeftPoint  = P1;
	    	}
	    	
	    	if (!Portals.empty() &&
				Portals.back().P1.Equals(RightPoint, 0.001f) &&
				Portals.back().P2.Equals(LeftPoint, 0.001f))
	    		continue;

	    	//Store portal
			Portals.push_back({ RightPoint, LeftPoint });
	    }

		//Add degenerate portal to force end evaluation
	    if (!Path.empty())
	    {
	        FVector2D EndPos = Path.back()->GetPosition();
	        Portals.push_back({ EndPos, EndPos });
	    }

	    return Portals;
	}
		
	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> Path{};
	    if (Portals.empty()) return Path;

	    //P1 == right point of portal, P2 == left point of portal
		FVector2D apexPos = Portals[0].P1;
		FVector2D portalRight = Portals[1].P1;
		FVector2D portalLeft  = Portals[1].P2;

		size_t leftIndex = 1, rightIndex = 1;

		Path.push_back(apexPos);

	    for (size_t i = 1; i < Portals.size(); ++i)
	    {
	    	FVector2D left = Portals[i].P2;
	    	FVector2D right = Portals[i].P1;

	        //--- RIGHT CHECK ---
	        //1. See if moving funnel inwards - RIGHT
	        if (Cross(apexPos, portalRight, right) <= 0)
	        {
	            //2. See if new line degenerates a line segment - RIGHT
	        	if (Cross(apexPos, portalLeft, right) > 0)
	        	{
	        		portalRight = right;
	        		rightIndex = i;
	        	}
	            else
	            {
	                //Leftleg becomes new apex point
	            	if (Path.empty() || !Path.back().Equals(portalLeft, 0.001f))
	            		Path.push_back(portalLeft);
	            	
	            	apexPos = portalLeft;

	            	if (leftIndex + 1 >= Portals.size())
	            		break;
	            	
	            	size_t newIndex = leftIndex + 1;
	            	i = newIndex;
	            	leftIndex = newIndex;
	            	rightIndex = newIndex;

	            	portalLeft = apexPos;
	            	portalRight = apexPos;
	            	continue;
	            }
	        }

	        //--- LEFT CHECK ---
	        //1. See if moving funnel inwards - LEFT
	        if (Cross(apexPos, portalLeft, left) >= 0)
	        {
	            //2. See if new line degenerates a line segment - LEFT
	            if (Cross(apexPos, portalRight, left) < 0)
	            {
	            	portalLeft = left;
	            	leftIndex = i;
	            }
	            else
	            {
	                //Rightleg becomes new apex point
	            	if (Path.empty() || !Path.back().Equals(portalRight, 0.001f))
	            		Path.push_back(portalRight);
	            	
	            	apexPos = portalRight;

	            	if (rightIndex + 1 >= Portals.size())
	            		break;
	            	
	            	size_t newIndex = rightIndex + 1;
	            	i = newIndex;
	            	leftIndex = newIndex;
	            	rightIndex = newIndex;

	            	portalLeft = apexPos;
	            	portalRight = apexPos;
	            	continue;
	            }
	        }
	    }

	    // Add last path point
		if (!Portals.empty() && Path.back() != Portals.back().P1)
		{
			Path.push_back(Portals.back().P1);
		}

	    return Path;
	}
		
private:
	SSFA() {};
	~SSFA() {};
		
	static float Cross(const FVector2D& A, const FVector2D& B)
	{
		return A.X * B.Y - A.Y * B.X;
	}

	static float Cross(const FVector2D& A, const FVector2D& B, const FVector2D& C)
	{
		return Cross(B - A, C - A);
	}
};
}
