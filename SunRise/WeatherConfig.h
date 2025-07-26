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

void LoadWeatherData(WeatherData* config, const YAML::Node& node)
{
	config->DiffuseColor = ParseVec3To4(node["DiffuseColor"]);
	config->AmbientColor = ParseVec3To4(node["AmbientColor"]);
	config->SpecularColor = ParseVec3To4(node["SpecularColor"]);
	config->SpecularPower = node["SpecularPower"].as<float>();

	config->SkyBetaR = ParseVec3To4(node["SkyBetaR"]);
	config->SkyBetaM = ParseVec3To4(node["SkyBetaM"]);

	config->SkyRayleigh = node["SkyRayleigh"].as<float>();
	config->SkyRayleighAtt = node["SkyRayleighAtt"].as<float>();
	config->SkyMie = node["SkyMie"].as<float>();
	config->SkyMieAtt = node["SkyMieAtt"].as<float>();
	config->SkyGamma = node["SkyGamma"].as<float>();
	config->SkyG = node["SkyG"].as<float>();

	config->FogColor = ParseVec3To4(node["FogColor"]);
	config->FogStart = node["FogStart"].as<float>();
	config->FogEnd = node["FogEnd"].as<float>();
	config->FogPower = node["FogPower"].as<float>();
	config->FogExponent = node["FogExponent"].as<float>();

	config->CloudColor = ParseVec4(node["CloudColor"]);
}

void LoadWeatherConfig()
{
	for (auto& config : WeatherList)
	{
		delete config;
	}

	WeatherList.clear();

#ifdef _DEBUG 
	YAML::Node weatherRoot = YAML::LoadFile("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\scripts\\SunRiseData\\Weather.yml");
#else
	auto dir = GetExeDirectory();
	YAML::Node weatherRoot = YAML::LoadFile(dir + "\\scripts\\SunRiseData\\Weather.yml");
#endif

	const auto& list = weatherRoot["Weather"];
	for (const auto& node : list)
	{
		auto config = new WeatherConfig();

		config->Time = node["Time"].as<float>();
		LoadWeatherData(&config->Main, node);

		auto rainNode = node["Rain"];
		LoadWeatherData(&config->Rain, rainNode);

		WeatherList.push_back(config);
	}
}