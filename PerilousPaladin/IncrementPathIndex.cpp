// Fill out your copyright notice in the Description page of Project Settings.


#include "IncrementPathIndex.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC.h"
#include <cmath>

UIncrementPathIndex::UIncrementPathIndex(FObjectInitializer const& object_initializer) {
	NodeName = TEXT("Increment Path Index");
}

EBTNodeResult::Type UIncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	ANPC_AIController* const controller = Cast<ANPC_AIController>(owner_comp.GetAIOwner());
	ANPC* const npc = Cast<ANPC>(controller->GetPawn());
	int const no_of_points = npc->GetPatrolPath()->num();
	int const min_index = 0;
	int const max_index = no_of_points - 1;

	int index = controller->GetBlackboard()->GetValueAsInt("PatrolPathIndex");
	if (bidirectional) {
		if (index >= max_index && direction == EDirectionType::Forward) {
			direction = EDirectionType::Reverse;
		}

		else if (index == min_index && direction == EDirectionType::Reverse) {
			direction = EDirectionType::Forward;
		}
	}

	controller->GetBlackboard()->SetValueAsInt("PatrolPathIndex", (direction == EDirectionType::Forward ? std::abs(++index) : std::abs(--index)) % no_of_points);

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
