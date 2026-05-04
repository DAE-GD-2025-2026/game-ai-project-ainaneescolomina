// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionMaking/Tasks/NextPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UNextPatrolPoint::UNextPatrolPoint()
{
	NodeName = "Get Next Patrol Point";
}

EBTNodeResult::Type UNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	TArray<AActor*> PatrolPoints;
	UGameplayStatics::GetAllActorsWithTag(Pawn->GetWorld(), FName("PatrolPoint"), PatrolPoints);

	if (PatrolPoints.Num() == 0) return EBTNodeResult::Failed;

	int32 Index = Blackboard->GetValueAsInt("PatrolIndex");
	if (Index >= PatrolPoints.Num()) Index = 0;

	Blackboard->SetValueAsObject(PatrolTargetKey.SelectedKeyName, PatrolPoints[Index]);

	Index = (Index + 1) % PatrolPoints.Num();
	Blackboard->SetValueAsInt("PatrolIndex", Index);

	return EBTNodeResult::Succeeded;
}