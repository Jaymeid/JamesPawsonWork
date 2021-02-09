// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirstPersonCharacterTemplate.h"
#include "PatrolPath.h"
#include "Boss.generated.h"

UCLASS()
class MYPROJECT3_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float ArmourModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float DamageGiven = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float RecoveryTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ABoss> ActorToSpawn;

	/** Attack Sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* AttackSound1;

	/** Take Damage Sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* TakeDamageSound1;

	/** Body */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Body;

	/** Battle Cry Animation */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UAnimSequence* BattleCryAnim;

	/** Walking Animation */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UAnimSequence* WalkAnim;

	/** Atack Animation */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UAnimSequence* AttackAnim;

	class UObject* BloodEffect;

	/** True if the AI can hit, flase otherwise*/
	bool bCanHit;

	/** Handles the dalay between hits*/
	FTimerHandle HitDelayTimerHandle;

	/** Restets the AI's ability to hit*/
	void ResetHit();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RayCast();



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void SpawnObject(FVector Loc, FRotator Rot);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Damage(float damage);

	void Die();
	

	//The delegate function for handling a hit event
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	APatrolPath* GetPatrolPath();


	AFirstPersonCharacterTemplate* CharacterTemplate;

	float GetHealth() const;
	float GetMaxHealth() const;
	void SetHealth(float const newHealth);
	void SetCanSeePlayer();
	bool CanSeePlayer;

	void SpawnInSecondRoom();

	class ARiseDoor* CurrentDoor;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		APatrolPath* patrol_path;

	class UWidgetComponent* WidgetComponent;

	FTimerHandle HealthBarHandle;
	int tickCounter;

	FVector location;
	FRotator rotation;

	int NumberOfEnemies;
	int Count;

	TArray<AActor*> DeadActors;

};
