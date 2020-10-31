// Copyright Danny Kay 2020.

#include "Framework/ShooterGameModeBase.h"
#include "Framework/ShooterGameStateBase.h"
#include "Player/ShooterPlayerState.h"
#include "UObject/ConstructorHelpers.h"


AShooterGameModeBase::AShooterGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	GameStateClass = AShooterGameStateBase::StaticClass();
	PlayerStateClass = AShooterPlayerState::StaticClass();
}
