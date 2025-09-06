#pragma once
#include <string>
#include "Hashes.h"
#include "Game.h"

struct HashField
{
	Hash hash = 0;
	std::string str = "";

	void SetString(std::string str)
	{
		this->str = str;
		this->hash = Game::bStringHash(this->str.c_str());
	}

	inline std::string& GetString()
	{
		return this->str;
	}

	inline const char* GetChar()
	{
		return this->str.c_str();
	}
};