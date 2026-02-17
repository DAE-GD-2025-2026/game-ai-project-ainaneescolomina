
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	BlendedSteering.IsValid = false;
	
	for (const WeightedBehavior& behavior : WeightedBehaviors)
	{
		if (!behavior.pBehavior) continue;

		SteeringOutput steering = behavior.pBehavior->CalculateSteering(DeltaT, Agent);

		if (!steering.IsValid) continue;

		if (!steering.LinearVelocity.IsNearlyZero())
		{
			steering.LinearVelocity.Normalize();
		}
		
	// TODO: Calculate the weighted average steeringbehavior
		BlendedSteering.LinearVelocity += steering.LinearVelocity * behavior.Weight;
		BlendedSteering.LinearVelocity = BlendedSteering.LinearVelocity.GetClampedToMaxSize(Agent.GetMaxLinearSpeed());
		BlendedSteering.AngularVelocity += steering.AngularVelocity * behavior.Weight;

		BlendedSteering.IsValid = true;
	}
	
	// TODO: Add debug drawing

	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = std::find_if(WeightedBehaviors.begin(),
		WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(it!= WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}