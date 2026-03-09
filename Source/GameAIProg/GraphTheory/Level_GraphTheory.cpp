// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_GraphTheory.h"

#include "Algorithms/EulerianPath.h"
#include "Shared/GameAISpectator.h"

using namespace GameAI;

// Sets default values
ALevel_GraphTheory::ALevel_GraphTheory()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_GraphTheory::BeginPlay()
{
	Super::BeginPlay();
	
	// Add World Ref to Renderer
	Renderer = GraphRenderer{GetWorld()};
	
	// Add the graph editor to our player
	if (PlayerController = Cast<APlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController); 
		GraphEditorClass && PlayerController)
	{
		PlayerGraphEditor = NewObject<UGraphEditorComponent>(PlayerController->GetPawn(), GraphEditorClass);
		PlayerGraphEditor->RegisterComponent();
		PlayerGraphEditor->SetEditedGraph(&Graph);
		PlayerGraphEditor->SetNodeFactory(&NodeFactory);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get PlayerController from LocalPlayer or GraphEditorClass is null"))
		return;
	}
	
	// Make the view orthogonal for less perspective issues
	if (AGameAISpectator* Player = Cast<AGameAISpectator>(PlayerController->GetPawnOrSpectator()); Player)
	{
		Player->SetCameraProjection(ECameraProjectionMode::Orthographic);
	}
	
	// TODO Make the graph and a couple connected nodes here...
	{
		// Create nodes
		auto NodeA = std::make_unique<Node>(FVector2D(0.f, 0.f));
		auto NodeB = std::make_unique<Node>(FVector2D(500.f, 0.f));
		auto NodeC = std::make_unique<Node>(FVector2D(500.f, 500.f));
		auto NodeD = std::make_unique<Node>(FVector2D(0.f, 500.f));

		// Add nodes
		int IdA = Graph.AddNode(std::move(NodeA));
		int IdB = Graph.AddNode(std::move(NodeB));
		int IdC = Graph.AddNode(std::move(NodeC));
		int IdD = Graph.AddNode(std::move(NodeD));

		// Connect nodes
		Graph.AddConnection(IdA, IdB);
		Graph.AddConnection(IdB, IdC);
		Graph.AddConnection(IdC, IdD);
	}
	
	// Spawn the Agent
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Agent->SetSteeringBehavior(&PathFollow);
}

void ALevel_GraphTheory::BeginDestroy()
{
	Super::BeginDestroy();
}

void ALevel_GraphTheory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#pragma region UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
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
		ImGui::Spacing();

		ImGui::Text("Graph Theory");
		ImGui::Spacing();
		ImGui::Spacing();

		//End
		ImGui::End();
	}
#pragma endregion UI
	
	Renderer.RenderGraph(Graph);
	
	// TODO Check if the graph has updated
	if (PlayerGraphEditor->HasGraphUpdated() || firstFrame)
	{
		// TODO if so, run the EulerianPath algorithm
		if (Graph.GetNodeCount() == 0) return;
		EulerianPath eulerian{&Graph};
		Eulerianity eulerianity{};
		auto path = eulerian.FindPath(eulerianity);

		// TODO if a path is found, have the agent follow it
		if (!path.empty() && Agent)
		{
			UpdateAgentPath(path);
		}
		
		firstFrame = false;
	}
	
}

void ALevel_GraphTheory::UpdateAgentPath(std::vector<Node*> const& Trail)
{
	std::vector<FVector2D> path{};
	
	// TODO convert Node vector to positions vector
	path.reserve(Trail.size());
	for (Node* node : Trail)
	{
		path.push_back(node->GetPosition());
	}
	
	PathFollow.SetPath(path);
	if (path.size() > 0)
	{
		Agent->SetPosition(path[0]);
	}
}




