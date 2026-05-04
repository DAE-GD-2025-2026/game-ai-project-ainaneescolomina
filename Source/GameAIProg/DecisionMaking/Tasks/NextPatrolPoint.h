// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class GAMEAIPROG_API UNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UNextPatrolPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolTargetKey;
};
