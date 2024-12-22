// Fill out your copyright notice in the Description page of Project Settings.


#include "CartHomeActor.h"
#include "SphereCartActor.h"
#include "GameFramework/Actor.h"
#include "Engine/GameEngine.h"
#include "Components/StaticMeshComponent.h"
#include <PhysicsEngine/PhysicsConstraintComponent.h>
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UCartHomeActor::UCartHomeActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCartHomeActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event to the OnOverlapBegin function
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("Collision"))[0]);
	if (Box) {
		Box->OnComponentBeginOverlap.AddDynamic(this, &UCartHomeActor::OnOverlapBegin);
	}
	else {
		//UE_LOG(LogFooBar, Warning, TEXT("%hs() => Could not find box collision component on parent!"), __func__);
	}
	
}


void UCartHomeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (GEngine) {
		if (OtherActor->GetClass()->GetName() == "SphereCart_C") {
			count += 1;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%lld Cart(s) Collected"), count));
			OtherActor->Destroy();
		}
	}
	
}

// Called every frame
void UCartHomeActor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

