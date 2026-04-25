#pragma once
#include "DecisionMaking/FSM/FSMComponent.h"

namespace GameAI::FSM
{
	class MoveState : public State
	{
	public:
		virtual void Enter() override
		{
			UE_LOG(LogTemp, Warning, TEXT("Start Moving"));
		}

		virtual void Update(float DeltaTime) override
		{
			// move actor
		}
		
		//void Exit() override;
	};
}
