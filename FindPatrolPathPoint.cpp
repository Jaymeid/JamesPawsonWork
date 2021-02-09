// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPatrolPathPoint.h"
#include "NPC_AIController.h"
#include"BehaviorTree/BlackboardComponent.h"
#include "NPC.h"

UFindPatrolPathPoint::UFindPatrolPathPoint(FObjectInitializer const& object_initializer) {
	NodeName = TEXT("Find Patrol Path Point");
}

EBTNodeResult::Type UFindPatrolPathPoint::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory) {
	ANPC_AIController* const controller = Cast<ANPC_AIController>(owner_comp.GetAIOwner());

	int const index = controller->GetBlackboard()->GetValueAsInt("PatrolPathIndex");

	ANPC* const npc = Cast<ANPC>(controller->GetPawn());
	FVector const point = npc->GetPatrolPath()->GetPatrolPoint(index);

	//transform from local vector relative to parent object to a global positioning
	FVector const global_point = npc->GetPatrolPath()->GetActorTransform().TransformPosition(point);

	controller->GetBlackboard()->SetValueAsVector("PatrolPathVector", global_point);

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}