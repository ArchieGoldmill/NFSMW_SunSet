#pragma once
#include "Game.h"
#include "TextureInfo.h"
#include "eEffect.h"
#include "TimeOfDay.h"
#include "Utilities.h"
#include "Rain.h"
#include "DirectResources.h"
#include "HeavyRain.h"

class Weather
{
private:
	WeatherData current;

	TextureInfo* Water = NULL;
	TextureInfo* CarRainDrops = NULL;
	TextureInfo* CarRainSlide = NULL;
	TextureInfo* PuddleMask = NULL;
	TextureInfo* RoadDetail = NULL;
	TextureInfo* RainSplash[30];
	TextureInfo* SkyNoise[2] = { NULL, NULL };
	TextureInfo* Lightning[2] = { NULL, NULL };
	TextureInfo* MoonTex = NULL;
	TextureInfo* SunTex = NULL;
	TextureInfo* SkyTex = NULL;
	TextureInfo* SkyRainTex = NULL;

	D3DXVECTOR4 fogValue;

	float RoadRainDrops = 0.0f;
	float LightIntensity = 0.0f;
	float NightFactor = 0.0f;
	float AmbientIntencity = 1.0;
	float DiffuseIntencity = 1.0;
	int day = 1;

public:

	void Update()
	{
		this->UpdateWeather();
		this->UpdateTextures();
		this->UpdateSun();
		this->SetParams();
		this->UpdateWater();

		if (!Game::IsPaused())
		{
			MoveTowards(this->LightIntensity, this->LightsOn() ? 1.0 : 0.0, Game::DeltaTime * 2);
		}
	}

	float GetLightIntensity()
	{
		return this->LightIntensity;
	}

