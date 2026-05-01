#pragma once
#include "DecisionMaking/FSM/FSMComponent.h"

namespace GameAI::FSM
{
	class ChaseState : public State
	{
	public:
		void Enter() override
		{
			if (!Agent || !TargetAgent) return;
			if (!SeekBehavior)
				SeekBehavior = std::make_unique<Seek>();

			FTargetData target;
			target.Position = FVector2D(
				TargetAgent->GetActorLocation().X,
				TargetAgent->GetActorLocation().Y
			);

			SeekBehavior->SetTarget(target);
			Agent->SetSteeringBehavior(SeekBehavior.get());

			UE_LOG(LogTemp, Warning, TEXT("Enter Chase"));
		}

		void Update(float DeltaTime) override
		{
			if (!TargetAgent) return;

			FTargetData target;
			target.Position = FVector2D(
				TargetAgent->GetActorLocation().X,
				TargetAgent->GetActorLocation().Y
			);

			SeekBehavior->SetTarget(target);
		}
		
		void Exit() override
		{
			UE_LOG(LogTemp, Warning, TEXT("Exit Chase"));
		}
		
		void SetTarget(ASteeringAgent* InTarget)
		{
			TargetAgent = InTarget;
		}
		
	private:
		ASteeringAgent* TargetAgent = nullptr;
	};
}
