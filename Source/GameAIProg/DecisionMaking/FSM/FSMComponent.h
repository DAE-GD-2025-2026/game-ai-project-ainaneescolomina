// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <memory>

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "FSMComponent.generated.h"

namespace GameAI::FSM
{
	class Transition;

	class State
	{
	public:
		virtual ~State() = default;

		void SetAgent(ASteeringAgent* InAgent) { Agent = InAgent; }
		
		virtual void Enter() {};
		virtual void Update(float DeltaTime) {};
		virtual void Exit() {};
		
		std::vector<Transition> Transitions;
		
	protected:
		ASteeringAgent* Agent = nullptr;
		ISteeringBehavior* SeekBehavior = nullptr;
	};
	
	class Transition
	{
	public:
		State* To;
		std::function<bool()> Condition;
	};
	
	class FSM // contains FSM logic
	{
	public:
		void AddState(std::unique_ptr<State>&& NewState);
		void AddTransition(State* From, State* To, std::function<bool()> Condition);

		void Update(float DeltaTime);
		void SetInitialState(State* State);

	private:
		std::vector<std::unique_ptr<State>> States;
		State* CurrentState = nullptr;
	};
}

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEAIPROG_API UFSMComponent : public UBrainComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFSMComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void StartLogic() override;
	virtual void StopLogic(const FString& Reason) override;
	
	virtual bool IsRunning() const override; 
	
	void AddState(std::unique_ptr<GameAI::FSM::State>&& NewState);
	void AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool()> EvalFunc) const;

	void SetInitialState(GameAI::FSM::State* State);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	std::unique_ptr<GameAI::FSM::FSM> FSMInstance;
	bool bIsRunning{false};
};
