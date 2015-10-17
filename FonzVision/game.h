#include "stdafx.h"
#pragma once

enum GameType {
	ARMA_3,
	DAYZ_STANDALONE
};

class GameManager {
private:
	GameType typeCache;

public:
	static GameManager* GetGameManager();
	GameType GetCurrentGame();
};

