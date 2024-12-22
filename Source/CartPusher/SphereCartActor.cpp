// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereCartActor.h"
#include "GameFramework/Actor.h"
#include "Engine/GameEngine.h"
#include "Components/StaticMeshComponent.h"
#include <PhysicsEngine/PhysicsConstraintComponent.h>
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"


// Sets default values for this component's properties
USphereCartActor::USphereCartActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	// ...
}


// Called when the game starts
void USphereCartActor::BeginPlay()
{
	Super::BeginPlay();
	isAttached = false; //indicates that this cart's front is linked to another's back
	// ...
	if (true) {


	}

	// Bind the overlap event to the OnOverlapBegin function
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontCollision"))[0]);
	UBoxComponent* AudioBox = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("AudioCollision"))[0]);
	if (Box) {
		Box->OnComponentBeginOverlap.AddDynamic(this, &USphereCartActor::OnOverlapBegin);
	}
	if (AudioBox) {
		AudioBox->OnComponentBeginOverlap.AddDynamic(this, &USphereCartActor::OverOverlapBeginAudio);
		AudioBox->OnComponentEndOverlap.AddDynamic(this, &USphereCartActor::OnOverlapEndAudio);
	}
	else {
		//UE_LOG(LogFooBar, Warning, TEXT("%hs() => Could not find box collision component on parent!"), __func__);
	}

	
}
void USphereCartActor::OnOverlapEndAudio(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { //Overlap end for the audio hitbox, currently not in use
	if (OtherActor->GetClass()->GetName() == "BP_FirstPersonCharacter_C") {

	}
}

void USphereCartActor::OverOverlapBeginAudio(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { //Overlap begin for the audio hitbox, currently not in use
	if (OtherActor->GetClass()->GetName() == "BP_FirstPersonCharacter_C") {

	}
}


void USphereCartActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ //this creates links the back cart to the front cart, constraint belongs to the back cart, this is because the code is triggered only by the front collision
	if (GEngine) {

		if (OtherActor->GetClass() == GetOwner()->GetClass() && isAttached==false && GetOwner()!=OtherActor && OtherComp->ComponentHasTag(FName("BackCollision")) == true) { //if the cart collides with another cart and it isn't already attached to it [Result[i]->IsAttachedTo(GetOwner())==false]
			isAttached = true; //disables the current cart's front collision

			USphereCartActor* cartInFront = (USphereCartActor*)(OtherActor->GetComponentByClass(USphereCartActor::StaticClass())); //disables the other carts back collision

			UAudioComponent* attachSound = Cast<UAudioComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("AttachAudio"))[0]); //sound for the attachment
			attachSound->Play();




			cartInFront->DisableBackCollision();
			cartAttachedToFront = OtherActor; //record the attached cart


			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, OtherActor->GetClass()->GetName());



			UPhysicsConstraintComponent* PhysicsConst = NewObject<UPhysicsConstraintComponent>(this, TEXT("PhysicsConstraint"));
			PhysicsConst->ComponentTags.Add(FName("CartLink"));
			PhysicsConst->SetupAttachment(GetOwner()->GetRootComponent());
			PhysicsConst->RegisterComponent();
			AttachActorsWithPhysicsConstraint(GetOwner(), OtherActor, PhysicsConst);
			

		
		}


	}
}

void USphereCartActor::AttachActorsWithPhysicsConstraint(AActor* Actor1, AActor* Actor2, UPhysicsConstraintComponent* Constraint)
{

	if (!Actor1 || !Actor2 || !Constraint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid input to AttachActorsWithPhysicsConstraint"));
		return;
	}

	
	// Retrieve the root components (assuming they are Static Meshes)
	UPrimitiveComponent* Component1 = Cast<UPrimitiveComponent>(Actor1->GetRootComponent());
	UPrimitiveComponent* Component2 = Cast<UPrimitiveComponent>(Actor2->GetRootComponent());

	if (!Component1 || !Component2)
	{
		UE_LOG(LogTemp, Warning, TEXT("One or both actors are missing a valid root component."));
		return;
	}

	Actor1->SetActorRotation(Actor2->GetActorRotation()); //set rotations
	
	FVector TargetLocation = Actor2->GetActorLocation(); //align the cart
	FVector NewLocation = Actor1->GetActorLocation();
	FVector ForwardVector = Actor2->GetActorForwardVector();
	NewLocation.X = TargetLocation.X-(ForwardVector.X* 50);
	NewLocation.Y = TargetLocation.Y - (ForwardVector.Y * 50);
	NewLocation.Z = TargetLocation.Z - (ForwardVector.Z * 50);
	Actor1->SetActorLocation(NewLocation);



	// Ensure both components have physics enabled
	Component1->SetSimulatePhysics(true);
	Component2->SetSimulatePhysics(true);

	// Set the constrained components
	Constraint->SetConstrainedComponents(Component1, NAME_None, Component2, NAME_None);

	// Configure constraint settings (optional)
	//Constraint->ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 100.0f); // Example: limit linear motion along X-axis
	Constraint->ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f); // Example: free angular movement
	Constraint->ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.0f); // Example: lock one axis
	Constraint->ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f); // Lock twist
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Actor1->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Attached to"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Actor2->GetName());
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%lld"), Actor1->GetComponentsByTag(USceneComponent::StaticClass(), FName("CartLink")).Num()));
	}


	UE_LOG(LogTemp, Log, TEXT("Physics constraint set between %s and %s"), *Actor1->GetName(), *Actor2->GetName());
}

void USphereCartActor::RemovePhysicsConstraints()
{
	TSet<UActorComponent*> components=GetOwner()->GetComponents();
	for (auto& c : components) {
		if (c->IsA(UPhysicsConstraintComponent::StaticClass())) {
			if (c->GetName() == "PhysicsConstraint") {
				c->DestroyComponent();
				isAttached = false;
				//disable the front collision
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("REMOVING PHYSICS CONSTRAINT"));
				UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontCollision"))[0]);
				Box->SetGenerateOverlapEvents(false);
			}
		}
	}
}


void USphereCartActor::EnableFrontCollision() { //reenables the front collision (used in cases where they detach the cart, and have stopped holding it), called by blueprint
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontCollision"))[0]);
	Box->SetGenerateOverlapEvents(true);
}

void USphereCartActor::DisableBackCollision() { //disables back collision
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackCollision"))[0]);
	Box->SetGenerateOverlapEvents(false);
}

void USphereCartActor::EnableBackCollision() { //enables back collision
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackCollision"))[0]);
	Box->SetGenerateOverlapEvents(true);
}










// Called every frame
void USphereCartActor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Dynamic Sound for the carts
	double speed = GetOwner()->GetVelocity().Length();

	UAudioComponent* rollingSound = Cast<UAudioComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("RollingAudio"))[0]);
	double exponent = (speed * -1) + 50;
	double exponentResult = pow(1.1, exponent);
	double multiplier = 1/(1+(exponentResult));

	rollingSound->SetVolumeMultiplier(multiplier);
	if (!rollingSound->IsPlaying())
	{
		rollingSound->Play();
	}



	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%f"), multiplier));
	

}



