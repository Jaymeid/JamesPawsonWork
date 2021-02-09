// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiseDoor.generated.h"

UCLASS()
class MYPROJECT3_API ARiseDoor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ARiseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class UStaticMeshComponent* Door;

	// declare sphere comp
	UPROPERTY(VisibleAnywhere, Category = "BoxComp")
	class UBoxComponent* BoxComp;

	bool Opening = false;
	bool Closing = false;
	bool isClosed = true;

	float DotP = 0.f;
	float MaxDegree = 0.f;
	float AddRotation = 0.f;
	float PosNeg = 0.f;
	float DoorCurrentRotation = 0.f;
	float CurrentRise = 0.f;
	FVector DoorCurrentLocation;
	class ANPC* npc;


	bool CanOpenDoor = false;

	UFUNCTION()
		void OpenDoor(float dt);

	UFUNCTION()
		void CloseDoor(float dt);

	UFUNCTION()
		void ToggleDoor(FVector ForwardVector);

};
