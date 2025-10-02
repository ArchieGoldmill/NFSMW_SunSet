#pragma once

class RainManager
{
private:
	float timer = 0;
	float target = 0;
	bool isHeavy = true;
	bool isRaining = true;

	float lightningTimeOut = 15;
	float lightningAnimTime = 0.0;

	D3DXVECTOR4 lightningParams = { 0, 0, 0, 0 };

	int lightningTex = 0;

public:

	float GetDiffuse()
	{
		return lightningParams.y;
	}

	int GetLightningTex()
	{
		return this->lightningTex;
	}

	void Update()
	{
		this->UpdateLightning();

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
				this->target = (10 + Game::fRandom(5)) * 60;
			}
		}
	}

	void UpdateLightning()
	{
		if (this->IsRaining() && this->IsHeavy() && Game::State == 6)
		{
			MoveTowards(this->lightningTimeOut, 0, Game::DeltaTime);

			if (this->lightningTimeOut == 0.0)
			{
				MoveTowards(this->lightningAnimTime, 5.5, Game::DeltaTime);

				if (this->lightningAnimTime <= 0.5)
				{
					MoveTowards(lightningParams.x, 1.0, Game::DeltaTime * 5);
					MoveTowards(lightningParams.y, 1.0, Game::DeltaTime * 5);
				}
				else if (this->lightningAnimTime < 5.5)
				{
					MoveTowards(lightningParams.x, 0.0, Game::DeltaTime * 0.5);
					MoveTowards(lightningParams.y, 0.0, Game::DeltaTime * 2);
				}
				else
				{
					lightningParams = { 0, 0, 0, 0 };

					this->lightningAnimTime = 0.0;
					this->lightningTimeOut = 15;

					this->lightningTex++;
					if (this->lightningTex > 1)
					{
						this->lightningTex = 0;
					}
				}
			}
		}
		else
		{
			lightningParams = { 0, 0, 0, 0 };
		}

		auto e = eEffect::Get(shader_type::skyshader);
		e->SetVector(ShaderParam::cvLightning, &lightningParams);
	}

	bool IsRaining()
	{
		return this->isRaining || Game::ForceRain;
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