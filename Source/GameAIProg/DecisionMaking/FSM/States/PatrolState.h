#pragma once
#include "DecisionMaking/FSM/FSMComponent.h"

namespace GameAI::FSM
{
	class PatrolState : public State
	{
	public:
		virtual void Enter() override
		{
			UE_LOG(LogTemp, Warning, TEXT("Entering Idle"));
		}
		
		virtual void Update(float DeltaTime) override
		{
			// stuff
		}
		
		//void Exit() override;
	};
}
