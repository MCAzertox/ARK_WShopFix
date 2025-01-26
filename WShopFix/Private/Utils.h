#pragma once

#include "Main.h"

namespace WShopBDFix::Utils
{
	FString GetBlueprint(UObjectBase* object);
	FString FloatAsStringWithPrecision(double number, int precision);
	std::string formatTime(int totalSeconds);
	AShooterPlayerController* getPlayerForSteamId(uint64 steamId);
	void SplitString(const std::string& s, char delim, std::vector<std::string>& out);
    int GetInternalIndex(const FString& classBlueprint, const FString& functionName);
}