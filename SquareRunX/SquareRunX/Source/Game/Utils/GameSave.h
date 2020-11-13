#pragma once
#include <string>

struct GameSave
{
	std::string PlayerName;
	uint8_t Level = 1, Act = 1;
	uint8_t CheckpointIndex = 0;
	int NumLives = 3, CurrentHealth = 100, MaxHealth = 100;

	uint32_t Score = 0;
	// PowerupState CurrentPowerupState;
};