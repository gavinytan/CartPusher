// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereCartComponent.h"

// Sets default values for this component's properties
USphereCartComponent::USphereCartComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USphereCartComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USphereCartComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

