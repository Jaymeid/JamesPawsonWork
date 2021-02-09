// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_AIController.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT3_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()
public:
	ANPC_AIController(FObjectInitializer const& object_intitializer = FObjectInitializer::Get());
	void BeginPlay() override;
	void OnPossess(APawn* const pawn)override;
	class UBlackboardComponent* GetBlackboard() const;


private:
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	class UBlackboardComponent* Blackboard;

	class UAISenseConfig_Sight* sight_config;
	/*
	UFUNCTION()
	void on_updated(TArray<AActor*> const& updated_actors);
	*/
	UFUNCTION()
	void on_target_detected(AActor* actor, FAIStimulus const stimulus);


	void setup_perception_system();


};
