#pragma once
#include "Game.h"
#include "TextureInfo.h"
#include "eEffect.h"
#include "TimeOfDay.h"

class Weather
{
private:
	float Timer = 0.0f;

	TextureInfo* PuddleMask = nullptr;
	TextureInfo* RainSplash[30];

public:
	void Update()
	{
		this->Timer += Game::DeltaTime;

		this->UpdateTextures();
		this->UpdateSun();
	}

private:

	void InitTextures()
	{
		if (!this->PuddleMask)
		{
			this->PuddleMask = TextureInfo::Get(Hashes::SR_PUDDLE_MASK, false, false);

			char buff[256];
			for (int i = 0; i < 30; i++)
			{
				sprintf(buff, "SR_RAINSPLASH%02d", i);
				this->RainSplash[i] = TextureInfo::Get(Game::bStringHash(buff), false, false);
			}
		}
	}

	void UpdateTextures()
	{
		this->InitTextures();

		auto roadShader = eEffect::Get(shader_type::WorldReflectShader);
		int frame = (int)fmodf(this->Timer * 30.0f, 30);

		roadShader->SetTexture(shader_param::FILTERTEXTURE0, PuddleMask);
		roadShader->SetTexture(shader_param::FILTERTEXTURE1, RainSplash[frame]);
	}

	void UpdateSun()
	{
		float time = this->GetTime();

		float sunTime;
		float sunRise = g_Config.SunRise;
		float sunSet = g_Config.SunSet;

		if (time >= sunRise && time <= sunSet)
		{
			sunTime = ConvertRange(time, sunRise, sunSet, 0, 1.0f);
		}
		else
		{
			sunTime = time >= sunSet ? ConvertRange(time, sunSet, 1.0f, 0, 0.5f) : ConvertRange(time, 0.0f, sunRise, 0.5f, 1.0f);
		}

		D3DXVECTOR4 sunDirection(0, 0, 0, 0);

		const float pi = 3.141592f;

		float altitude_rad = sin(sunTime * pi) * pi / 3;
		float azimuth_rad = pi * sunTime + pi / 2;

		sunDirection.x = cos(altitude_rad) * sin(azimuth_rad);
		sunDirection.y = cos(altitude_rad) * cos(azimuth_rad);
		sunDirection.z = sin(altitude_rad);

		D3DXVec4Normalize(&sunDirection, &sunDirection);

		TimeOfDay::Instance->SunDirection = sunDirection;

		auto e = eEffect::Get(shader_type::skyshader);
		e->SetVector(ShaderParam::cvSunDirection, &sunDirection);
	}

	float GetTime()
	{
		return TimeOfDay::Instance->CurrentTime;
	}
};

inline Weather g_Weather;