	bool WorldLightsOn()
	{
		return this->LightIntensity > 0.0;
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

	float TimeSinceLightsOn()
	{
		float time = this->GetTime();
		if (time > g_Config.LightsOn)
		{
			return time - g_Config.LightsOn;
		}

		if (time < g_Config.LightsOff)
		{
			return 1 - g_Config.LightsOn + time;
		}

		return -1;
	}

	float TimeSinceLightsOff()
	{
		float time = this->GetTime();
		if (time >= g_Config.LightsOff && time <= g_Config.LightsOn)
		{
			return time - g_Config.LightsOff;
		}

		return -1;
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

		auto renderTarget = RenderTarget::Player;

		bloom.x = this->current.BloomPower;
		bloom.y = this->current.BloomThreshold;
		bloom.z = renderTarget->resolution_x;
		bloom.w = renderTarget->resolution_y;

		if (!g_Config.Bloom)
		{
			bloom.x = 0;
		}

		return bloom;
	}

	D3DXVECTOR4 GetWindowGlowColor()
	{
		D3DXVECTOR4 windowGlowColor = g_Config.WindowGlowColor;
		windowGlowColor *= g_Config.WindowGlowPower;
		return windowGlowColor;
	}

	int GetDay()
	{
		return this->day;
	}

	void IncDay()
	{
		this->day++;
	}

	float* GetSunFlare()
	{
		return &this->current.SunFlare;
	}

	D3DXVECTOR3 GetExposure()
	{
		D3DXVECTOR3 exp;

		exp.x = this->current.ExposureKey;
		exp.y = this->current.ExposureMin;
		exp.z = this->current.ExposureMax;

		return exp;
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

		MoveTowards(this->DiffuseIntencity, Rain::Instance->IsInTunnel ? this->current.TunnelDiffuseIntensity : 1, Game::DeltaTime);
		MoveTowards(this->AmbientIntencity, Rain::Instance->IsInTunnel ? this->current.TunnelAmbientIntensity : 1, Game::DeltaTime);

		for (shader_type stype : { shader_type::WorldShader, shader_type::WorldNormalMap, shader_type::WorldReflectShader, shader_type::GlossyWindow, shader_type::CarShader, shader_type::billboardshader })
		{
			auto e = eEffect::Get(stype);

			this->current.SpecularColor.w = this->current.SpecularPower;

			auto isCar = stype == shader_type::CarShader;
			D3DXVECTOR4 diffuseColor = this->current.DiffuseColor * (isCar ? this->current.CarDiffuseIntensity : this->current.DiffuseIntensity);
			D3DXVECTOR4 ambientColor = this->current.AmbientColor * (isCar ? this->current.CarAmbientIntensity : this->current.AmbientIntensity);

			if (g_Config.TunnelLightIntensity)
			{
				diffuseColor += ambientColor * (1 - this->AmbientIntencity);

				ambientColor *= this->AmbientIntencity;
				diffuseColor *= this->DiffuseIntencity;
			}

			diffuseColor.w = (stype == shader_type::CarShader ? g_Config.CarVertexColor : g_Config.WorldVertexColor) * 1.0;

			ambientColor += D3DXVECTOR4(0.875, 0.831, 1, 0) * g_Rain.GetDiffuse();

			ambientColor.w = stype == shader_type::CarShader ? this->current.CarSpecularIntensity : this->current.SpecularIntensity;

			e->SetVector(ShaderParam::cvDiffuseColor, &diffuseColor);
			e->SetVector(ShaderParam::cvAmbientColor, &ambientColor);
			e->SetVector(ShaderParam::cvSpecularColor, &this->current.SpecularColor);
			this->SetFog(e);
		}

		auto worldPrelit = eEffect::Get(shader_type::WorldPrelitShader);
		this->SetFog(worldPrelit);

		D3DXVECTOR4 camPos = D3DXVECTOR4(GetCameraPos(), 1);
		for (int i = 0; i < (int)shader_type::_count; i++)
		{
			auto e = eEffect::Get((shader_type)i);
			e->SetFloat(ShaderParam::cfShadowsEnabled, (Game::DrawShadows && Game::ShadowDetail > 0) ? 1.0 : 0.0);
			e->SetVector(ShaderParam::cvWorldEyePos, &camPos);
		}

		g_Rain.SetRoadMaskIntensity(this->current.RoadMaskIntensity);
	}

	void LerpWeather(WeatherData* a, WeatherData* b, float t)
	{
		this->current.DiffuseIntensity = std::lerp(a->DiffuseIntensity, b->DiffuseIntensity, t);
		this->current.AmbientIntensity = std::lerp(a->AmbientIntensity, b->AmbientIntensity, t);
		this->current.SpecularIntensity = std::lerp(a->SpecularIntensity, b->SpecularIntensity, t);

		this->current.CarDiffuseIntensity = std::lerp(a->CarDiffuseIntensity, b->CarDiffuseIntensity, t);
		this->current.CarAmbientIntensity = std::lerp(a->CarAmbientIntensity, b->CarAmbientIntensity, t);
		this->current.CarSpecularIntensity = std::lerp(a->CarSpecularIntensity, b->CarSpecularIntensity, t);

		this->current.TunnelDiffuseIntensity = std::lerp(a->TunnelDiffuseIntensity, b->TunnelDiffuseIntensity, t);
		this->current.TunnelAmbientIntensity = std::lerp(a->TunnelAmbientIntensity, b->TunnelAmbientIntensity, t);

		this->current.DiffuseColor = LerpVector(a->DiffuseColor, b->DiffuseColor, t);
		this->current.AmbientColor = LerpVector(a->AmbientColor, b->AmbientColor, t);
		this->current.SpecularColor = LerpVector(a->SpecularColor, b->SpecularColor, t);
		this->current.SpecularPower = std::lerp(a->SpecularPower, b->SpecularPower, t);

		this->current.SkyBeta = LerpVector(a->SkyBeta, b->SkyBeta, t);
		this->current.SkyRayleigh = std::lerp(a->SkyRayleigh, b->SkyRayleigh, t);
		this->current.SkyMie = std::lerp(a->SkyMie, b->SkyMie, t);
		this->current.SkyBrightness = std::lerp(a->SkyBrightness, b->SkyBrightness, t);
		this->current.SkyPower = std::lerp(a->SkyPower, b->SkyPower, t);

		this->current.FogColor = LerpVector(a->FogColor, b->FogColor, t);
		this->current.FogSunColor = LerpVector(a->FogSunColor, b->FogSunColor, t);
		this->current.FogStart = std::lerp(a->FogStart, b->FogStart, t);
		this->current.FogEnd = std::lerp(a->FogEnd, b->FogEnd, t);
		this->current.FogPower = std::lerp(a->FogPower, b->FogPower, t);
		this->current.FogExponent = std::lerp(a->FogExponent, b->FogExponent, t);

		this->current.CarLightsPower = std::lerp(a->CarLightsPower, b->CarLightsPower, t);
		this->current.TextureLightPower = std::lerp(a->TextureLightPower, b->TextureLightPower, t);

		this->current.CloudColor = LerpVector(a->CloudColor, b->CloudColor, t);
		this->current.MoonColor = LerpVector(a->MoonColor, b->MoonColor, t);
		this->current.MoonSize = std::lerp(a->MoonSize, b->MoonSize, t);

		this->current.WaterColor = LerpVector(a->WaterColor, b->WaterColor, t);
		this->current.WaterSpecularPower = std::lerp(a->WaterSpecularPower, b->WaterSpecularPower, t);

		this->current.BloomPower = std::lerp(a->BloomPower, b->BloomPower, t);
		this->current.BloomThreshold = std::lerp(a->BloomThreshold, b->BloomThreshold, t);

		this->current.RoadMaskIntensity = std::lerp(a->RoadMaskIntensity, b->RoadMaskIntensity, t);

		this->current.GodRaysColor = LerpVector(a->GodRaysColor, b->GodRaysColor, t);
		this->current.SunFlare = std::lerp(a->SunFlare, b->SunFlare, t);

		this->current.ExposureKey = std::lerp(a->ExposureKey, b->ExposureKey, t);
		this->current.ExposureMin = std::lerp(a->ExposureMin, b->ExposureMin, t);
		this->current.ExposureMax = std::lerp(a->ExposureMax, b->ExposureMax, t);
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
			float rain = g_Rain.GetRain();

			if (rain == 0.0f)
			{
				this->LerpWeather(&a->Main, &b->Main, t);
			}
			else if (rain == 1.0f)
			{
				this->LerpWeather(&a->Rain, &b->Rain, t);
			}
			else
			{
				this->LerpWeather(&a->Main, &b->Main, t);
				auto mainCurrent = this->current;
				this->LerpWeather(&a->Rain, &b->Rain, t);
				auto rainCurrent = this->current;
				this->LerpWeather(&mainCurrent, &rainCurrent, rain);
			}
		}
	}

