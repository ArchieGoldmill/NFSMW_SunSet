#pragma once
#include <yaml-cpp/yaml.h>
#include "Game.h"
#include "WeatherConfig.h"
#include "FlareConfig.h"
#include "SpotLightConfig.h"
#include "TextureConfig.h"

struct Config
{
	float ForceTime;
	float LightLodDistance;
	float SunRise;
	float SunSet;
	float LightsOn;
	float LightsOff;
	float LightCellSize;

	bool Console;
	bool ShaderLoader;
};

inline Config g_Config;

void LoadConfig()
{
#ifdef _DEBUG 
	YAML::Node settingsRoot = YAML::LoadFile("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\scripts\\SunRiseData\\Config.yml");
#else
	auto dir = GetExeDirectory();
	YAML::Node settingsRoot = YAML::LoadFile(dir + "\\scripts\\SunRiseData\\Config.yml");
#endif

	const auto& settings = settingsRoot["Config"];

	g_Config.ForceTime = settings["ForceTime"].as<float>();
	g_Config.LightLodDistance = 150.0f;
	g_Config.SunRise = 0.2f;
	g_Config.SunSet = 0.8f;
	g_Config.LightsOn = settings["LightsOn"].as<float>();
	g_Config.LightsOff = settings["LightsOff"].as<float>();
	g_Config.LightCellSize = 128.0f;
	g_Config.Console = settings["Console"].as<bool>();
	g_Config.ShaderLoader = settings["ShaderLoader"].as<bool>();
}

void InitConfig()
{
	LoadConfig();

	LoadLightFlareConfig();

	LoadSpotLightConfig();

	LoadWeatherConfig();

	InitTextureConfig();
}