// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_AIController.h"
#include "FirstPersonCharacterTemplate.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
//#include "EnemyAICharacter.h"
#include "NPC.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
//#include "blackboard_keys.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& object_intitializer) {
	BehaviorTreeComponent = object_intitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	Blackboard = object_intitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
	setup_perception_system();
}

void ANPC_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void ANPC_AIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
	ANPC* AIPawn = Cast<ANPC>(pawn);
	if (AIPawn) {
		if (AIPawn->BehaviorTree->BlackboardAsset) {
			Blackboard->InitializeBlackboard(*AIPawn->BehaviorTree->BlackboardAsset);
		}
		BehaviorTreeComponent->StartTree(*AIPawn->BehaviorTree);
	}
}

UBlackboardComponent* ANPC_AIController::GetBlackboard()const
{
	return Blackboard;
}

void ANPC_AIController::on_target_detected(AActor* actor, FAIStimulus const stimulus) {
	if (auto const ch = Cast<AFirstPersonCharacterTemplate>(actor)) {
		GetBlackboard()->SetValueAsBool("CanSeePlayer", stimulus.WasSuccessfullySensed());
	}
}

void ANPC_AIController::setup_perception_system()
{
	//create config object
	sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight_Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
	sight_config->SightRadius = 50000.0f;
	sight_config->LoseSightRadius = sight_config->SightRadius + 50.0f;
	sight_config->PeripheralVisionAngleDegrees = 90.0f;
	sight_config->SetMaxAge(5.0f);
	sight_config->AutoSuccessRangeFromLastSeenLocation = 900.0f;
	sight_config->DetectionByAffiliation.bDetectEnemies = true;
	sight_config->DetectionByAffiliation.bDetectFriendlies = true;
	sight_config->DetectionByAffiliation.bDetectNeutrals = true;

	//adding sight config to perception component

	GetPerceptionComponent()->SetDominantSense(*sight_config->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::on_target_detected);
	GetPerceptionComponent()->ConfigureSense(*sight_config);
}
