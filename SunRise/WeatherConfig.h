#pragma once
#include <vector>
#include <d3dx9.h>
#include <yaml-cpp/yaml.h>
#include "Config.h"

void LoadWeatherData(WeatherData* config, const YAML::Node& node, WeatherData* cfg)
{
	config->DiffuseColor = ParseVec3To4(node["DiffuseColor"], cfg->DiffuseColor);
	config->AmbientColor = ParseVec3To4(node["AmbientColor"], cfg->AmbientColor);
	config->SpecularColor = ParseVec3To4(node["SpecularColor"], cfg->SpecularColor);
	config->SpecularPower = YmlGet<float>(node, "SpecularPower", cfg->SpecularPower);

	config->DiffuseIntensity = YmlGet<float>(node, "DiffuseIntensity", cfg->DiffuseIntensity);
	config->AmbientIntensity = YmlGet<float>(node, "AmbientIntensity", cfg->AmbientIntensity);
	config->CarDiffuseIntensity = YmlGet<float>(node, "CarDiffuseIntensity", cfg->CarDiffuseIntensity);
	config->CarAmbientIntensity = YmlGet<float>(node, "CarAmbientIntensity", cfg->CarAmbientIntensity);

	config->SkyBeta = ParseVec3To4(node["SkyBeta"], cfg->SkyBeta);
	config->SkyRayleigh = YmlGet<float>(node, "SkyRayleigh", cfg->SkyRayleigh);
	config->SkyMie = YmlGet<float>(node, "SkyMie", cfg->SkyMie);
	config->SkyBrightness = YmlGet<float>(node, "SkyBrightness", cfg->SkyBrightness);

	config->FogColor = ParseVec3To4(node["FogColor"], cfg->FogColor);
	config->FogSunColor = ParseVec3To4(node["FogSunColor"], cfg->FogSunColor);
	config->FogStart = YmlGet<float>(node, "FogStart", cfg->FogStart);
	config->FogEnd = YmlGet<float>(node, "FogEnd", cfg->FogEnd);
	config->FogPower = YmlGet<float>(node, "FogPower", cfg->FogPower);
	config->FogExponent = YmlGet<float>(node, "FogExponent", cfg->FogExponent);

	config->CarLightsPower = YmlGet<float>(node, "CarLightsPower", cfg->CarLightsPower);
	config->TextureLightPower = YmlGet<float>(node, "TextureLightPower", cfg->TextureLightPower);

	config->CloudColor = ParseVec4(node["CloudColor"], cfg->CloudColor);
	config->MoonColor = ParseVec4(node["MoonColor"], cfg->MoonColor);
	config->MoonSize = YmlGet<float>(node, "MoonSize", cfg->MoonSize);

	config->GodRaysColor = ParseVec3To4(node["GodRaysColor"], cfg->GodRaysColor);
	config->SunFlare = YmlGet<float>(node, "SunFlare", 0.0f);

	config->WaterColor = ParseVec3To4(node["WaterColor"], cfg->WaterColor);
	config->WaterSpecularPower = YmlGet<float>(node, "WaterSpecularPower", cfg->WaterSpecularPower);

	config->BloomPower = YmlGet<float>(node, "BloomPower", 1.0f);
	config->BloomThreshold = YmlGet<float>(node, "BloomThreshold", 1.5f);

	config->RoadMaskIntensity = YmlGet<float>(node, "RoadMaskIntensity", 1.0f);

	config->ExposureKey = YmlGet<float>(node, "ExposureKey", 0.3f);
	config->ExposureMin = YmlGet<float>(node, "ExposureMin", 1.0f);
	config->ExposureMax = YmlGet<float>(node, "ExposureMax", 1.0f);
}

void LoadWeatherConfig()
{
	for (auto& config : WeatherList)
	{
		delete config;
	}

	WeatherList.clear();

	YAML::Node weatherRoot = YAML::LoadFile(GetConfigFolder("Weather.yml"));

	const auto& list = weatherRoot["Weather"];
	for (const auto& node : list)
	{
		auto config = new WeatherConfig();

		config->Time = node["Time"].as<float>();
		LoadWeatherData(&config->Main, node, &config->Main);

		auto rainNode = node["Rain"];
		LoadWeatherData(&config->Rain, rainNode, &config->Main);

		WeatherList.push_back(config);
	}
}

void SaveWeatherData(YAML::Node& node, WeatherData* data)
{
	node["DiffuseColor"] = SerializeVector3(data->DiffuseColor);
	node["AmbientColor"] = SerializeVector3(data->AmbientColor);
	node["SpecularColor"] = SerializeVector3(data->SpecularColor);
	node["SpecularPower"] = data->SpecularPower;

	node["DiffuseIntensity"] = data->DiffuseIntensity;
	node["AmbientIntensity"] = data->AmbientIntensity;

	node["CarDiffuseIntensity"] = data->CarDiffuseIntensity;
	node["CarAmbientIntensity"] = data->CarAmbientIntensity;

	node["GodRaysColor"] = SerializeVector4(data->GodRaysColor);
	node["SunFlare"] = data->SunFlare;

	node["CloudColor"] = SerializeVector4(data->CloudColor);
	node["MoonColor"] = SerializeVector4(data->MoonColor);
	node["MoonSize"] = data->MoonSize;

	node["WaterColor"] = SerializeVector3(data->WaterColor);
	node["WaterSpecularPower"] = data->WaterSpecularPower;

	node["CarLightsPower"] = data->CarLightsPower;
	node["TextureLightPower"] = data->TextureLightPower;

	node["BloomPower"] = data->BloomPower;
	node["BloomThreshold"] = data->BloomThreshold;

	node["RoadMaskIntensity"] = data->RoadMaskIntensity;

	node["SkyBeta"] = SerializeVector3(data->SkyBeta);
	node["SkyMie"] = data->SkyMie;
	node["SkyRayleigh"] = data->SkyRayleigh;
	node["SkyBrightness"] = data->SkyBrightness;

	node["FogColor"] = SerializeVector3(data->FogColor);
	node["FogSunColor"] = SerializeVector3(data->FogSunColor);
	node["FogStart"] = data->FogStart;
	node["FogEnd"] = data->FogEnd;
	node["FogPower"] = data->FogPower;
	node["FogExponent"] = data->FogExponent;

	node["ExposureKey"] = data->ExposureKey;
	node["ExposureMin"] = data->ExposureMin;
	node["ExposureMax"] = data->ExposureMax;
}

void SaveWeatherConfig()
{
	YAML::Node weatherList;
	for (auto weather : WeatherList)
	{
		YAML::Node weatherNode;

		weatherNode["Time"] = weather->Time;
		SaveWeatherData(weatherNode, &weather->Main);

		YAML::Node rainNode;
		SaveWeatherData(rainNode, &weather->Rain);
		weatherNode["Rain"] = rainNode;

		weatherList.push_back(weatherNode);
	}

	YAML::Node weatherRoot;
	weatherRoot["Weather"] = weatherList;

	std::ofstream fout(GetConfigFolder("Weather.yml"));
	fout << weatherRoot;
	fout.close();
}