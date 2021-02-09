// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "NPC.h"
#include "ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT3_API UChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UChasePlayer(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory) override;

};
