// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "NPC_AIController.h"
//#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
//#include "blackboard_keys.h"


UFindRandomLocation::UFindRandomLocation(FObjectInitializer const& object_initializer) {
	NodeName = TEXT("Find Random Location");
	search_radius = 1500.0f;
}

EBTNodeResult::Type UFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{

	//get the ai controller and npc

	auto const cont = Cast<ANPC_AIController>(owner_comp.GetAIOwner());
	auto const npc = cont->GetPawn();

	//get pawn location of npc use as an origin

	FVector const origin = npc->GetActorLocation();
	FNavLocation loc;

	//get the nav system and generate randomg location on the nav mesh
	UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (nav_sys->GetRandomPointInNavigableRadius(origin, search_radius, loc, nullptr)) {
		cont->GetBlackboard()->SetValueAsVector("TargetLocation", loc.Location);
	}

	//finish the task with sucess

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
