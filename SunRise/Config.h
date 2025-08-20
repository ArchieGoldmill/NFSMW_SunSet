#pragma once
#include "Game.h"
#include "WeatherConfig.h"
#include "FlareConfig.h"
#include "SpotLightConfig.h"
#include "TextureConfig.h"

inline const char* ModName = "NFSMW - Sun Set 1.2";

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

	bool Console;
	bool ShaderLoader;
	bool ShaderCompiler;
	bool LiveReload;
	bool X360Effects;
	bool Editor;
	bool SkipFE;
	bool RandomStartupTime;
	bool RealTime;

	float WetnessTime;
	float DryTime;

	int HK_ShaderReload;
};

inline Config g_Config;

void LoadConfig()
{
	YAML::Node settingsRoot = YAML::LoadFile(GetConfigFolder("Config.yml"));

	const auto& settings = settingsRoot["Config"];

	g_Config.ForceTime = settings["ForceTime"].as<float>();
	g_Config.TimeUpdateRate = settings["TimeUpdateRate"].as<float>();
	g_Config.LightLodDistance = 150.0f;
	g_Config.SunRise = 0.2f;
	g_Config.SunSet = 0.8f;
	g_Config.LightsOn = settings["LightsOn"].as<float>();
	g_Config.LightsOff = settings["LightsOff"].as<float>();
	g_Config.LightCellSize = 64;

	g_Config.Console = settings["Console"].as<bool>();
	g_Config.ShaderLoader = settings["ShaderLoader"].as<bool>();
	g_Config.ShaderCompiler = settings["ShaderCompiler"].as<bool>();
	g_Config.LiveReload = settings["LiveReload"].as<bool>();
	g_Config.X360Effects = settings["X360Effects"].as<bool>();
	g_Config.Editor = settings["Editor"].as<bool>();
	g_Config.SkipFE = settings["SkipFE"].as<bool>();
	g_Config.RandomStartupTime = settings["RandomStartupTime"].as<bool>();
	g_Config.RealTime = settings["RealTime"].as<bool>();

	g_Config.HK_ShaderReload = settings["HK_ShaderReload"].as<int>();

	const auto& weather = settingsRoot["Weather"];
	g_Config.WetnessTime = weather["WetnessTime"].as<float>();
	g_Config.DryTime = weather["DryTime"].as<float>();

	if (g_Config.Editor)
	{
		g_Config.LiveReload = false;
	}
}

void InitConfig()
{
	LoadConfig();

	LoadLightFlareConfig();

	LoadSpotLightConfig();

	LoadWeatherConfig();

	InitTextureConfig();
}