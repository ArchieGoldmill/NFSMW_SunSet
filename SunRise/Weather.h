#pragma once
#include "Game.h"
#include "TextureInfo.h"
#include "eEffect.h"
#include "TimeOfDay.h"
#include "Utilities.h"
#include "Rain.h"

class Weather
{
private:
	WeatherConfig current;

	float Timer = 0.0f;

	TextureInfo* PuddleMask = nullptr;
	TextureInfo* RoadDetail = nullptr;
	TextureInfo* RainSplash[30];

	float RoadWetness = 0.0f;
	float RoadRainDrops = 0.0f;

	float LightIntensity = 0.0f;

public:
	void Update()
	{
		this->Timer += Game::DeltaTime;

		this->UpdateWeather();
		this->UpdateTextures();
		this->UpdateSun();
		this->SetParams();
		this->UpdateRain();

		MoveTowards(this->LightIntensity, this->LightsOn() ? 1.0 : 0.0, Game::DeltaTime);
	}

	float GetLightIntensity()
	{
		return this->LightIntensity;
	}

	bool WorldLightsOn()
	{
		return this->LightIntensity > 0.0;
	}

private:

	bool LightsOn()
	{
		float time = this->GetTime();
		return (time > g_Config.LightsOn || time < g_Config.LightsOff);
	}

	void SetParams()
	{
		for (shader_type stype : { shader_type::WorldShader, shader_type::WorldNormalMap, shader_type::WorldReflectShader, shader_type::GlossyWindow, shader_type::CarShader, shader_type::billboardshader })
		{
			auto e = eEffect::Get(stype);

			this->current.DiffuseColor.w = this->LightsOn() ? 1.0f : 0.0f;
			this->current.SpecularColor.w = this->current.SpecularPower;

			e->SetVector(ShaderParam::cvDiffuseColor, &this->current.DiffuseColor);
			e->SetVector(ShaderParam::cvAmbientColor, &this->current.AmbientColor);
			e->SetVector(ShaderParam::cvSpecularColor, &this->current.SpecularColor);
		}
	}

	void LerpWeather(WeatherConfig* a, WeatherConfig* b, float t)
	{
		this->current.DiffuseColor = LerpVector(a->DiffuseColor, b->DiffuseColor, t);
		this->current.AmbientColor = LerpVector(a->AmbientColor, b->AmbientColor, t);
		this->current.SpecularColor = LerpVector(a->SpecularColor, b->SpecularColor, t);
		this->current.SpecularPower = std::lerp(a->SpecularPower, b->SpecularPower, t);
	}

	void UpdateWeather()
	{
		WeatherConfig* a = NULL;
		WeatherConfig* b = NULL;
		float t = 0;
		int weatherSize = WeatherList.size();

		float time = this->GetTime();
		if (time == 0.0f || time == 1.0f)
		{
			a = b = WeatherList[0];
			t = 0.0f;
		}
		else if (time > WeatherList[weatherSize - 1]->Time)
		{
			a = WeatherList[weatherSize - 1];
			b = WeatherList[0];
			t = ConvertRange(time, a->Time, 1.0, 0.0f, 1.0f);
		}
		else
		{
			for (int i = 0; i < weatherSize; i++)
			{
				if (WeatherList[i]->Time >= time)
				{
					a = WeatherList[i - 1];
					b = WeatherList[i];
					t = ConvertRange(time, a->Time, b->Time, 0.0f, 1.0f);
					break;
				}
			}
		}

		if (a && b)
		{
			this->LerpWeather(a, b, t);
		}
	}

	void InitTextures()
	{
		if (!this->PuddleMask)
		{
			this->PuddleMask = TextureInfo::Get(Hashes::SR_PUDDLE_MASK, false, false);
			this->RoadDetail = TextureInfo::Get(Hashes::SR_ROAD_DETAIL, false, false);

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

		roadShader->SetTexture(shader_param::FILTERTEXTURE0, this->PuddleMask);
		roadShader->SetTexture(shader_param::FILTERTEXTURE1, this->RainSplash[frame]);
		roadShader->SetTexture(shader_param::FILTERTEXTURE2, this->RoadDetail);
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
		sunDirection.w = 0;

		D3DXVec4Normalize(&sunDirection, &sunDirection);

		TimeOfDay::Instance->SunDirection = sunDirection;

		auto e = eEffect::Get(shader_type::skyshader);
		e->SetVector(ShaderParam::cvSunDirection, &sunDirection);
	}

	void UpdateRain()
	{
		bool isRaining = this->IsRaining();
		MoveTowards(this->RoadWetness, isRaining ? 1.0 : 0.0, Game::DeltaTime * (isRaining ? 0.5 : 0.1));

		auto roadShader = eEffect::Get(shader_type::WorldReflectShader);

		D3DXVECTOR4 rainParams(0, 0, 0, 0);
		rainParams.x = Rain::Instance->Intensity;
		rainParams.y = RoadWetness;

		roadShader->SetVector(ShaderParam::cvRainParams, &rainParams);
	}

	float GetTime()
	{
		return TimeOfDay::Instance->CurrentTime;
	}

	bool IsRaining()
	{
		return Rain::Instance->Intensity > 0.0f;
	}
};

inline Weather g_Weather;