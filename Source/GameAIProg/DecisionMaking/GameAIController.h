// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameAIController.generated.h"

UCLASS()
class GAMEAIPROG_API AGameAIController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|FSM")
	TObjectPtr<UBlackboardData> FSMBlackboardAsset; 
	
	// Sets default values for this actor's properties
	AGameAIController();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void RunFiniteStateMachine();
	
	// AI Perception
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void InitFiniteStateMachine();
	
	// Behavior Tree
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
	
	// AI Perception
	UAIPerceptionComponent* PerceptionComp;
	UAISenseConfig_Sight* SightConfig;
};
