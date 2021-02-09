// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
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
#include "Boss.h"
#include "RiseDoor.h"
#include "HealthPickup.h"
#include "AmmoPickup.h"
#include "DrawDebugHelpers.h"


// Sets default values
ANPC::ANPC() :
	Health(100.f), WidgetComponent(CreateAbstractDefaultSubobject<UWidgetComponent>(TEXT("HealthBar")))
{
	this->Tags.Add("Enemy");
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UObject> BloodParticles(TEXT("Blueprint'/Game/MigrateFiles_Charlie/VFX/BloodSplatter/BloodSplatterBlueprint.BloodSplatterBlueprint'"));
	BloodEffect = BloodParticles.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> TakeDamage(TEXT("SoundCue'/Game/Audio/EnemyDamage_Cue.EnemyDamage_Cue'"));
	TakeDamageSound1 = TakeDamage.Object;

	static ConstructorHelpers::FObjectFinder<UObject> HealthPack(TEXT("Blueprint'/Game/Cameron/HealthPickupBP.HealthPickupBP'"));
	HealthPickup = HealthPack.Object;
	static ConstructorHelpers::FObjectFinder<UObject> AmmoPack(TEXT("Blueprint'/Game/Cameron/AmmoPickupBP.AmmoPickupBP'"));
	AmmoPickup = AmmoPack.Object;

	/** Set up collision triggers for SwordHitBox */
	//Register OnBeginOverlap function as an OnActorBeginOverlap delegate
	OnActorHit.AddDynamic(this, &ANPC::OnHit);

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

	location = FVector(1460.000000f,-1129.999756f, 288.00012f);
	rotation = FRotator(0, 0, 0);

	NumberOfEnemies = 1;
	Count = 0;
	CurrentDoor = NULL;
}



void ANPC::ResetHit()
{
	bCanHit = true;
	GetWorldTimerManager().ClearTimer(HitDelayTimerHandle);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	UMaterialInstanceDynamic* const material_instance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	if (material_instance) {
		material_instance->SetVectorParameterValue("BodyColor", FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		GetMesh()->SetMaterial(0, material_instance);
	}
	//SpawnObject(location, rotation);
}



// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	tickCounter++;
	RayCast();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), FoundActors);
	if (Count < 1) {
		SpawnObject(location, rotation);
		Count++;
	}

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
	if (FoundActors.Num() < 5) {
		SpawnInSecondRoom();
	}
	

}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::Damage(float damage)
{
	this->Health -= (damage * this->ArmourModifier);
	if (TakeDamageSound1 != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, TakeDamageSound1, GetActorLocation());
	}
}

void ANPC::Die() {
	AFirstPersonCharacterTemplate* player = Cast<AFirstPersonCharacterTemplate>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	player->AddHealth(5.f);
	UBlueprint* GeneratedBP = Cast<UBlueprint>(BloodEffect);
	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	int num = FMath::FRandRange(0, 8);
	UBlueprint* GeneratedBPHP = Cast<UBlueprint>(HealthPickup);
	UBlueprint* GeneratedBPAP = Cast<UBlueprint>(AmmoPickup);
	if (this->ActorHasTag("Boss")) {
		player->StartHealthPowerup();
	}
	if (num == 0) {
		World->SpawnActor<AActor>(GeneratedBPHP->GeneratedClass, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
	}
	if (num == 1 || num == 2) {
		World->SpawnActor<AActor>(GeneratedBPAP->GeneratedClass, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
	}
	World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
	Destroy();
}


APatrolPath* ANPC::GetPatrolPath()
{
	return patrol_path;
}


float ANPC::GetHealth() const
{
	return Health;
}

float ANPC::GetMaxHealth() const
{
	return MaxHealth;
}

void ANPC::SetHealth(float const newHealth)
{
	Health = newHealth;
}

void ANPC::SetCanSeePlayer()
{
	CanSeePlayer = false;
}

void ANPC::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	if (bCanHit == true) {
		bCanHit = false;
		if (OtherActor->ActorHasTag("Player")) {
			AFirstPersonCharacterTemplate* Player = Cast<AFirstPersonCharacterTemplate>(OtherActor);
			Player->Damage(this->DamageGiven);
		}
		GetWorld()->GetTimerManager().SetTimer(HitDelayTimerHandle, this, &ANPC::ResetHit, RecoveryTime, false);
	}
}


void ANPC::RayCast()
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

void ANPC::SpawnObject(FVector Loc, FRotator Rot)
{
	FActorSpawnParameters SpawnParams;
	ANPC* SpawnedActor = GetWorld()->SpawnActor<ANPC>(ActorToSpawn, Loc, Rot, SpawnParams);
	FVector NewLoc = FVector(5349.585449, -6305.435547, 351.816040);
	ANPC* SecondRoom = GetWorld()->SpawnActor<ANPC>(ActorToSpawn, NewLoc, Rot, SpawnParams);
	FVector thirdLoc = FVector(5260.000000, -440.000000, 300.000000);
	ANPC* ThirdRoom = GetWorld()->SpawnActor<ANPC>(ActorToSpawn, thirdLoc, Rot, SpawnParams);
	
}

void ANPC::SpawnInSecondRoom() {
	
}



