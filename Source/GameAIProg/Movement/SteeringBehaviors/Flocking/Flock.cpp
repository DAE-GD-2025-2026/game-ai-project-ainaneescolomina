#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"
#include "../SpacePartitioning/SpacePartitioning.h"

Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);

 // TODO: initialize the flock and the memory pool
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	Neighbors.SetNum(FlockSize);
	NrOfNeighbors = 0;
#endif
	
	// Create CellSpace
	int rows = 10;
	int cols = 10;

	pCellSpace = new CellSpace(
		pWorld,
		WorldSize * 2.f,
		WorldSize * 2.f,
		rows,
		cols,
		FlockSize
	);
	
	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pVelMatchBehavior = std::make_unique<VelocityMatch>(this);

	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	
	pEvadeBehavior = std::make_unique<Evade>();
	pEvadeBehavior->SetEvadeRadius(300.f);

	std::vector<BlendedSteering::WeightedBehavior> blendedBehaviors
	{
		{ pSeparationBehavior.get(), 0.f },
		{ pCohesionBehavior.get(),   0.f },
		{ pVelMatchBehavior.get(),   0.f },
		{ pSeekBehavior.get(),   5.f },
		{ pWanderBehavior.get(),     0.f }
	};

	pBlendedSteering = std::make_unique<BlendedSteering>(blendedBehaviors);
	
	std::vector<ISteeringBehavior*> priorityBehaviors
    {
        pEvadeBehavior.get(),
        pBlendedSteering.get()
    };
    
    pPrioritySteering = std::make_unique<PrioritySteering>(priorityBehaviors);
	
	pAgentToEvade->SetSteeringBehavior(pPrioritySteering.get());
	
	for (int i = 0; i < FlockSize; ++i)
	{
		if (!pWorld) continue;

		FVector SpawnLocation = FVector(
			FMath::FRandRange(-WorldSize, WorldSize),
			FMath::FRandRange(-WorldSize, WorldSize),
			0.f);

		FActorSpawnParameters SpawnParams;
		ASteeringAgent* pAgent = pWorld->SpawnActor<ASteeringAgent>(
			AgentClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams);

		if (pAgent)
		{
			Agents[i] = pAgent;
			//pAgent->SetSteeringBehavior(pBlendedSteering.get());
			pAgent->SetSteeringBehavior(pPrioritySteering.get());
			
			if (pCellSpace)
			{
				pCellSpace->AddAgent(*pAgent);
			}
			
			// Turn Off Tick of Agent
			pAgent->SetActorTickEnabled(false);
		}
	}
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
	for (ASteeringAgent* pAgent : Agents)
	{
		if (pAgent)
			pAgent->Destroy();
	}
	
	if (pCellSpace)
	{
		delete pCellSpace;
		pCellSpace = nullptr;
	}
}

