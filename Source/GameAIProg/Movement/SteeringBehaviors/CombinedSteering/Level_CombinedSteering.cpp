#include "Level_CombinedSteering.h"

#include "imgui.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();
	
	FVector SpawnLocaition = FVector::ZeroVector;
	SteeringAgent = GetWorld()->SpawnActor<ASteeringAgent>(AgentClass, SpawnLocaition, FRotator::ZeroRotator);
	
	if (SteeringAgent)
	{
		pSeekBehavior = std::make_unique<Seek>();
		pWanderBehavior = std::make_unique<Wander>();
		
		std::vector<BlendedSteering::WeightedBehavior> blendedBehaviors
		{
				{ pSeekBehavior.get(),   0.5f },
				{ pWanderBehavior.get(),     0.5f }
		};
		
		pBlendedSteering = std::make_unique<BlendedSteering>(blendedBehaviors);
		
		SteeringAgent->SetSteeringBehavior(pBlendedSteering.get());
	}
}

void ALevel_CombinedSteering::BeginDestroy()
{
	Super::BeginDestroy();

}

// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
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
		ImGui::Spacing();
	
		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
   // TODO: Handle the debug rendering of your agents here :)
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

		if (pBlendedSteering)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
		pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
				
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
			pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
			
		}

		//End
		ImGui::End();
	}
#pragma endregion
	
	// Combined Steering Update
 // TODO: implement Make sure to also evade the wanderer
	
	if (SteeringAgent)
	{
		PlayerController = GetWorld()->GetFirstPlayerController();
		
		if (PlayerController && PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			UseMouseTarget = true;
		}

 // TODO: implement handling mouse click input for seek	
		if (UseMouseTarget && pSeekBehavior)
		{
			pSeekBehavior->SetTarget(MouseTarget);
		}
		
		if (CanDebugRender)
		{
			DrawDebugCircle(GetWorld(), SteeringAgent->GetActorLocation(), 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
		}
	}
}
