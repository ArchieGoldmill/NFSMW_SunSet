#pragma once
#include "Game.h"
#include "WeatherConfig.h"
#include "FlareConfig.h"
#include "SpotLightConfig.h"
#include "TextureConfig.h"
#include "CustomMeshesConfig.h"

inline const char* ModName = "NFSMW - Sun Set 1.4";

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
	bool GodRays;
	bool Bloom;
	bool RealFeReflections;

	float WetTime;
	float DryTime;

	float BlurMinSpeed;
	float BlurMaxSpeed;
	float BlurDepth;

	int HK_ShaderReload;
	int HK_ToggleEditor;

	D3DXVECTOR4 WindowGlowColor;
	float WindowGlowPower;
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
	g_Config.GodRays = settings["GodRays"].as<bool>();
	g_Config.Bloom = settings["Bloom"].as<bool>();
	g_Config.RealFeReflections = settings["RealFeReflections"].as<bool>();
	g_Config.DepthPrepass = true;

	const auto& time = settingsRoot["Time"];
	g_Config.WetTime = time["WetTime"].as<float>();
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

	const auto& defaultParams = settingsRoot["DefaultParams"];
	g_Config.WindowGlowColor = ParseVec3To4(defaultParams["WindowGlowColor"]);
	g_Config.WindowGlowPower = defaultParams["WindowGlowPower"].as<float>();

	if (g_Config.Editor)
	{
		g_Config.LiveReload = false;
	}
}

void SaveConfig()
{
	YAML::Node root;

	YAML::Node config;
	config["Console"] = g_Config.Console;
	config["ShaderLoader"] = g_Config.ShaderLoader;
	config["ShaderCompiler"] = g_Config.ShaderCompiler;
	config["X360Effects"] = g_Config.X360Effects;
	config["LiveReload"] = g_Config.LiveReload;
	config["Editor"] = g_Config.Editor;
	config["SkipFE"] = g_Config.SkipFE;
	config["CarVinylPaintFix"] = g_Config.CarVinylPaintFix;
	config["BrakeGlow"] = g_Config.BrakeGlow;
	config["GodRays"] = g_Config.GodRays;
	config["Bloom"] = g_Config.Bloom;
	config["RealFeReflections"] = g_Config.RealFeReflections;

	YAML::Node time;
	time["ForceTime"] = g_Config.ForceTime;
	time["TimeUpdateRate"] = g_Config.TimeUpdateRate;
	time["LightsOn"] = g_Config.LightsOn;
	time["LightsOff"] = g_Config.LightsOff;
	time["WetTime"] = g_Config.WetTime;
	time["DryTime"] = g_Config.DryTime;
	time["DryTime"] = g_Config.DryTime;
	time["RandomStartupTime"] = g_Config.RandomStartupTime;
	time["RealTime"] = g_Config.RealTime;

	YAML::Node blur;
	blur["MinSpeed"] = g_Config.BlurMinSpeed;
	blur["MaxSpeed"] = g_Config.BlurMaxSpeed;
	blur["Depth"] = g_Config.BlurDepth;

	YAML::Node hotkeys;
	hotkeys["ShaderReload"] = g_Config.HK_ShaderReload;
	hotkeys["ToggleEditor"] = g_Config.HK_ToggleEditor;

	YAML::Node defaultParams;
	defaultParams["WindowGlowColor"] = SerializeVector3(g_Config.WindowGlowColor);
	defaultParams["WindowGlowPower"] = g_Config.WindowGlowPower;

	root["Config"] = config;
	root["Time"] = time;
	root["Blur"] = blur;
	root["Hotkeys"] = hotkeys;
	root["DefaultParams"] = defaultParams;

	std::ofstream fout(GetConfigFolder("Config.yml"));
	fout << root;
	fout.close();
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