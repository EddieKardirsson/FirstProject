// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/** Static Mesh for the platform */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	class UStaticMeshComponent* Mesh;

	/** Starting point for the platform in local space */
	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	/** Ending point for the platform in local space */
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	/** The Verp speed for the vector interpolation between the points*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	float InterpTime;

	/** Timer for the platform's verping, pausing, reverse and repeat */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	FTimerHandle InterpTimer;

	/** Flagging if it's verping or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	bool bInterping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping();

	void SwapVectors(FVector& VecOne, FVector& VecTwo);
};
