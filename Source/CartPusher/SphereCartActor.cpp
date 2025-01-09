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
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontCollision"))[0]); //for cart connections
	UBoxComponent* AudioBox = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("AudioCollision"))[0]);
	UBoxComponent* PlayerBox = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("PlayerCollision"))[0]);

	if (Box) {
		Box->OnComponentBeginOverlap.AddDynamic(this, &USphereCartActor::OnOverlapBegin);
	}
	if (AudioBox) {
		AudioBox->OnComponentBeginOverlap.AddDynamic(this, &USphereCartActor::OverOverlapBeginAudio);
		AudioBox->OnComponentEndOverlap.AddDynamic(this, &USphereCartActor::OnOverlapEndAudio);
	}
	if (PlayerBox) {
		PlayerBox->OnComponentBeginOverlap.AddDynamic(this, &USphereCartActor::OnOverlapBeginPlayerHandle);
		PlayerBox->OnComponentEndOverlap.AddDynamic(this, &USphereCartActor::OnOverlapEndPlayerHandle);
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

void USphereCartActor::OnOverlapEndPlayerHandle(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, OtherActor->GetClass()->GetName());
		if (OtherActor->GetClass()->GetName() == "BP_FirstPersonCharacter_C") { //if the player is colliding
			DisablePlayerCollision();
		}
	}
}

void USphereCartActor::OnOverlapBeginPlayerHandle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, OtherActor->GetClass()->GetName());
		if (OtherActor->GetClass()->GetName() == "BP_FirstPersonCharacter_C") { //if the player is colliding
			EnablePlayerCollision();
		}
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

			//USphereCartActor* thisCart= (USphereCartActor*)(GetOwner()->GetComponentByClass(USphereCartActor::StaticClass()));
			//thisCart->DisableFrontCollision();
			isFrontMostCart = false; 
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("fffffffffffffffffffffffffffffffffff"));

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

void USphereCartActor::DisableFrontCollision() { //disables the front collision (used in cases where there's a cart attached to the front of the cart), called by blueprint UNUSED BECAUSE THE HITBOX IS COVERED BY THE NEXT CART ANYWAYS AND THE BACK HITBOX IS DISABLED WHEN A CART IS ATTACHED
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontCollision"))[0]);
	Box->SetGenerateOverlapEvents(false);

	isFrontMostCart = false;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Disable FrontBlock"));
}

void USphereCartActor::EnableFrontCollision() { //reenables the front collision (used in cases where they detach the cart, and have stopped holding it), called by blueprint
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontCollision"))[0]);
	Box->SetGenerateOverlapEvents(true);

	isFrontMostCart = true;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Enable FrontBlock"));
}

void USphereCartActor::DisableBackCollision() { //disables back collision, also disables the back block
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackCollision"))[0]);
	Box->SetGenerateOverlapEvents(false);

	UStaticMeshComponent* BackBlock = Cast<UStaticMeshComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackBlock"))[0]);
	BackBlock->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	isBackMostCart = false;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Disable Backblock"));
}

void USphereCartActor::EnableBackCollision() { //enables back collision, also reenables the back block
	UBoxComponent* Box = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackCollision"))[0]);
	Box->SetGenerateOverlapEvents(true);

	UStaticMeshComponent* BackBlock = Cast<UStaticMeshComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackBlock"))[0]);
	BackBlock->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	isBackMostCart = true;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Enable Backblock"));
}

void USphereCartActor::EnablePlayerCollision() { //enables the collision between the player and the handles
	TArray<UActorComponent*> CollisionHandles = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("CollisionHandle"));
	for (int i = 0; i < CollisionHandles.Num(); i++) {
		Cast<UStaticMeshComponent>(CollisionHandles[i])->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	if (isBackMostCart == false) { //if it's not the backmost cart, then redisable the collision for the BackBlock (this is because only the back most cart needs to be able to be pulled by the player)
		UStaticMeshComponent* BackBlock = Cast<UStaticMeshComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("BackBlock"))[0]);
		BackBlock->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	if (isFrontMostCart == false) { //if it's not the frontmost cart, then redisable the collision for the FrontBlock (this is because only the front most cart needs to be able to be pulled by the player)
		UStaticMeshComponent* FrontBlock = Cast<UStaticMeshComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("FrontBlock"))[0]);
		FrontBlock->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("test"));
	}
}

void USphereCartActor::DisablePlayerCollision() { //enables the collision between the player and the handles
	TArray<UActorComponent*> CollisionHandles = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("CollisionHandle"));
	for (int i = 0; i < CollisionHandles.Num(); i++) {
		Cast<UStaticMeshComponent>(CollisionHandles[i])->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

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



