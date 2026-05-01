#pragma once
#include "DecisionMaking/FSM/FSMComponent.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

namespace GameAI::FSM
{
	class PatrolState : public State
	{
	public:
		void Enter() override
		{
			if (!Agent) return;
			if (!SeekBehavior)
				SeekBehavior = std::make_unique<Seek>();
			if (PatrolPoints.empty())
			{
				PatrolPoints = {
					FVector(0, 0, 0),
					FVector(500, 0, 0),
					FVector(500, 500, 0),
					FVector(0, 500, 0)
				};
			}
			
			FTargetData target;
			target.Position = FVector2D(
				PatrolPoints[CurrentIndex].X,
				PatrolPoints[CurrentIndex].Y
			);
			
			SeekBehavior->SetTarget(target);
			Agent->SetSteeringBehavior(SeekBehavior.get());
			
			UE_LOG(LogTemp, Warning, TEXT("Enter Patrol"));
		}
		
		virtual void Update(float DeltaTime) override
		{
			if (!Agent) return;
			if (PatrolPoints.empty()) return;
			
			float dist = FVector::Dist(
				Agent->GetActorLocation(),
				PatrolPoints[CurrentIndex]
			);

			if (dist < 100.f)
			{
				CurrentIndex = (CurrentIndex + 1) % PatrolPoints.size();

				FTargetData target;
				target.Position = FVector2D(
					PatrolPoints[CurrentIndex].X,
					PatrolPoints[CurrentIndex].Y
				);
				SeekBehavior->SetTarget(target);
			}
		}
		
		void Exit() override
		{
			UE_LOG(LogTemp, Warning, TEXT("Exit Patrol"));
		}
		
	private:
		std::vector<FVector> PatrolPoints;
		int CurrentIndex = 0;
	};
}
