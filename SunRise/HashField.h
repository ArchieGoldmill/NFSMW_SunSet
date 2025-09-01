#pragma once
#include <string>
#include "Hashes.h"
#include "Game.h"

class HashField
{
private:
	Hash hash = 0;
	std::string str = "";

public:
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

	inline Hash GetHash()
	{
		return this->hash;
	}

	inline bool operator == (Hash hash)
	{
		return this->hash == hash;
	}
};