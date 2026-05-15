#pragma once
#include "Config.h"
#include "Game.h"
#include "WeatherConfig.h"
#include "FlareConfig.h"
#include "SpotLightConfig.h"
#include "TextureConfig.h"
#include "CustomMeshesConfig.h"

void LoadConfig()
{
	YAML::Node settingsRoot = YAML::LoadFile(GetConfigFolder("Config.yml"));

	const auto& settings = settingsRoot["Config"];
	g_Config.LightLodDistance = 300.0f;
	g_Config.LightCellSize = 64;
	g_Config.Console = YmlGet(settings, "Console", false);
	g_Config.ShaderLoader = YmlGet(settings, "ShaderLoader", true);
	g_Config.ShaderCompiler = YmlGet(settings, "ShaderCompiler", false);
	g_Config.LiveReload = YmlGet(settings, "LiveReload", false);
	g_Config.X360Effects = YmlGet(settings, "X360Effects", false);
	g_Config.Editor = YmlGet(settings, "Editor", false);
	g_Config.SkipFE = YmlGet(settings, "SkipFE", false);
	g_Config.CarVinylPaintFix = YmlGet(settings, "CarVinylPaintFix", true);
	g_Config.BrakeGlow = YmlGet(settings, "BrakeGlow", true);
	g_Config.GodRays = YmlGet(settings, "GodRays", true);
	g_Config.Bloom = YmlGet(settings, "Bloom", true);
	g_Config.RealFeReflections = YmlGet(settings, "RealFeReflections", true);
	g_Config.WorldVertexColor = YmlGet(settings, "WorldVertexColor", false);
	g_Config.CarVertexColor = YmlGet(settings, "CarVertexColor", true);
	g_Config.TunnelWetnessFix = YmlGet(settings, "TunnelWetnessFix", true);
	g_Config.TunnelLightIntensity = YmlGet(settings, "TunnelLightIntensity", true);
	g_Config.DisableNightShadows = YmlGet(settings, "DisableNightShadows", true);
	g_Config.MSAAx8 = YmlGet(settings, "MSAAx8", false);
	g_Config.Tonemapping = YmlGet(settings, "Tonemapping", false);
	g_Config.DisableSunFlare = YmlGet(settings, "DisableSunFlare", false);
	g_Config.RoadblockLights = YmlGet(settings, "RoadblockLights", false);
	g_Config.CustomMeshDist = YmlGet(settings, "CustomMeshDist", 500.0f);
	g_Config.FlareDistance = YmlGet(settings, "FlareDistance", 200.0f);
	g_Config.MaxLights = YmlGet(settings, "MaxLights", 500);
	g_Config.EditorScale = YmlGet(settings, "EditorScale", 1.0);
	g_Config.Exposure = YmlGet(settings, "Exposure", false);
	g_Config.NosLight = YmlGet(settings, "NosLight", false);
	g_Config.NosEmmiter = YmlGet(settings, "NosEmmiter", false);
	g_Config.NosFix = YmlGet(settings, "NosFix", false);

	const auto& time = settingsRoot["Time"];
	g_Config.WetTime = YmlGet(time, "WetTime", 2.0f);
	g_Config.DryTime = YmlGet(time, "DryTime", 30.0f);
	g_Config.ForceTime = YmlGet(time, "ForceTime", -1.0f);
	g_Config.TimeUpdateRate = YmlGet(time, "TimeUpdateRate", 1.0f);
	g_Config.LightsOn = YmlGet(time, "LightsOn", 0.77f);
	g_Config.LightsOff = YmlGet(time, "LightsOff", 0.24f);
	g_Config.RandomStartupTime = YmlGet(time, "RandomStartupTime", false);
	g_Config.RandomWindows = YmlGet(time, "RandomWindows", false);
	g_Config.RealTime = YmlGet(time, "RealTime", false);
	g_Config.SunAzimuth = YmlGet(time, "SunAzimuth", 0.0f);
	g_Config.SunRise = 0.2f;
	g_Config.SunSet = 0.8f;

	const auto& blur = settingsRoot["Blur"];
	g_Config.BlurMinSpeed = YmlGet(blur, "MinSpeed", 25.0f);
	g_Config.BlurMaxSpeed = YmlGet(blur, "MaxSpeed", 325.0f);
	g_Config.BlurDepth = YmlGet(blur, "Depth", 150.0f);

	const auto& hotkeys = settingsRoot["Hotkeys"];
	g_Config.HK_ShaderReload = YmlGet(hotkeys, "ShaderReload", 0);
	g_Config.HK_ToggleEditor = YmlGet(hotkeys, "ToggleEditor", 0);

	const auto& rain = settingsRoot["Rain"];
	g_Config.Rain.DryTime = YmlGet(rain, "DryTime", 10.0f);
	g_Config.Rain.DryTimeRandom = YmlGet(rain, "DryTimeRandom", 5.0f);
	g_Config.Rain.LightTime = YmlGet(rain, "LightTime", 1.0f);
	g_Config.Rain.LightTimeRandom = YmlGet(rain, "LightTimeRandom", 1.0f);
	g_Config.Rain.HeavyTime = YmlGet(rain, "HeavyTime", 3.0f);
	g_Config.Rain.HeavyTimeRandom = YmlGet(rain, "HeavyTimeRandom", 3.0f);
	g_Config.Rain.LightningTimeOut = YmlGet(rain, "LightningTimeOut", 15.0f);
	g_Config.Rain.RainCloudsTexture = YmlGet(rain, "RainCloudsTexture", true);

	const auto& windowGlow = settingsRoot["WindowGlow"];
	g_Config.WindowGlowColor = ParseVec3To4(windowGlow["Color"]);
	g_Config.WindowGlowPower = YmlGet(windowGlow, "Power", 5.0f);
	g_Config.WindowGlowOverride = YmlGet(windowGlow, "Override", true);

	const auto& filter = settingsRoot["Filter"];
	g_Config.Filter.SetString(filter["Texture"].as<std::string>());
	g_Config.FilterPower = YmlGet(filter, "Power", 1.0f);

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
	config["WorldVertexColor"] = g_Config.WorldVertexColor;
	config["CarVertexColor"] = g_Config.CarVertexColor;
	config["TunnelWetnessFix"] = g_Config.TunnelWetnessFix;
	config["TunnelLightIntensity"] = g_Config.TunnelLightIntensity;
	config["DisableNightShadows"] = g_Config.DisableNightShadows;
	config["MSAAx8"] = g_Config.MSAAx8;
	config["Tonemapping"] = g_Config.Tonemapping;
	config["CustomMeshDist"] = g_Config.CustomMeshDist;
	config["FlareDistance"] = g_Config.FlareDistance;
	config["MaxLights"] = g_Config.MaxLights;
	config["DisableSunFlare"] = g_Config.DisableSunFlare;
	config["RoadblockLights"] = g_Config.RoadblockLights;
	config["EditorScale"] = g_Config.EditorScale;
	config["Exposure"] = g_Config.Exposure;
	config["NosLight"] = g_Config.NosLight;
	config["NosEmmiter"] = g_Config.NosEmmiter;
	config["NosFix"] = g_Config.NosFix;

	YAML::Node time;
	time["ForceTime"] = g_Config.ForceTime;
	time["TimeUpdateRate"] = g_Config.TimeUpdateRate;
	time["LightsOn"] = g_Config.LightsOn;
	time["LightsOff"] = g_Config.LightsOff;
	time["WetTime"] = g_Config.WetTime;
	time["DryTime"] = g_Config.DryTime;
	time["DryTime"] = g_Config.DryTime;
	time["RandomStartupTime"] = g_Config.RandomStartupTime;
	time["RandomWindows"] = g_Config.RandomWindows;
	time["RealTime"] = g_Config.RealTime;
	time["SunAzimuth"] = g_Config.SunAzimuth;

	YAML::Node blur;
	blur["MinSpeed"] = g_Config.BlurMinSpeed;
	blur["MaxSpeed"] = g_Config.BlurMaxSpeed;
	blur["Depth"] = g_Config.BlurDepth;

	YAML::Node hotkeys;
	hotkeys["ShaderReload"] = g_Config.HK_ShaderReload;
	hotkeys["ToggleEditor"] = g_Config.HK_ToggleEditor;

	YAML::Node rain;
	rain["DryTime"] = g_Config.Rain.DryTime;
	rain["DryTimeRandom"] = g_Config.Rain.DryTimeRandom;
	rain["LightTime"] = g_Config.Rain.LightTime;
	rain["LightTimeRandom"] = g_Config.Rain.LightTimeRandom;
	rain["HeavyTime"] = g_Config.Rain.HeavyTime;
	rain["HeavyTimeRandom"] = g_Config.Rain.HeavyTimeRandom;
	rain["LightningTimeOut"] = g_Config.Rain.LightningTimeOut;
	rain["RainCloudsTexture"] = g_Config.Rain.RainCloudsTexture;

	YAML::Node windowGlow;
	windowGlow["Override"] = g_Config.WindowGlowOverride;
	windowGlow["Color"] = SerializeVector3(g_Config.WindowGlowColor);
	windowGlow["Power"] = g_Config.WindowGlowPower;

	YAML::Node filter;
	filter["Texture"] = g_Config.Filter.str;
	filter["Power"] = g_Config.FilterPower;

	root["Config"] = config;
	root["Time"] = time;
	root["Blur"] = blur;
	root["Hotkeys"] = hotkeys;
	root["Rain"] = rain;
	root["WindowGlow"] = windowGlow;
	root["Filter"] = filter;

	std::ofstream fout(GetConfigFolder("Config.yml"));
	fout << root;
	fout.close();
}



void InitConfig()
{
	try
	{
		LoadConfig();

		LoadLightFlareConfig();

		LoadSpotLightConfig();

		LoadWeatherConfig();

		LoadTextureConfig();

		LoadCustomMeshes();
	}
	catch (const YAML::Exception& e)
	{
		MessageBoxA(NULL, (std::string("Config parse error: ") + e.what()).c_str(), ModName, MB_ICONERROR);
		exit(1);
	}

	if (g_Config.Editor)
	{
		PrelitTextures.PopulateUiList();
		SolidLightsList.PopulateUiList();
	}
}