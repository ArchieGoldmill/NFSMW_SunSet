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
	WeatherData current;

	float Timer = 0.0f;
	float rain = 0.0f;

	TextureInfo* Water = nullptr;
	TextureInfo* CarRainDrops = nullptr;
	TextureInfo* CarRainSlide = nullptr;
	TextureInfo* PuddleMask = NULL;
	TextureInfo* RoadDetail = NULL;
	TextureInfo* RainSplash[30];

	D3DXVECTOR4 rainParams = { 0, 0, 0, 0 };
	D3DXVECTOR4 fogValue;

	float RoadWetness = 0.0f;
	float RoadRainDrops = 0.0f;

	float LightIntensity = 0.0f;

	float NightFactor = 0.0f;

public:

	void Update()
	{
		this->Timer += Game::DeltaTime;

		this->UpdateWeather();
		this->UpdateTextures();
		this->UpdateSun();
		this->SetParams();
		this->UpdateRain();
		this->UpdateWater();

		MoveTowards(this->LightIntensity, this->LightsOn() ? 1.0 : 0.0, Game::DeltaTime * 2);
	}

	float GetLightIntensity()
	{
		return this->LightIntensity;
	}

	bool WorldLightsOn()
	{
		return this->LightIntensity > 0.0;
	}

	float GetRain()
	{
		return this->rain;
	}

	float GetCarLightsPower()
	{
		return this->current.CarLightsPower;
	}

	float GetTextureLightPower()
	{
		return this->current.TextureLightPower;
	}

	bool LightsOn()
	{
		float time = this->GetTime();
		return (time > g_Config.LightsOn || time < g_Config.LightsOff);
	}

	bool IsDay()
	{
		return NightFactor == 1.0;
	}

	D3DXVECTOR4 GetGodRaysColor()
	{
		return this->current.GodRaysColor;
	}

	D3DXVECTOR4 GetBloom()
	{
		D3DXVECTOR4 bloom;

		bloom.x = this->current.BloomPower;
		bloom.y = this->current.BloomThreshold;

		if (!g_Config.Bloom)
		{
			bloom.x = 0;
		}

		return bloom;
	}

