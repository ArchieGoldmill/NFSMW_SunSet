#pragma once
#include <yaml-cpp/yaml.h>
#include "Game.h"
#include "WeatherConfig.h"
#include "FlareConfig.h"
#include "SpotLightConfig.h"

struct Config
{
	float ForceTime;
	float LightMaxDistance;
	float LightLodDistance;
	float SunRise;
	float SunSet;
	float LightsOn;
	float LightsOff;
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
	g_Config.LightLodDistance = settings["LightLodDistance"].as<float>();
	g_Config.LightMaxDistance = settings["LightMaxDistance"].as<float>();
	g_Config.SunRise = settings["SunRise"].as<float>();
	g_Config.SunSet = settings["SunSet"].as<float>();
	g_Config.LightsOn = settings["LightsOn"].as<float>();
	g_Config.LightsOff = settings["LightsOff"].as<float>();
}

void InitConfig()
{
	LoadConfig();

	LoadLightFlareConfig();

	LoadSpotLightConfig();

	LoadWeatherConfig();
}