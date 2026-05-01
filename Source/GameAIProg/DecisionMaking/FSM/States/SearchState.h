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
			if (!SeekBehavior)
				SeekBehavior = std::make_unique<Seek>();
			
			FTargetData target;
			target.Position = LastKnownPosition;

			SeekBehavior->SetTarget(target);
			Agent->SetSteeringBehavior(SeekBehavior.get());
			
			StartSearchTime = Agent->GetWorld()->GetTimeSeconds();
			
			UE_LOG(LogTemp, Warning, TEXT("Enter Search"));
		}

		virtual void Update(float DeltaTime) override
		{
			float dist = FVector::Dist(
				Agent->GetActorLocation(),
				FVector(LastKnownPosition.X, LastKnownPosition.Y, 0)
			);

			if (dist < 50.f)
			{
				// change state?
			}
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

