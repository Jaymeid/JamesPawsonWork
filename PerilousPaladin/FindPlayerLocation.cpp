// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "NPC_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
//#include "blackboard_keys.h"

UFindPlayerLocation::UFindPlayerLocation(FObjectInitializer const& object_initializer) {
	NodeName = TEXT("Find Player Location");
	search_random = false;
	search_radius = 150.0f;
}

EBTNodeResult::Type UFindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	//get player character and NPC's controller
	ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	auto const cont = Cast<ANPC_AIController>(owner_comp.GetAIOwner());

	//get player location to use as origin
	FVector const player_location = player->GetActorLocation();
	FVector const runAwayLocation = FVector((player_location.X * -1), (player_location.Y * -1), player_location.Z);
	if (search_random) {
		FNavLocation loc;
		//get nav system generate a random location around the player
		UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nav_sys->GetRandomPointInNavigableRadius(runAwayLocation, search_radius, loc, nullptr)) {
			cont->GetBlackboard()->SetValueAsVector("TargetLocation", loc.Location);
		}
	}
	else {
		cont->GetBlackboard()->SetValueAsVector("TargetLocation", player_location);
	}


	//finishing the task with success
	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}


/*if (nav_sys->GetRandomPointInNavigableRadius(player_location, search_radius, loc, nullptr)) {
			cont->GetBlackboard()->SetValueAsVector("TargetLocation", loc.Location);
		}
	}
	else {
		cont->GetBlackboard()->SetValueAsVector("TargetLocation", player_location);
	}*/