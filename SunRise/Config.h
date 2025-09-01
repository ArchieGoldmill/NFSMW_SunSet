#pragma once
#include "Game.h"
#include "WeatherConfig.h"
#include "FlareConfig.h"
#include "SpotLightConfig.h"
#include "TextureConfig.h"
#include "CustomMeshesConfig.h"

inline const char* ModName = "NFSMW - Sun Set 1.3";

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
	bool DepthPrepass;
	bool CarVinylPaintFix;
	bool BrakeGlow;

	float WetnessTime;
	float DryTime;

	float BlurMinSpeed;
	float BlurMaxSpeed;
	float BlurDepth;

	int HK_ShaderReload;
	int HK_ToggleEditor;
};

inline Config g_Config;

void LoadConfig()
{
	YAML::Node settingsRoot = YAML::LoadFile(GetConfigFolder("Config.yml"));

	const auto& settings = settingsRoot["Config"];
	g_Config.LightLodDistance = 150.0f;
	g_Config.LightCellSize = 64;
	g_Config.Console = settings["Console"].as<bool>();
	g_Config.ShaderLoader = settings["ShaderLoader"].as<bool>();
	g_Config.ShaderCompiler = settings["ShaderCompiler"].as<bool>();
	g_Config.LiveReload = settings["LiveReload"].as<bool>();
	g_Config.X360Effects = settings["X360Effects"].as<bool>();
	g_Config.Editor = settings["Editor"].as<bool>();
	g_Config.SkipFE = settings["SkipFE"].as<bool>();
	g_Config.CarVinylPaintFix = settings["CarVinylPaintFix"].as<bool>();
	g_Config.BrakeGlow = settings["BrakeGlow"].as<bool>();
	g_Config.DepthPrepass = true;

	const auto& time = settingsRoot["Time"];
	g_Config.WetnessTime = time["WetnessTime"].as<float>();
	g_Config.DryTime = time["DryTime"].as<float>();
	g_Config.ForceTime = time["ForceTime"].as<float>();
	g_Config.TimeUpdateRate = time["TimeUpdateRate"].as<float>();
	g_Config.LightsOn = time["LightsOn"].as<float>();
	g_Config.LightsOff = time["LightsOff"].as<float>();
	g_Config.RandomStartupTime = time["RandomStartupTime"].as<bool>();
	g_Config.RealTime = time["RealTime"].as<bool>();
	g_Config.SunRise = 0.2f;
	g_Config.SunSet = 0.8f;

	const auto& blur = settingsRoot["Blur"];
	g_Config.BlurMinSpeed = blur["MinSpeed"].as<float>();
	g_Config.BlurMaxSpeed = blur["MaxSpeed"].as<float>();
	g_Config.BlurDepth = blur["Depth"].as<float>();

	const auto& hotkeys = settingsRoot["Hotkeys"];
	g_Config.HK_ShaderReload = hotkeys["ShaderReload"].as<int>();
	g_Config.HK_ToggleEditor = hotkeys["ToggleEditor"].as<int>();

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

	LoadTextureConfig();

	LoadCustomMeshes();
}