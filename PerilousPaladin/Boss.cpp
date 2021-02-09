// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "FirstPersonCharacterTemplate.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.h"
#include "FirstPersonCharacterTemplate.h"
#include "Engine.h"
#include "NPC.h"
#include "RiseDoor.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABoss::ABoss() :
	Health(100.f), WidgetComponent(CreateAbstractDefaultSubobject<UWidgetComponent>(TEXT("HealthBar")))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("Enemy");
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UObject> BloodParticles(TEXT("Blueprint'/Game/MigrateFiles_Charlie/VFX/BloodSplatter/BloodSplatterBlueprint.BloodSplatterBlueprint'"));
	BloodEffect = BloodParticles.Object;

	//body
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodyMesh(TEXT("SkeletalMesh'/Game/Cameron/BossAnimations/Falling_Back_Death.Falling_Back_Death'"));
	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body->SetSkeletalMesh(BodyMesh.Object);
	
	//spot enemy anim
	static ConstructorHelpers::FObjectFinder<UAnimSequence> SpotEnemyAnimation(TEXT("AnimSequence'/Game/Cameron/BossAnimations/Standing_Taunt_Battlecry_Anim.Standing_Taunt_Battlecry_Anim'"));

	//Attack Anim
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAnimation(TEXT("AnimSequence'/Game/Cameron/BossAnimations/Standing_Melee_Attack_Downward_Anim.Standing_Melee_Attack_Downward_Anim'"));

	//Walk Anim
	static ConstructorHelpers::FObjectFinder<UAnimSequence> WalkAnimation(TEXT("AnimSequence'/Game/Cameron/BossAnimations/Walking_Anim.Walking_Anim'"));

	/** Set up collision triggers for SwordHitBox */
	//Register OnBeginOverlap function as an OnActorBeginOverlap delegate
	OnActorHit.AddDynamic(this, &ABoss::OnHit);

	CharacterTemplate = Cast<AFirstPersonCharacterTemplate>(GetWorld());

	bCanHit = true;
	CanSeePlayer = false;
	if (WidgetComponent) {
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -85000.0f));
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/James/HealthBar_BP"));
		if (WidgetClass.Succeeded()) {
			WidgetComponent->SetWidgetClass(WidgetClass.Class);
		}
	}
	tickCounter = 0;

	location = FVector(2680.000000, -1370.000000, 280.000000);
	rotation = FRotator(0, 0, 0);

	NumberOfEnemies = 1;
	Count = 0;
	CurrentDoor = NULL;

}

void ABoss::ResetHit()
{
	bCanHit = true;
	GetWorldTimerManager().ClearTimer(HitDelayTimerHandle);
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	UMaterialInstanceDynamic* const material_instance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	if (material_instance) {
		material_instance->SetVectorParameterValue("BodyColor", FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		GetMesh()->SetMaterial(0, material_instance);
	}
	SpawnObject(FVector(0,0,0), rotation);
	this->SetActorEnableCollision(false);
	this->SetActorLocation(FVector(2688.084229, -1360.000854, -50.000000));
	
}



// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), FoundActors);

	if (FoundActors.Num() < 2) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Boss Spawned")));
		this->SetActorLocation(FVector(2680.000000, -1370.000000, 280.000000));
		this->SetActorEnableCollision(true);
	}
	tickCounter++;
	RayCast();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Boss Spawned")));
	

	if (tickCounter < 300) {
		if (CanSeePlayer) {
			WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 85.0f));
		}
	}
	else {
		WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -850000.0f));
		tickCounter = 0;
	}
	if (Health <= 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Actor Death")));
		Die();
	}

	auto const uw = Cast<UHealthBar>(WidgetComponent->GetUserWidgetObject());
	if (uw) {
		uw->set_bar_value_percent(Health / MaxHealth);
	}
	


}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoss::Damage(float damage)
{
	this->Health -= (damage * this->ArmourModifier);
}

void ABoss::Die() {
	UBlueprint* GeneratedBP = Cast<UBlueprint>(BloodEffect);
	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
	Destroy();
}


APatrolPath* ABoss::GetPatrolPath()
{
	return patrol_path;
}


float ABoss::GetHealth() const
{
	return Health;
}

float ABoss::GetMaxHealth() const
{
	return MaxHealth;
}

void ABoss::SetHealth(float const newHealth)
{
	Health = newHealth;
}

void ABoss::SetCanSeePlayer()
{
	CanSeePlayer = false;
}

void ABoss::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	if (bCanHit == true) {
		bCanHit = false;
		if (OtherActor->ActorHasTag("Player")) {
			AFirstPersonCharacterTemplate* Player = Cast<AFirstPersonCharacterTemplate>(OtherActor);
			Player->Damage(this->DamageGiven);
		}
		GetWorld()->GetTimerManager().SetTimer(HitDelayTimerHandle, this, &ABoss::ResetHit, RecoveryTime, false);
	}
}


void ABoss::RayCast()
{
	/*
	FHitResult* OutHit = new FHitResult;

	FVector Start = this->GetActorLocation();
	Start.Z += 55;
	FVector ForwardVector = this->GetActorForwardVector();
	FVector EndTrace = ((ForwardVector * 100000.0f) * Start);
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*OutHit, Start, EndTrace, ECC_Visibility, *TraceParams)) {
		DrawDebugLine(GetWorld(), Start, EndTrace, FColor(255, 0, 0), true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You hit %s"), *OutHit->Actor->GetName()));
		if (OutHit->Actor->ActorHasTag("Player")) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("You hit Player")));
			CanSeePlayer = true;
		}
		else {
			CanSeePlayer = false;
		}
	}
	*/
	TArray<FHitResult> OutHits;

	FVector ActorLocation = this->GetActorLocation();

	FCollisionShape ColSphere = FCollisionShape::MakeSphere(500.0f);

	//DrawDebugSphere(GetWorld(), ActorLocation, ColSphere.GetSphereRadius(), 100, FColor::Red, false);

	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, this->GetActorLocation(), this->GetActorLocation(), FQuat::Identity, ECC_Pawn, ColSphere);
	if (isHit) {
		for (auto& Hit : OutHits) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *Hit.Actor->GetName()));

			if (Hit.Actor->ActorHasTag("Player")) {
				CanSeePlayer = true;
			}
			else {
				CanSeePlayer = false;
			}
		}

	}
	else {
		CanSeePlayer = false;
	}
	OutHits.Empty();
}

void ABoss::SpawnObject(FVector Loc, FRotator Rot)
{
	FActorSpawnParameters SpawnParams;
	ABoss* SpawnedActor = GetWorld()->SpawnActor<ABoss>(ActorToSpawn, Loc, Rot, SpawnParams);

	
		//ABoss* const SpawnedPickup = GetWorld()->SpawnActor<ABoss>(ActorToSpawn->GetDefaultObject()->GetClass(), Loc, Rot, SpawnParams);

}



