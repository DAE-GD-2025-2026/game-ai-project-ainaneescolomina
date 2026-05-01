#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTask.generated.h"

UCLASS()
class YOURGAME_API UPatrolBTTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPatrolBTTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
