#pragma once
#include "Game.h"
#include "SolidLights.h"
#include "CustomMeshContainer.h"

inline std::unordered_map<int, FESolidLights> FrontEndLights;
inline SpotLight CarHeadlighsConfig;
inline SpotLight CarAiHeadlighsConfig;
inline SpotLight CarBrakeLightsOnConfig;
inline SpotLight CarBrakeLightsOffConfig;
inline SpotLight CarReverseConfig;
inline SpotLight CopLightBlueConfig;
inline SpotLight CopLightRedConfig;
inline SpotLight ExhaustLightConfig;
inline SpotLightModel HelicopterLightConfig;
inline CustomMeshContainer CustomMeshes;

struct WeatherData
{
	D3DXVECTOR4 DiffuseColor;
	D3DXVECTOR4 AmbientColor;
	D3DXVECTOR4 SpecularColor;
	float SpecularPower;

	float DiffuseIntensity;
	float AmbientIntensity;
	float CarDiffuseIntensity;
	float CarAmbientIntensity;

	D3DXVECTOR4 SkyBeta;
	float SkyRayleigh;
	float SkyMie;
	float SkyBrightness;

	D3DXVECTOR4 CloudColor;
	D3DXVECTOR4 MoonColor;
	float MoonSize;

	D3DXVECTOR4 WaterColor;
	float WaterSpecularPower;

	D3DXVECTOR4 FogColor;
	D3DXVECTOR4 FogSunColor;
	float FogStart;
	float FogEnd;
	float FogPower;
	float FogExponent;

	float CarLightsPower;
	float TextureLightPower;

	float BloomPower;
	float BloomThreshold;

	D3DXVECTOR4 GodRaysColor;
	float SunFlare;

	float RoadMaskIntensity;
};

struct WeatherConfig
{
	float Time;
	WeatherData Main;
	WeatherData Rain;
};

inline std::vector<WeatherConfig*> WeatherList;

struct RainConfig
{
	float DryTime;
	float DryTimeRandom;
	float LightTime;
	float LightTimeRandom;
	float HeavyTime;
	float HeavyTimeRandom;
	float LightningTimeOut;
	bool RainCloudsTexture;
};

struct Config
{
	float ForceTime;
	float TimeUpdateRate;
	float LightLodDistance;
	float SunRise;
	float SunSet;
	float LightsOn;
	float LightsOff;
	float LightCellSize;
	float SunAzimuth;

	int MaxLights;

	bool Console;
	bool ShaderLoader;
	bool ShaderCompiler;
	bool LiveReload;
	bool X360Effects;
	bool Editor;
	bool SkipFE;
	bool RandomStartupTime;
	bool RealTime;
	bool CarVinylPaintFix;
	bool BrakeGlow;
	bool GodRays;
	bool Bloom;
	bool RealFeReflections;
	bool WorldVertexColor;
	bool CarVertexColor;
	bool TunnelWetnessFix;
	bool DisableNightShadows;
	bool MSAAx8;
	bool Tonemapping;
	bool RandomWindows;
	bool DisableSunFlare;
	bool RoadblockLights;

	float WetTime;
	float DryTime;

	float BlurMinSpeed;
	float BlurMaxSpeed;
	float BlurDepth;

	float CustomMeshDist;
	float FlareDistance;

	int HK_ShaderReload;
	int HK_ToggleEditor;

	D3DXVECTOR4 WindowGlowColor;
	float WindowGlowPower;
	bool WindowGlowOverride;

	RainConfig Rain;

	HashField Filter;
	float FilterPower;
};

inline Config g_Config;

