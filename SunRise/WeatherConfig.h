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

	D3DXVECTOR4 FogColor;
	float FogStart;
	float FogEnd;
	float FogPower;
	float FogExponent;
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

	config->SkyBetaR = ParseVec3To4(node["SkyBetaR"], cfg->SkyBetaR);
	config->SkyBetaM = ParseVec3To4(node["SkyBetaM"], cfg->SkyBetaM);

	config->SkyRayleigh = YmlGet<float>(node, "SkyRayleigh", cfg->SkyRayleigh);
	config->SkyRayleighAtt = YmlGet<float>(node, "SkyRayleighAtt", cfg->SkyRayleighAtt);
	config->SkyMieAtt = YmlGet<float>(node, "SkyMieAtt", cfg->SkyMieAtt);
	config->SkyGamma = YmlGet<float>(node, "SkyGamma", cfg->SkyGamma);
	config->SkyG = YmlGet<float>(node, "SkyG", cfg->SkyG);

	config->FogColor = ParseVec3To4(node["FogColor"], cfg->FogColor);
	config->FogStart = YmlGet<float>(node, "FogStart", cfg->FogStart);
	config->FogEnd = YmlGet<float>(node, "FogEnd", cfg->FogEnd);
	config->FogPower = YmlGet<float>(node, "FogPower", cfg->FogPower);
	config->FogExponent = YmlGet<float>(node, "FogExponent", cfg->FogExponent);

	config->CloudColor = ParseVec4(node["CloudColor"], cfg->CloudColor);
}

void LoadWeatherConfig()
{
	for (auto& config : WeatherList)
	{
		delete config;
	}

	WeatherList.clear();

	auto dir = GetExeDirectory();
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