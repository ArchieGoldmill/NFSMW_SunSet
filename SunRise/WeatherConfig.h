#pragma once
#include <vector>
#include <d3dx9.h>
#include <yaml-cpp/yaml.h>

struct WeatherData
{
	D3DXVECTOR4 DiffuseColor;
	D3DXVECTOR4 AmbientColor;
	D3DXVECTOR4 SpecularColor;
	float SpecularPower;

	D3DXVECTOR4 SkyBetaR;
	D3DXVECTOR4 SkyBetaM;
	float SkyRayleigh;
	float SkyRayleighAtt;
	float SkyMie;
	float SkyMieAtt;
	float SkyGamma;
	float SkyG;

	D3DXVECTOR4 CloudColor;
	D3DXVECTOR4 WaterColor;

	D3DXVECTOR4 FogColor;
	D3DXVECTOR4 FogSunColor;
	float FogStart;
	float FogEnd;
	float FogPower;
	float FogExponent;

	float CarLightsPower = 1.0;
	float TextureLightPower = 1.0;
};

struct WeatherConfig
{
	float Time;
	WeatherData Main;
	WeatherData Rain;
};

inline std::vector<WeatherConfig*> WeatherList;

void LoadWeatherData(WeatherData* config, const YAML::Node& node, WeatherData* cfg)
{
	config->DiffuseColor = ParseVec3To4(node["DiffuseColor"], cfg->DiffuseColor);
	config->AmbientColor = ParseVec3To4(node["AmbientColor"], cfg->AmbientColor);
	config->SpecularColor = ParseVec3To4(node["SpecularColor"], cfg->SpecularColor);
	config->SpecularPower = YmlGet<float>(node, "SpecularPower", cfg->SpecularPower);
	config->DiffuseColor.w = YmlGet<float>(node, "DiffuseIntensity", cfg->DiffuseColor.w);

	config->SkyBetaR = ParseVec3To4(node["SkyBetaR"], cfg->SkyBetaR);
	config->SkyBetaM = ParseVec3To4(node["SkyBetaM"], cfg->SkyBetaM);

	config->SkyRayleigh = YmlGet<float>(node, "SkyRayleigh", cfg->SkyRayleigh);
	config->SkyRayleighAtt = YmlGet<float>(node, "SkyRayleighAtt", cfg->SkyRayleighAtt);
	config->SkyMie = YmlGet<float>(node, "SkyMie", cfg->SkyMie);
	config->SkyMieAtt = YmlGet<float>(node, "SkyMieAtt", cfg->SkyMieAtt);
	config->SkyGamma = YmlGet<float>(node, "SkyGamma", cfg->SkyGamma);
	config->SkyG = YmlGet<float>(node, "SkyG", cfg->SkyG);

	config->FogColor = ParseVec3To4(node["FogColor"], cfg->FogColor);
	config->FogSunColor = ParseVec3To4(node["FogSunColor"], cfg->FogSunColor);
	config->FogStart = YmlGet<float>(node, "FogStart", cfg->FogStart);
	config->FogEnd = YmlGet<float>(node, "FogEnd", cfg->FogEnd);
	config->FogPower = YmlGet<float>(node, "FogPower", cfg->FogPower);
	config->FogExponent = YmlGet<float>(node, "FogExponent", cfg->FogExponent);

	config->CarLightsPower = YmlGet<float>(node, "CarLightsPower", cfg->CarLightsPower);
	config->TextureLightPower = YmlGet<float>(node, "TextureLightPower", cfg->TextureLightPower);

	config->CloudColor = ParseVec4(node["CloudColor"], cfg->CloudColor);
	config->WaterColor = ParseVec3To4(node["WaterColor"], cfg->WaterColor);
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
	node["DiffuseIntensity"] = data->DiffuseColor.w;

	node["CloudColor"] = SerializeVector4(data->CloudColor);
	node["WaterColor"] = SerializeVector3(data->WaterColor);

	node["CarLightsPower"] = data->CarLightsPower;
	node["TextureLightPower"] = data->TextureLightPower;

	node["SkyBetaR"] = SerializeVector3(data->SkyBetaR);
	node["SkyBetaM"] = SerializeVector3(data->SkyBetaM);
	node["SkyMie"] = data->SkyMie;
	node["SkyMieAtt"] = data->SkyMieAtt;
	node["SkyRayleigh"] = data->SkyRayleigh;
	node["SkyRayleighAtt"] = data->SkyRayleighAtt;
	node["SkyGamma"] = data->SkyGamma;
	node["SkyG"] = data->SkyG;

	node["FogColor"] = SerializeVector3(data->FogColor);
	node["FogSunColor"] = SerializeVector3(data->FogSunColor);
	node["FogStart"] = data->FogStart;
	node["FogEnd"] = data->FogEnd;
	node["FogPower"] = data->FogPower;
	node["FogExponent"] = data->FogExponent;
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