#pragma once
#include <vector>
#include <d3dx9.h>
#include <yaml-cpp/yaml.h>

struct WeatherConfig
{
	float Time;
	D3DXVECTOR4 DiffuseColor;
	D3DXVECTOR4 AmbientColor;
	D3DXVECTOR4 SpecularColor;
	float SpecularPower;
};

inline std::vector<WeatherConfig*> WeatherList;

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
		config->DiffuseColor = ParseVec3To4(node["DiffuseColor"]);
		config->AmbientColor = ParseVec3To4(node["AmbientColor"]);
		config->SpecularColor = ParseVec3To4(node["SpecularColor"]);
		config->SpecularPower = node["SpecularPower"].as<float>();

		WeatherList.push_back(config);
	}
}