// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_FSM.h"

#include "FSMComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "States/PatrolState.h"
#include "States/ChaseState.h"
#include "States/SearchState.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

// Sets default values
ALevel_FSM::ALevel_FSM()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_FSM::BeginPlay()
{
	Super::BeginPlay();
	
	GuardAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	GuardAgent->SetDebugRenderingEnabled(false);
	
	ThiefAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	ThiefAgent->SetDebugRenderingEnabled(false);
	
	ThiefSeekBehavior = new Seek();
	ThiefAgent->SetSteeringBehavior(ThiefSeekBehavior);

	//TODO
	if (AGameAIController* AIController = Cast<AGameAIController>(GuardAgent->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			// STATES
			auto Patrol = std::make_unique<GameAI::FSM::PatrolState>();
			auto Chase  = std::make_unique<GameAI::FSM::ChaseState>();
			auto Search = std::make_unique<GameAI::FSM::SearchState>();

			Patrol->SetAgent(GuardAgent);
			Chase->SetAgent(GuardAgent);
			Chase->SetTarget(ThiefAgent);
			Search->SetAgent(GuardAgent);

			GameAI::FSM::State* PatrolPtr = Patrol.get();
			GameAI::FSM::State* ChasePtr  = Chase.get();
			GameAI::FSM::State* SearchPtr = Search.get();
			GameAI::FSM::SearchState* SearchStatePtr = Search.get();
			
			FSM->AddState(std::move(Patrol));
			FSM->AddState(std::move(Chase));
			FSM->AddState(std::move(Search));

			FSM->SetInitialState(PatrolPtr);
			
			// TRANSITIONS FSM
			/*
			ASteeringAgent* Guard = GuardAgent;
			ASteeringAgent* Thief = ThiefAgent;
			UWorld* World = GetWorld();

			FSM->AddTransition(PatrolPtr, ChasePtr, [Guard, Thief]()
			{
				if (!Guard || !Thief) return false;

				float dist = FVector::Dist(
					Guard->GetActorLocation(),
					Thief->GetActorLocation()
				);

				return dist < 600.f;
			});
			
			FSM->AddTransition(ChasePtr, SearchPtr, [Guard, Thief, SearchStatePtr]()
			{
				if (!IsValid(Guard) || !IsValid(Thief)) return false;

				float dist = FVector::Dist(
					Guard->GetActorLocation(),
					Thief->GetActorLocation()
				);

				if (dist > 800.f)
				{
					FVector thiefPos = Thief->GetActorLocation();

					SearchStatePtr->SetLastKnownPosition(
						FVector2D(thiefPos.X, thiefPos.Y)
					);

					return true;
				}

				return false;
			});
			
			FSM->AddTransition(SearchPtr, ChasePtr, [Guard, Thief]()
			{
				if (!IsValid(Guard) || !IsValid(Thief)) return false;

				float dist = FVector::Dist(
					Guard->GetActorLocation(),
					Thief->GetActorLocation()
				);

				return dist < 600.f;
			});
			
			FSM->AddTransition(SearchPtr, PatrolPtr, [SearchPtr]()
			{
				auto* Search = static_cast<GameAI::FSM::SearchState*>(SearchPtr);
				if (!Search) return false;

				float elapsed = Search->GetElapsedTime();

				return elapsed > 5.f;
			});
			*/
			
			// TRANSITIONS Behavior Tree
			ASteeringAgent* Guard = GuardAgent;
			ASteeringAgent* Thief = ThiefAgent;

			FSM->AddTransition(PatrolPtr, ChasePtr, [AIController]()
			{
				if (!AIController) return false;

				auto* BB = AIController->GetBlackboardComponent();
				return BB && BB->GetValueAsBool("CanSeeTarget");
			});
			
			FSM->AddTransition(ChasePtr, SearchPtr, [AIController, SearchStatePtr]()
			{
				if (!AIController) return false;

				auto* BB = AIController->GetBlackboardComponent();
				if (!BB) return false;

				bool canSee = BB->GetValueAsBool("CanSeeTarget");

				if (!canSee)
				{
					FVector lastPos = BB->GetValueAsVector("LastKnownPosition");

					SearchStatePtr->SetLastKnownPosition(
						FVector2D(lastPos.X, lastPos.Y)
					);

					return true;
				}

				return false;
			});
			
			FSM->AddTransition(SearchPtr, ChasePtr, [AIController]()
			{
				if (!AIController) return false;

				auto* BB = AIController->GetBlackboardComponent();
				return BB && BB->GetValueAsBool("CanSeeTarget");
			});
			
			FSM->AddTransition(SearchPtr, PatrolPtr, [SearchPtr]()
			{
				auto* Search = static_cast<GameAI::FSM::SearchState*>(SearchPtr);
				if (!Search) return false;

				float elapsed = Search->GetElapsedTime();

				return elapsed > 5.f;
			});
			
			//FSM->StartLogic();
		}
	}
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ThiefSeekBehavior->SetTarget(MouseTarget);
}