private:

	void SetFog(eEffect* e)
	{
		e->SetVector(ShaderParam::cvFogColor, &this->current.FogColor);
		e->SetVector(ShaderParam::cvFogSunColor, &this->current.FogSunColor);
		e->SetVector(ShaderParam::cvFogValue, &fogValue);
	}

	void SetParams()
	{
		fogValue =
		{
			this->current.FogEnd,
			1.0f / (this->current.FogEnd - this->current.FogStart),
			this->current.FogPower,
			this->current.FogExponent
		};

		for (shader_type stype : { shader_type::WorldShader, shader_type::WorldNormalMap, shader_type::WorldReflectShader, shader_type::GlossyWindow, shader_type::CarShader, shader_type::billboardshader })
		{
			auto e = eEffect::Get(stype);

			this->current.SpecularColor.w = this->current.SpecularPower;

			auto isCar = stype == shader_type::CarShader;
			D3DXVECTOR4 diffuseColor = this->current.DiffuseColor * (isCar ? this->current.CarDiffuseIntensity : this->current.DiffuseIntensity);
			D3DXVECTOR4 ambientColor = this->current.AmbientColor * (isCar ? this->current.CarAmbientIntensity : this->current.AmbientIntensity);

			ambientColor.w = this->LightsOn() ? 1.0f : 0.0f;
			diffuseColor.w = (stype == shader_type::CarShader ? g_Config.CarVertexColor : g_Config.WorldVertexColor) * 1.0;

			e->SetVector(ShaderParam::cvDiffuseColor, &diffuseColor);
			e->SetVector(ShaderParam::cvAmbientColor, &ambientColor);
			e->SetVector(ShaderParam::cvSpecularColor, &this->current.SpecularColor);
			this->SetFog(e);
		}

		auto worldPrelit = eEffect::Get(shader_type::WorldPrelitShader);
		this->SetFog(worldPrelit);

		auto glossyWindow = eEffect::Get(shader_type::GlossyWindow);
		D3DXVECTOR4 windowGlowColor = g_Config.WindowGlowColor;
		windowGlowColor *= g_Config.WindowGlowPower;
		glossyWindow->SetVector(ShaderParam::cvWindowColor, &windowGlowColor);
	}

	void LerpWeather(WeatherData* a, WeatherData* b, float t)
	{
		this->current.DiffuseIntensity = std::lerp(a->DiffuseIntensity, b->DiffuseIntensity, t);
		this->current.AmbientIntensity = std::lerp(a->AmbientIntensity, b->AmbientIntensity, t);
		this->current.CarDiffuseIntensity = std::lerp(a->CarDiffuseIntensity, b->CarDiffuseIntensity, t);
		this->current.CarAmbientIntensity = std::lerp(a->CarAmbientIntensity, b->CarAmbientIntensity, t);

		this->current.DiffuseColor = LerpVector(a->DiffuseColor, b->DiffuseColor, t);
		this->current.AmbientColor = LerpVector(a->AmbientColor, b->AmbientColor, t);
		this->current.SpecularColor = LerpVector(a->SpecularColor, b->SpecularColor, t);
		this->current.SpecularPower = std::lerp(a->SpecularPower, b->SpecularPower, t);

		this->current.SkyBeta = LerpVector(a->SkyBeta, b->SkyBeta, t);
		this->current.SkyRayleigh = std::lerp(a->SkyRayleigh, b->SkyRayleigh, t);
		this->current.SkyMie = std::lerp(a->SkyMie, b->SkyMie, t);
		this->current.SkyBrightness = std::lerp(a->SkyBrightness, b->SkyBrightness, t);

		this->current.FogColor = LerpVector(a->FogColor, b->FogColor, t);
		this->current.FogSunColor = LerpVector(a->FogSunColor, b->FogSunColor, t);
		this->current.FogStart = std::lerp(a->FogStart, b->FogStart, t);
		this->current.FogEnd = std::lerp(a->FogEnd, b->FogEnd, t);
		this->current.FogPower = std::lerp(a->FogPower, b->FogPower, t);
		this->current.FogExponent = std::lerp(a->FogExponent, b->FogExponent, t);

		this->current.CarLightsPower = std::lerp(a->CarLightsPower, b->CarLightsPower, t);
		this->current.TextureLightPower = std::lerp(a->TextureLightPower, b->TextureLightPower, t);

		this->current.CloudColor = LerpVector(a->CloudColor, b->CloudColor, t);

		this->current.WaterColor = LerpVector(a->WaterColor, b->WaterColor, t);
		this->current.WaterSpecularPower = std::lerp(a->WaterSpecularPower, b->WaterSpecularPower, t);

		this->current.BloomPower = std::lerp(a->BloomPower, b->BloomPower, t);
		this->current.BloomThreshold = std::lerp(a->BloomThreshold, b->BloomThreshold, t);

		this->current.GodRaysColor = LerpVector(a->GodRaysColor, b->GodRaysColor, t);
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
			if (this->rain == 0.0f)
			{
				this->LerpWeather(&a->Main, &b->Main, t);
			}
			else if (this->rain == 1.0f)
			{
				this->LerpWeather(&a->Rain, &b->Rain, t);
			}
			else
			{
				this->LerpWeather(&a->Main, &b->Main, t);
				auto mainCurrent = this->current;
				this->LerpWeather(&a->Rain, &b->Rain, t);
				auto rainCurrent = this->current;
				this->LerpWeather(&mainCurrent, &rainCurrent, this->rain);
			}
		}
	}

	void InitTextures()
	{
		LoadSkyBoxTexture();
		LoadVolumeTexture();

		if (!this->PuddleMask)
		{
			this->Water = TextureInfo::Get(Hashes::SR_PCAWATER0, false, false);
			this->CarRainDrops = TextureInfo::Get(Hashes::SR_CAR_RAINDROPS_N, false, false);
			this->CarRainSlide = TextureInfo::Get(Hashes::SR_CAR_RAINDROPS_SLIDE_N, false, false);
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

		if (this->PuddleMask)
		{
			auto roadShader = eEffect::Get(shader_type::WorldReflectShader);
			int frame = (int)fmodf(this->Timer * 30.0f, 30);

			roadShader->SetTexture(ShaderParam::MISCMAP1_TEXTURE, this->PuddleMask);
			roadShader->SetTexture(ShaderParam::MISCMAP2_TEXTURE, this->RainSplash[frame]);
			roadShader->SetTexture(ShaderParam::MISCMAP3_TEXTURE, this->RoadDetail);
		}

		if (this->CarRainDrops)
		{
			auto carShader = eEffect::Get(shader_type::CarShader);
			carShader->SetTexture(ShaderParam::MISCMAP1_TEXTURE, NoiseTexture);
			carShader->SetTexture(ShaderParam::MISCMAP2_TEXTURE, this->CarRainDrops);
			carShader->SetTexture(ShaderParam::MISCMAP3_TEXTURE, this->CarRainSlide);
		}
	}

	void UpdateSun()
	{
		float time = this->GetTime();

		float sunTime;
		float sunRise = g_Config.SunRise;
		float sunSet = g_Config.SunSet;
		NightFactor = 0;

		if (time >= sunRise && time <= sunSet)
		{
			sunTime = ConvertRange(time, sunRise, sunSet, 0, 1.0f);
			NightFactor = 1.0f;
		}
		else
		{
			sunTime = time >= sunSet ? ConvertRange(time, sunSet, 1.0f, 0, 0.5f) : ConvertRange(time, 0.0f, sunRise, 0.5f, 1.0f);
		}

		float offset = 0.1;
		sunTime = ConvertRange(sunTime, 0, 1, -offset, 1 + offset);

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

		D3DXVECTOR4 skyParams;
		skyParams.x = this->current.SkyRayleigh;
		skyParams.y = this->current.SkyMie;
		skyParams.z = this->current.SkyBrightness;
		skyParams.w = NightFactor;

		auto e = eEffect::Get(shader_type::skyshader);
		e->SetVector(ShaderParam::cvSunDirection, &sunDirection);
		e->SetVector(ShaderParam::cvSkyBeta, &this->current.SkyBeta);
		e->SetVector(ShaderParam::cvSkyParams, &skyParams);
		e->SetVector(ShaderParam::cvCloudColor, &this->current.CloudColor);
		e->SetFloat(ShaderParam::cfTimeTicker, this->Timer);

		if (SkyCubeTexture)
		{
			e->SetTexture(ShaderParam::MISCMAP1_TEXTURE, SkyCubeTexture);
		}
	}

	void UpdateRain()
	{
		bool isRaining = this->IsRaining();
		MoveTowards(this->RoadWetness, isRaining ? 1.0 : 0.0, Game::DeltaTime * 1.0 / (isRaining ? g_Config.WetTime : g_Config.DryTime));

		MoveTowards(this->rain, isRaining > 0.0f ? 1.0f : 0.0f, Game::DeltaTime / 20.0f);
		if (g_Config.Editor)
		{
			if (*Game::ForceRain)
			{
				this->rain = 1;
			}
			else
			{
				this->rain = 0;
			}
		}

		rainParams.x = Rain::Instance->Intensity;
		rainParams.y = RoadWetness;

		if (isRaining)
		{
			rainParams.z = Timer;
		}

		auto roadShader = eEffect::Get(shader_type::WorldReflectShader);
		roadShader->SetVector(ShaderParam::cvRainParams, &rainParams);

		auto carShader = eEffect::Get(shader_type::CarShader);
		carShader->SetVector(ShaderParam::cvRainParams, &rainParams);
	}

	void UpdateWater()
	{
		if (this->Water && Game::ReflectionTexture)
		{
			this->current.WaterColor.w = this->current.WaterSpecularPower;

			auto waterShader = eEffect::Get(shader_type::WorldShader);
			waterShader->SetVector(ShaderParam::cvWaterColor, &this->current.WaterColor);
			waterShader->SetTexture(ShaderParam::MISCMAP4_TEXTURE, this->Water);
			waterShader->SetFloat(ShaderParam::cfTimeTicker, this->Timer);
			waterShader->SetTexture(shader_param::REFLECTEDTEX, Game::ReflectionTexture);
		}
	}

	float GetTime()
	{
		return TimeOfDay::Instance->CurrentTime;
	}

	bool IsRaining()
	{
		if (Game::State == 3)
		{
			return false;
		}

		return Rain::Instance->Intensity > 0.0f;
	}
};

inline Weather g_Weather;