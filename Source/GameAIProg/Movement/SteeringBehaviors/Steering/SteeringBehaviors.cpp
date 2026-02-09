#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D LinearVelocity(Target.Position - Agent.GetPosition());
	Steering.LinearVelocity = LinearVelocity;
	//Steering.LinearVelocity.Normalize(); --> Already automatically normalized by AssMovementInput

	// Debug Rendering
	const float DebugScale = 0.4f;
	UWorld* World = Agent.GetWorld();

	FVector Start(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector2D CurrentVel2D = Target.Position;
	FVector SteeringVel = Agent.GetVelocity();
	FVector2D DesiredVel2D = LinearVelocity;

	// -- current velocity
	DrawDebugLine(World, Start, Start + FVector(CurrentVel2D.X, CurrentVel2D.Y, 0) * DebugScale, FColor::Magenta, false, -1, 0, 5);

	// -- desired velocity
	DrawDebugLine(World, Start, Start + FVector(DesiredVel2D.X, DesiredVel2D.Y, 0) * DebugScale, FColor::Green, false, -1, 0, 5);

	// -- (desired - current) velocity
	DrawDebugLine(World, Start, Start + FVector(SteeringVel.X, SteeringVel.Y, 0) * DebugScale, FColor::Cyan, false, -1, 0, 5);

	// -- target circle
	DrawDebugCircle(Agent.GetWorld(), FVector(CurrentVel2D.X, CurrentVel2D.Y, 0), 10, 50, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- area circle
	DrawDebugCircle(Agent.GetWorld(), Start, 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D LinearVelocity(Agent.GetPosition() - Target.Position);
	Steering.LinearVelocity = LinearVelocity;

	// Debug Rendering
	const float DebugScale = 0.4f;
	UWorld* World = Agent.GetWorld();

	FVector Start(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector2D CurrentVel2D = Target.Position;
	FVector SteeringVel = Agent.GetVelocity();
	FVector2D DesiredVel2D = LinearVelocity;

	// -- current velocity
	DrawDebugLine(World, Start, Start + FVector(CurrentVel2D.X, CurrentVel2D.Y, 0) * DebugScale, FColor::Magenta, false, -1, 0, 5);

	// -- desired velocity
	DrawDebugLine(World, Start, Start + FVector(DesiredVel2D.X, DesiredVel2D.Y, 0) * DebugScale, FColor::Green, false, -1, 0, 5);

	// -- (desired - current) velocity
	DrawDebugLine(World, Start, Start + FVector(SteeringVel.X, SteeringVel.Y, 0) * DebugScale, FColor::Cyan, false, -1, 0, 5);

	// -- target circle
	DrawDebugCircle(Agent.GetWorld(), FVector(CurrentVel2D.X, CurrentVel2D.Y, 0), 10, 50, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- area circle
	DrawDebugCircle(Agent.GetWorld(), Start, 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	if(InitialAgentMaxSpeed <= 0) InitialAgentMaxSpeed = Agent.GetMaxLinearSpeed();
	
	SteeringOutput Steering{};

	FVector2D LinearVelocity(Target.Position - Agent.GetPosition());
	Steering.LinearVelocity = LinearVelocity;

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
	const float DebugScale = 0.4f;
	UWorld* World = Agent.GetWorld();

	FVector Start(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector2D CurrentVel2D = Target.Position;
	FVector SteeringVel = Agent.GetVelocity();
	FVector2D DesiredVel2D = LinearVelocity;

	// -- current velocity
	DrawDebugLine(World, Start, Start + FVector(CurrentVel2D.X, CurrentVel2D.Y, 0) * DebugScale, FColor::Magenta, false, -1, 0, 5);

	// -- desired velocity
	DrawDebugLine(World, Start, Start + FVector(DesiredVel2D.X, DesiredVel2D.Y, 0) * DebugScale, FColor::Green, false, -1, 0, 5);

	// -- (desired - current) velocity
	DrawDebugLine(World, Start, Start + FVector(SteeringVel.X, SteeringVel.Y, 0) * DebugScale, FColor::Cyan, false, -1, 0, 5);

	// -- target circle
	DrawDebugCircle(Agent.GetWorld(), FVector(CurrentVel2D.X, CurrentVel2D.Y, 0), 10, 50, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- area cirecle
	DrawDebugCircle(Agent.GetWorld(), Start, 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- effect circle
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

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D AgentPos = Agent.GetPosition();
	FVector2D TargetPos = Target.Position;

	float Distance = FVector2D::Distance(AgentPos, TargetPos);

	float TimeToTarget = Distance / Agent.GetMaxLinearSpeed();
	TimeToTarget = FMath::Clamp(TimeToTarget, 0.f, 2.f);

	FVector2D PredictedTargetPos = Target.Position + Target.LinearVelocity * TimeToTarget;
	
	FVector2D LinearVelocity(PredictedTargetPos - AgentPos);
	Steering.LinearVelocity = LinearVelocity;

	// Debug Rendering
	const float DebugScale = 0.4f;
	UWorld* World = Agent.GetWorld();

	FVector Start(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector2D CurrentVel2D = Target.Position;
	FVector SteeringVel = Agent.GetVelocity();
	FVector2D DesiredVel2D = LinearVelocity;

	// -- current velocity
	DrawDebugLine(World, Start, Start + FVector(CurrentVel2D.X, CurrentVel2D.Y, 0) * DebugScale, FColor::Magenta, false, -1, 0, 5);

	// -- desired velocity
	DrawDebugLine(World, Start, Start + FVector(DesiredVel2D.X, DesiredVel2D.Y, 0) * DebugScale, FColor::Green, false, -1, 0, 5);

	// -- (desired - current) velocity
	DrawDebugLine(World, Start, Start + FVector(SteeringVel.X, SteeringVel.Y, 0) * DebugScale, FColor::Cyan, false, -1, 0, 5);

	// -- predicted circle
	DrawDebugCircle(Agent.GetWorld(), FVector(PredictedTargetPos.X, PredictedTargetPos.Y, 0), 10, 50, FColor::Magenta, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- area circle
	DrawDebugCircle(Agent.GetWorld(), Start, 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D AgentPos = Agent.GetPosition();
	FVector2D TargetPos = Target.Position;

	float Distance = FVector2D::Distance(AgentPos, TargetPos);

	float TimeToTarget = Distance / Agent.GetMaxLinearSpeed();
	TimeToTarget = FMath::Clamp(TimeToTarget, 0.f, 2.f);

	FVector2D PredictedTargetPos = Target.Position + Target.LinearVelocity * TimeToTarget;

	FVector2D LinearVelocity(AgentPos - PredictedTargetPos);
	Steering.LinearVelocity = LinearVelocity;

	// Debug Rendering
	const float DebugScale = 0.4f;
	UWorld* World = Agent.GetWorld();

	FVector Start(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector2D CurrentVel2D = Target.Position;
	FVector SteeringVel = Agent.GetVelocity();
	FVector2D DesiredVel2D = LinearVelocity;

	// -- current velocity
	DrawDebugLine(World, Start, Start + FVector(CurrentVel2D.X, CurrentVel2D.Y, 0) * DebugScale, FColor::Magenta, false, -1, 0, 5);

	// -- desired velocity
	DrawDebugLine(World, Start, Start + FVector(DesiredVel2D.X, DesiredVel2D.Y, 0) * DebugScale, FColor::Green, false, -1, 0, 5);

	// -- (desired - current) velocity
	DrawDebugLine(World, Start, Start + FVector(SteeringVel.X, SteeringVel.Y, 0) * DebugScale, FColor::Cyan, false, -1, 0, 5);

	// -- predicted circle
	DrawDebugCircle(Agent.GetWorld(), FVector(PredictedTargetPos.X, PredictedTargetPos.Y, 0), 10, 50, FColor::Magenta, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- area circle
	DrawDebugCircle(Agent.GetWorld(), Start, 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D AgentPos = Agent.GetPosition();
	FVector2D Forward = Target.Position;

	FVector2D CircleCenter = AgentPos + Forward * WanderDistanceFromCenter;
	float RandomOffset = FMath::FRandRange(-0.5f, 0.5f);
	WanderAngle += RandomOffset;

	FVector2D TargetOffset = FVector2D(FMath::Cos(WanderAngle), FMath::Sin(WanderAngle)) * WanderRadius;
	FVector2D WanderTarget = CircleCenter + TargetOffset;

	FVector2D LinearVelocity(WanderTarget - AgentPos);
	Steering.LinearVelocity = LinearVelocity;

	// Debug Rendering
	const float DebugScale = 0.4f;
	UWorld* World = Agent.GetWorld();

	FVector Start(Agent.GetPosition().X, Agent.GetPosition().Y, 0);
	FVector2D CurrentVel2D = Target.Position;
	FVector SteeringVel = Agent.GetVelocity();
	FVector2D DesiredVel2D = LinearVelocity;

	// -- current velocity
	DrawDebugLine(World, Start, Start + FVector(CurrentVel2D.X, CurrentVel2D.Y, 0) * DebugScale, FColor::Magenta, false, -1, 0, 5);

	// -- desired velocity
	DrawDebugLine(World, Start, Start + FVector(DesiredVel2D.X, DesiredVel2D.Y, 0) * DebugScale, FColor::Green, false, -1, 0, 5);

	// -- (desired - current) velocity
	DrawDebugLine(World, Start, Start + FVector(SteeringVel.X, SteeringVel.Y, 0) * DebugScale, FColor::Cyan, false, -1, 0, 5);

	// -- wander target circle
	DrawDebugCircle(Agent.GetWorld(), FVector(WanderTarget.X, WanderTarget.Y, 0), 5, 50, FColor::Green, false, -1.f, 0, 10.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- area circle
	DrawDebugCircle(Agent.GetWorld(), Start, 40, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	// -- wander circle
	DrawDebugCircle(Agent.GetWorld(), FVector(CircleCenter.X, CircleCenter.Y, 0.f), WanderRadius, 50, FColor::Yellow, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Steering;
}
