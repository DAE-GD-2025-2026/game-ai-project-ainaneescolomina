#pragma once
#include "DecisionMaking/FSM/FSMComponent.h"

namespace GameAI::FSM
{
	class SearchState : public State
	{
	public:
		virtual void Enter() override
		{
			if (!Agent) return;
			if (!SeekBehavior) SeekBehavior = new Seek();
			
			FTargetData target;
			target.Position = LastKnownPosition;

			SeekBehavior->SetTarget(target);
			Agent->SetSteeringBehavior(SeekBehavior);
			
			StartSearchTime = Agent->GetWorld()->GetTimeSeconds();
			
			UE_LOG(LogTemp, Warning, TEXT("Enter Search"));
		}

		virtual void Update(float DeltaTime) override
		{
			// move actor
		}
		
		void Exit() override
		{
			UE_LOG(LogTemp, Warning, TEXT("Exit Search"));
		}
		
		float GetElapsedTime() const
		{
			if (!Agent) return 0.f;
			return Agent->GetWorld()->GetTimeSeconds() - StartSearchTime;
		}
		
		void SetLastKnownPosition(FVector2D lastKnownPosition) {LastKnownPosition = lastKnownPosition;};
		
	private:
		FVector2D LastKnownPosition;
		float StartSearchTime = 0.f;
	};
}