	void InitTextures()
	{
		LoadVolumeTexture();
		CreateFilterTexture();

		if (!this->PuddleMask)
		{
			this->Water = TextureInfo::Get(Hashes::SR_PCAWATER0, false, false);
			this->CarRainDrops = TextureInfo::Get(Hashes::SR_CAR_RAINDROPS_N, false, false);
			this->CarRainSlide = TextureInfo::Get(Hashes::SR_CAR_RAINDROPS_SLIDE_N, false, false);
			this->PuddleMask = TextureInfo::Get(Hashes::SR_PUDDLE_MASK, false, false);
			this->RoadDetail = TextureInfo::Get(Hashes::SR_ROAD_DETAIL, false, false);
			this->SkyNoise[0] = TextureInfo::Get(Hashes::SKYNOISETEX, false, false);
			this->SkyNoise[1] = TextureInfo::Get(Hashes::SKYNOISETEX1, false, false);
			this->Lightning[0] = TextureInfo::Get(Hashes::LIGHTNING_STRIKE0, false, false);
			this->Lightning[1] = TextureInfo::Get(Hashes::LIGHTNING_STRIKE1, false, false);
			this->MoonTex = TextureInfo::Get(Hashes::SR_MOON, false, false);
			this->SunTex = TextureInfo::Get(Hashes::SR_SUN, false, false);
			this->SkyTex = TextureInfo::Get(Hashes::SR_SKYBOX, false, false);
			this->SkyRainTex = TextureInfo::Get(Hashes::SR_SKYBOX_RAIN, false, false);

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
			int frame = (int)fmodf(g_Rain.GetTimer() * 30.0f, 30);

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

		if (g_Config.DisableNightShadows)
		{
			Game::DrawShadows = NightFactor > 0;
		}
		else
		{
			Game::DrawShadows = true;
		}

		float offset = 0.1;
		sunTime = ConvertRange(sunTime, 0, 1, -offset, 1 + offset);

		D3DXVECTOR4 sunDirection(0, 0, 0, 0);

		const float pi = 3.141592f;

		float altitude_rad = sin(sunTime * pi) * pi / 3;
		float azimuth_rad = pi * sunTime + pi / 2 + TimeOfDay::Instance->SunAzimuth;

		sunDirection.x = cos(altitude_rad) * sin(azimuth_rad);
		sunDirection.y = cos(altitude_rad) * cos(azimuth_rad);
		sunDirection.z = sin(altitude_rad);
		sunDirection.w = 0;

		D3DXVec4Normalize(&sunDirection, &sunDirection);

		TimeOfDay::Instance->SunDirection = sunDirection;
		TimeOfDay::Instance->SunPosition = sunDirection * 120000;

		D3DXVECTOR4 skyParams;
		skyParams.x = this->current.SkyRayleigh;
		skyParams.y = this->current.SkyMie;
		skyParams.z = this->current.SkyBrightness;
		skyParams.w = NightFactor;

		D3DXVECTOR4 skyBeta = this->current.SkyBeta;
		skyBeta.w = this->current.SkyPower * this->DiffuseIntencity;

		auto e = eEffect::Get(shader_type::skyshader);
		e->SetVector(ShaderParam::cvSunDirection, &sunDirection);
		e->SetVector(ShaderParam::cvSkyBeta, &skyBeta);
		e->SetVector(ShaderParam::cvSkyParams, &skyParams);
		e->SetVector(ShaderParam::cvCloudColor, &this->current.CloudColor);

		e->SetTexture(ShaderParam::MISCMAP1_TEXTURE, this->SkyTex);
		e->SetTexture(ShaderParam::MISCMAP2_TEXTURE, this->SkyRainTex);

		e->SetTexture(ShaderParam::SkyNoiseTexture, this->SkyNoise[0]);
		e->SetTexture(ShaderParam::SkyNoiseTexture1, this->SkyNoise[1]);

		e->SetTexture(ShaderParam::MISCMAP3_TEXTURE, this->Lightning[g_Rain.GetLightningTex()]);

		if (this->IsDay())
		{
			e->SetTexture(ShaderParam::MISCMAP4_TEXTURE, this->SunTex);
		}
		else
		{
			e->SetTexture(ShaderParam::MISCMAP4_TEXTURE, this->MoonTex);
		}

		e->SetVector(ShaderParam::cvMoonColor, &this->current.MoonColor);
		e->SetFloat(ShaderParam::cfMoonSize, this->current.MoonSize);
	}

	void UpdateWater()
	{
		if (this->Water && Game::ReflectionTexture)
		{
			this->current.WaterColor.w = this->current.WaterSpecularPower;

			auto waterShader = eEffect::Get(shader_type::WorldShader);
			waterShader->SetVector(ShaderParam::cvWaterColor, &this->current.WaterColor);
			waterShader->SetTexture(ShaderParam::MISCMAP4_TEXTURE, this->Water);
			waterShader->SetFloat(ShaderParam::cfTimeTicker, g_Rain.GetTimer());
			waterShader->SetTexture(shader_param::REFLECTEDTEX, Game::ReflectionTexture);
		}
	}

	float GetTime()
	{
		return TimeOfDay::Instance->CurrentTime;
	}
};

inline Weather g_Weather;