void Flock::Tick(float DeltaTime)
{	
	if (pEvadeBehavior && pAgentToEvade)
	{
		FSteeringParams target;
		target.Position = pAgentToEvade->GetPosition();
		target.LinearVelocity = pAgentToEvade->GetLinearVelocity();

		pEvadeBehavior->SetTarget(target);
	}
	
 // TODO: update the flock
	for (ASteeringAgent* pAgent : Agents)
	{
 // TODO: for every agent:
		if (!pAgent) continue;
		
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
		if (bUseSpacePartitioning && pCellSpace)
		{
			pCellSpace->RegisterNeighbors(*pAgent, NeighborhoodRadius);
		}
		else
		{
			RegisterNeighbors(pAgent);
		}
		
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
		FVector2D oldPos = pAgent->GetPosition();
		pAgent->Tick(DeltaTime);

		if (bUseSpacePartitioning && pCellSpace)
		{
			pCellSpace->UpdateAgentCell(*pAgent, oldPos);
		}
		
  // TODO: trim the agent to the world
	}
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
	
	if (DebugRenderSteering)
	{
		
	}
	
	if (DebugRenderPartitions && bUseSpacePartitioning && pCellSpace)
		pCellSpace->RenderCells();
	
	if (DebugRenderNeighborhood) 
		RenderNeighborhood();
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		ImGui::Checkbox("DebugRenderSteering", &DebugRenderSteering);
		ImGui::Checkbox("DebugRenderNeighborhood", &DebugRenderNeighborhood);
		if (bUseSpacePartitioning) ImGui::Checkbox("DebugRenderPartitions", &DebugRenderPartitions);
		ImGui::Spacing();
		
		ImGui::Separator();
		ImGui::Text("Modes");
		ImGui::Spacing();

		ImGui::Checkbox("Use Spatial Partitioning", &bUseSpacePartitioning);

		/*
		if (ImGui::Checkbox("Use Priority Steering", &bUsePrioritySteering))
		{
			for (ASteeringAgent* pAgent : Agents)
			{
				if (!pAgent) continue;

				if (bUsePrioritySteering)
					pAgent->SetSteeringBehavior(pPrioritySteering.get());
				else
					pAgent->SetSteeringBehavior(pBlendedSteering.get());
			}
		}
		*/

		ImGui::Spacing();
		
  // TODO: implement ImGUI sliders for steering behavior weights here
		if (float* weight = pBlendedSteering->GetWeight(pSeparationBehavior.get()))
		{
			float NewWeight = *weight;
			if (ImGui::SliderFloat("Separation", &NewWeight, 0.f, 5.f))
			{
				*weight = NewWeight;
			}
		}

		if (float* weight = pBlendedSteering->GetWeight(pCohesionBehavior.get()))
		{
			float NewWeight = *weight;
			if (ImGui::SliderFloat("Cohesion", &NewWeight, 0.f, 5.f))
			{
				*weight = NewWeight;
			}
		}

		if (float* weight = pBlendedSteering->GetWeight(pVelMatchBehavior.get()))
		{
			float NewWeight = *weight;
			if (ImGui::SliderFloat("Alignment", &NewWeight, 0.f, 5.f))
			{
				*weight = NewWeight;
			}
		}
		
		if (float* weight = pBlendedSteering->GetWeight(pSeekBehavior.get()))
		{
			float NewWeight = *weight;
			if (ImGui::SliderFloat("Seek", &NewWeight, 0.f, 5.f))
			{
				*weight = NewWeight;
			}
		}

		if (float* weight = pBlendedSteering->GetWeight(pWanderBehavior.get()))
		{
			float NewWeight = *weight;
			if (ImGui::SliderFloat("Wander", &NewWeight, 0.f, 5.f))
			{
				*weight = NewWeight;
			}
		}
		
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
	if (Agents.Num() == 0 || Agents[0] == nullptr) 
		return;

	if (bUseSpacePartitioning && pCellSpace)
		pCellSpace->RegisterNeighbors(*Agents[0], NeighborhoodRadius);
	else
		RegisterNeighbors(Agents[0]);

	int nrNeighbors = bUseSpacePartitioning
		? pCellSpace->GetNrOfNeighbors()
		: NrOfNeighbors;

	const TArray<ASteeringAgent*>& neighbors = bUseSpacePartitioning
		? pCellSpace->GetNeighbors()
		: Neighbors;

	
	FVector2D agentPos = Agents[0]->GetPosition();
	DrawDebugCircle(pWorld, FVector(agentPos.X, agentPos.Y, 0), 40, 50, FColor::Magenta, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		FVector2D neighborPos = Neighbors[i]->GetPosition();
		DrawDebugCircle(pWorld, FVector(neighborPos.X, neighborPos.Y, 0), 40, 50, FColor::Emerald, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	}	
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
 // TODO: Implement
	NrOfNeighbors = 0;
	for (ASteeringAgent* pNeighbor : Agents)
	{
		// skip self
		if (!pNeighbor || pNeighbor == pAgent)
			continue;
		
		FVector2D agentPos = pAgent->GetPosition();
		FVector2D neighborPos = pNeighbor->GetPosition();

		float distSq = FVector2D::DistSquared(agentPos, neighborPos);
		float radiusSq = NeighborhoodRadius * NeighborhoodRadius;

		if (distSq <= radiusSq)
		{
			Neighbors[NrOfNeighbors] = pNeighbor;
			NrOfNeighbors++;
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	int nrNeighbors = NrOfNeighbors;
	auto neighbors = Neighbors;
	
	// NrOfNeighbours it's not updated when using space partitioning
	if (bUseSpacePartitioning)
	{
		nrNeighbors = pCellSpace->GetNrOfNeighbors();
		neighbors = pCellSpace->GetNeighbors();
	}
	
	if (nrNeighbors == 0)
		return FVector2D::ZeroVector;

	FVector2D avgPosition = FVector2D::ZeroVector;

	for (int i = 0; i < nrNeighbors; ++i)
	{
		avgPosition += neighbors[i]->GetPosition();
	}
	
	avgPosition /= nrNeighbors;
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	if (NrOfNeighbors == 0)
		return FVector2D::ZeroVector;

	FVector2D avgVelocity = FVector2D::ZeroVector;

	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgVelocity += Neighbors[i]->GetLinearVelocity();
	}

	avgVelocity /= NrOfNeighbors;
	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
	pSeekBehavior->SetTarget(Target);
}

