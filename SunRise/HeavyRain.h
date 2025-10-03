#pragma once

class RainManager
{
private:
	float timer = 0;
	float target = 0;
	float rain = 0;
	float roadWetness = 0;
	float tunnelWetness = 0;

	bool isHeavy = true;
	bool isRaining = true;

	float lightningTimeOut = 20;
	float lightningAnimTime = 0;

	D3DXVECTOR4 lightningParams = { 0, 0, 0, 0 };
	D3DXVECTOR4 rainParams = { 0, 0, 0, 0 };

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

	float GetRain()
	{
		return this->rain;
	}

	float GetTimer()
	{
		return this->timer;
	}

	bool IsRaining()
	{
		if (Game::State == 3)
		{
			return false;
		}

		return this->isRaining || Game::ForceRain;
	}

	bool IsHeavy()
	{
		return this->isHeavy || Game::ForceRain;
	}

	void Update()
	{
		this->timer += Game::DeltaTime;

		MoveTowards(this->rain, this->IsRaining() && this->IsHeavy() ? 1.0f : 0.0f, Game::DeltaTime / 20.0f);
		if (g_Config.Editor)
		{
			if (Game::ForceRain)
			{
				this->rain = 1;
			}
			else
			{
				this->rain = 0;
			}
		}

		this->UpdateLightning();
		this->UpdateRainState();
		this->UpdateShaders();
	}

private:

	void UpdateRainState()
	{
		if (this->timer > this->target)
		{
			this->timer = 0;

			this->isRaining = !this->isRaining;

			if (this->isRaining)
			{
				this->isHeavy = !this->isHeavy;

				if (this->isHeavy)
				{
					this->target = (g_Config.Rain.HeavyTime + Game::fRandom(g_Config.Rain.HeavyTimeRandom)) * 60;
				}
				else
				{
					this->target = (g_Config.Rain.LightTime + Game::fRandom(g_Config.Rain.LightTimeRandom)) * 60;
				}
			}
			else
			{
				this->target = (g_Config.Rain.DryTime + Game::fRandom(g_Config.Rain.DryTimeRandom)) * 60;
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
					this->lightningTimeOut = g_Config.Rain.LightningTimeOut;

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

	void UpdateShaders()
	{
		bool isRaining = this->IsRaining();
		MoveTowards(this->roadWetness, isRaining ? 1.0 : 0.0, Game::DeltaTime / (isRaining ? g_Config.WetTime : g_Config.DryTime));

		if (g_Config.TunnelWetnessFix)
		{
			MoveTowards(this->tunnelWetness, Rain::Instance->IsInTunnel ? 0.0 : 1.0, Game::DeltaTime);
		}
		else
		{
			this->tunnelWetness = 1.0f;
		}

		bool covered = Rain::Instance->IsInTunnel || Rain::Instance->IsUnderOverpass;

		rainParams.x = Rain::Instance->Intensity * !covered;
		rainParams.y = this->roadWetness * this->tunnelWetness;

		if (isRaining)
		{
			rainParams.z = this->GetTimer();
		}

		auto roadShader = eEffect::Get(shader_type::WorldReflectShader);
		roadShader->SetVector(ShaderParam::cvRainParams, &rainParams);

		auto carShader = eEffect::Get(shader_type::CarShader);
		carShader->SetVector(ShaderParam::cvRainParams, &rainParams);
	}
};

inline RainManager g_Rain;

float __cdecl GetRandomRain(float x, float y)
{
	return g_Rain.IsRaining() ? 1.0 : 0.0;
}

void InitHeavyRain()
{
	injector::MakeCALL(0x00758351, GetRandomRain);
}