#include "stdafx.h"
#include "RVBootstrap.h"
#include "game.h"

void RVBootstrap::Initialize(HMODULE module) {
	printf("Game identified as ");
	if (GameManager::GetGameManager()->GetCurrentGame() == GameType::DAYZ_STANDALONE) {
		printf("DayZ Standalone\n");
	}
	else if (GameManager::GetGameManager()->GetCurrentGame() == GameType::ARMA_3) {
		printf("ArmA 3\n");
	}
	else {
		FreeLibrary(module);
	}
}