#pragma once
#include "DecisionMaking/FSM/FSMComponent.h"

namespace GameAI::FSM
{
	class ChaseState : public State
	{
	public:
		virtual void Enter() override
		{
			if (!Agent) return;
			if (!SeekBehavior) SeekBehavior = new Seek();
			
			FTargetData target;
			target.Position = Agent->GetPosition();
			
			SeekBehavior->SetTarget(target);
			Agent->SetSteeringBehavior(SeekBehavior);
			
			UE_LOG(LogTemp, Warning, TEXT("Enter Chase"));
		}

		virtual void Update(float DeltaTime) override
		{
			FTargetData target;
			target.Position = Agent->GetPosition();
			SeekBehavior->SetTarget(target);
		}
		
		void Exit() override
		{
			UE_LOG(LogTemp, Warning, TEXT("Exit Chase"));
		}
	};
}
