// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMComponent.h"

// --- FSM ---
void GameAI::FSM::FSM::AddState(std::unique_ptr<State>&& NewState)
{
	States.push_back(std::move(NewState));
}

void GameAI::FSM::FSM::AddTransition(State* From, State* To, std::function<bool()> Condition)
{
	if (!From || !To) return;
	From->Transitions.push_back({ To, Condition });
}

void GameAI::FSM::FSM::Update(float DeltaTime)
{
	if (!CurrentState) return;

	// Check transitions
	for (auto& Transition : CurrentState->Transitions)
	{
		if (Transition.Condition())
		{
			CurrentState->Exit();
			CurrentState = Transition.To;
			CurrentState->Enter();
			break;
		}
	}

	CurrentState->Update(DeltaTime);
}

void GameAI::FSM::FSM::SetInitialState(State* State)
{
	CurrentState = State;

	if (CurrentState)
	{
		CurrentState->Enter();
	}
}

// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// TODO Setup FSM
	FSMInstance = std::make_unique<GameAI::FSM::FSM>();
}


void UFSMComponent::AddState(std::unique_ptr<GameAI::FSM::State>&& NewState)
{
	// TODO
	FSMInstance->AddState(std::move(NewState));
}

void UFSMComponent::AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool()> EvalFunc) const
{
	// TODO
	FSMInstance->AddTransition(From, To, EvalFunc);
}

void UFSMComponent::SetInitialState(GameAI::FSM::State* State)
{
	FSMInstance->SetInitialState(State);
}

// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// TODO
	if (bIsRunning && FSMInstance)
	{
		FSMInstance->Update(DeltaTime);
	}
}

void UFSMComponent::StartLogic()
{
	Super::StartLogic();

	// TODO
	bIsRunning = true;
}

void UFSMComponent::StopLogic(const FString& Reason)
{
	// TODO
	bIsRunning = false;
}

bool UFSMComponent::IsRunning() const
{
	return bIsRunning;
}

