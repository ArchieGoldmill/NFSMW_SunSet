#pragma once

class RainManager
{
private:
	float timer = 0;
	float target = 0;
	bool isHeavy = true;
	bool isRaining = true;

public:

	void Update()
	{
		this->timer += Game::DeltaTime;

		if (this->timer > this->target)
		{
			this->timer = 0;

			this->isRaining = !this->isRaining;

			if (this->isRaining)
			{
				this->isHeavy = !this->isHeavy;

				if (this->isHeavy)
				{
					this->target = (3 + Game::fRandom(3)) * 60;
				}
				else
				{
					this->target = (1 + Game::fRandom(1)) * 60;
				}
			}
			else
			{
				this->target = (10 + Game::fRandom(10)) * 60;
			}
		}
	}

	bool IsRaining()
	{
		return this->isRaining;
	}

	bool IsHeavy()
	{
		return this->isHeavy || Game::ForceRain;
	}
};

RainManager g_Rain;

float __cdecl GetRandomRain(float x, float y)
{
	return g_Rain.IsRaining() ? 1.0 : 0.0;
}

void InitHeavyRain()
{
	injector::MakeCALL(0x00758351, GetRandomRain);
}