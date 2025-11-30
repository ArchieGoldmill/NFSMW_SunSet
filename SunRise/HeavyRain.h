#pragma once

class RainManager
{
private:
	const char* pattern = "DLDH";
	int current;

	float timer = 0;
	float target = 0;

	float rain = 0;
	float roadWetness = 0;
	float tunnelWetness = 0;

	float lightningTimeOut = 0;
	float lightningAnimTime = 0;

	D3DXVECTOR4 lightningParams = { 0, 0, 0, 0 };
	D3DXVECTOR4 rainParams = { 0, 0, 0, 0 };

	int lightningTex = 0;

public:
	
	void SetStartState()
	{
		this->lightningTimeOut = g_Config.Rain.LightningTimeOut;

		this->current = time(NULL) % 10;
		if (this->current < 8)
		{
			this->current = 0;
		}
		else if (this->current < 9)
		{
			this->current = 1;
		}
		else
		{
			this->current = 3;
		}

		this->SetRainState();
	}

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

		char state = this->GetState();

		return (state == 'L' || state == 'H') || Game::ForceRain;
	}

	bool IsHeavy()
	{
		return (GetState() == 'H' && this->IsRaining()) || Game::ForceRain;
	}

	void Update()
	{
		if (!Game::IsPaused())
		{
			this->timer += Game::DeltaTime;

			MoveTowards(this->rain, this->IsRaining() && this->IsHeavy() ? 1.0f : 0.0f, Game::DeltaTime / 20.0f);
		}

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

	void SetRoadMaskIntensity(float w)
	{
		this->rainParams.w = w;
	}

private:

	char GetState()
	{
		return this->pattern[this->current];
	}

	void UpdateRainState()
	{
		if (this->timer > this->target)
		{
			this->timer = 0;

			this->current++;
			if (this->current > 3)
			{
				this->current = 0;
			}

			this->SetRainState();
		}
	}

	void SetRainState()
	{
		char state = this->GetState();

		if (state == 'H')
		{
			this->target = (g_Config.Rain.HeavyTime + Game::fRandom(g_Config.Rain.HeavyTimeRandom)) * 60;
		}

		if (state == 'L')
		{
			this->target = (g_Config.Rain.LightTime + Game::fRandom(g_Config.Rain.LightTimeRandom)) * 60;
		}

		if (state == 'D')
		{
			this->target = (g_Config.Rain.DryTime + Game::fRandom(g_Config.Rain.DryTimeRandom)) * 60;
		}
	}

	void UpdateLightning()
	{
		if (this->IsHeavy() && Game::State == 6 && this->rain == 1.0)
		{
			if (!Game::IsPaused())
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
		}
		else
		{
			lightningParams = { 0, 0, 0, 0 };
		}

		if (g_Config.Rain.RainCloudsTexture)
		{
			lightningParams.z = this->rain;
		}

		auto e = eEffect::Get(shader_type::skyshader);
		e->SetVector(ShaderParam::cvLightning, &lightningParams);
	}

	void UpdateShaders()
	{
		bool isRaining = this->IsRaining();

		if (!Game::IsPaused())
		{
			MoveTowards(this->roadWetness, isRaining ? 1.0 : 0.0, Game::DeltaTime / (isRaining ? g_Config.WetTime : g_Config.DryTime));

			if (g_Config.TunnelWetnessFix)
			{
				MoveTowards(this->tunnelWetness, Rain::Instance->IsInTunnel ? 0.0 : 1.0, Game::DeltaTime);
			}
			else
			{
				this->tunnelWetness = 1.0f;
			}
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
	g_Rain.SetStartState();

	injector::MakeCALL(0x00758351, GetRandomRain);
}