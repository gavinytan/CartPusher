// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SphereCartActor.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTPUSHER_API USphereCartActor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USphereCartActor();
	UPROPERTY(BlueprintReadWrite)
	bool isAttached = false;
	UPROPERTY(BlueprintReadWrite)
	AActor* cartAttachedToFront;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void AttachActorsWithPhysicsConstraint(AActor* Actor1, AActor* Actor2, UPhysicsConstraintComponent* Constraint);

	UFUNCTION(BlueprintCallable)
	void RemovePhysicsConstraints();

	UFUNCTION(BlueprintCallable)
	void EnableFrontCollision();

	UFUNCTION(BlueprintCallable)
	void DisableBackCollision();

	UFUNCTION(BlueprintCallable)
	void EnableBackCollision();

	// Overlap
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverOverlapBeginAudio(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEndAudio(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
