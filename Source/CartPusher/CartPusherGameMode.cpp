// Copyright Epic Games, Inc. All Rights Reserved.

#include "CartPusherGameMode.h"
#include "CartPusherCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACartPusherGameMode::ACartPusherGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
