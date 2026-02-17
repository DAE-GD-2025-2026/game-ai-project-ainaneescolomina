#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};

	FVector2D LinearVelocity(pFlock->GetAverageNeighborPos() - pAgent.GetPosition());
	Steering.LinearVelocity = LinearVelocity;

	return Steering;
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
    SteeringOutput Steering{};
    FVector2D LinearVelocity = FVector2D::ZeroVector;

    for (int i = 0; i < pFlock->GetNrOfNeighbors(); ++i)
    {
        ASteeringAgent* pNeighbor = pFlock->GetNeighbors()[i];
        FVector2D flee = pAgent.GetPosition() - pNeighbor->GetPosition();
        float distance = flee.Length();

        if (distance > 0.001f)
        {
            flee.Normalize();
            float impact = 1.0f / distance;
            LinearVelocity += flee * impact;
        }
    }

    Steering.LinearVelocity = LinearVelocity;
    return Steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};

    FVector2D LinearVelocity = pFlock->GetAverageNeighborVelocity();

    if (!LinearVelocity.IsNearlyZero())
    {
        Steering.LinearVelocity = LinearVelocity;
    }

	return Steering;
}
