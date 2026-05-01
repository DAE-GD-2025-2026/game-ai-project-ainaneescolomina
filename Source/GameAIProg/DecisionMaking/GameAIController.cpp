// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "FSM/FSMComponent.h"


// Sets default values
AGameAIController::AGameAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BrainComponent = CreateDefaultSubobject<UFSMComponent>(TEXT("FSMComponent"));
	
	// AI Perception
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
	this, &AGameAIController::OnTargetPerceptionUpdated);
}

// Called when the game starts or when spawned
void AGameAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Create Blackboard if need be
	InitFiniteStateMachine();
	
	if (BehaviorTree)
		RunBehaviorTree(BehaviorTree);
}

// Called every frame
void AGameAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameAIController::InitFiniteStateMachine()
{
	UFSMComponent* FSMComp = FindComponentByClass<UFSMComponent>();
	if (ensure(FSMComp) && FSMBlackboardAsset)
	{
		UBlackboardComponent* BlackboardComp = Blackboard;
		UseBlackboard(FSMBlackboardAsset, BlackboardComp);
		Blackboard = BlackboardComp;
	}
}

void AGameAIController::RunFiniteStateMachine()
{
	UFSMComponent* FSMComp = FindComponentByClass<UFSMComponent>();
	if (ensure(FSMComp))
	{
		FSMComp->StartLogic();
	}
}

void AGameAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Blackboard) return;
	if (!Actor) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsObject("TargetActor", Actor);
		Blackboard->SetValueAsBool("CanSeeTarget", true);
	}
	else
	{
		Blackboard->SetValueAsBool("CanSeeTarget", false);

		FVector lastPos = Stimulus.StimulusLocation;
		Blackboard->SetValueAsVector("LastKnownPosition", lastPos);
	}
}



