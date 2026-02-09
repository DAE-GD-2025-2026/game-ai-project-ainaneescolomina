#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	//Steering.LinearVelocity.Normalize(); --> Already automatically normalized by AssMovementInput

	// Add debug rendering for grades :)
	// Lines and circle guides

	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Agent.GetPosition() - Target.Position;

	return Steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	if(InitialAgentMaxSpeed <= 0) InitialAgentMaxSpeed = Agent.GetMaxLinearSpeed();
	
	SteeringOutput Steering{};
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();

	FVector CenterPos(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector TargetPos(Target.Position.X, Target.Position.Y, 0);
	float TargetDistance = FVector::Distance(CenterPos, TargetPos);
	float SpeedDecrease = SlowRadius / TargetRadius;

	if (TargetDistance < TargetRadius)
		Agent.SetMaxLinearSpeed(0);
	else if (TargetDistance < SlowRadius)
		Agent.SetMaxLinearSpeed(Agent.GetMaxLinearSpeed() - SpeedDecrease);
	else
		Agent.SetMaxLinearSpeed(InitialAgentMaxSpeed);

	// Debug Rendering 
	DrawDebugCircle(Agent.GetWorld(), CenterPos, SlowRadius, 50, FColor::Blue, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	DrawDebugCircle(Agent.GetWorld(), CenterPos, TargetRadius, 50, FColor::Orange, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D LinearVelocity = Target.Position - Agent.GetPosition();

	float DesiredOrientation = FMath::Atan2(LinearVelocity.Y, LinearVelocity.X);
	float CurrentOrientation = Agent.GetAngularVelocity();

	float RotationDelta = DesiredOrientation - CurrentOrientation;
	RotationDelta = FMath::UnwindRadians(RotationDelta);

	Steering.AngularVelocity = RotationDelta;

	return Steering;
}
