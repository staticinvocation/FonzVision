#include "stdafx.h"
#include "game.h"
#include <string>

GameManager* gameManager = new GameManager();

GameManager* GameManager::GetGameManager() {
	return gameManager;
}

GameType GameManager::GetCurrentGame() {
	if (this->typeCache == NULL) {
		if (GetModuleHandleA("arma3.exe") != 0) {
			return GameType::ARMA_3;
		}
		else if (GetModuleHandleA("dayz.exe") != 0) {
			return GameType::DAYZ_STANDALONE;
		} 
	}
	else {
		return this->typeCache;
	}
}