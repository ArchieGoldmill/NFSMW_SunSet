#pragma once
#include <vector>
#include <yaml-cpp/yaml.h>
#include "Spotlight.h"
#include "LightFlare.h"

inline std::vector<FlareModel> FlareList;

void LoadLightFlareConfig()
{
	FlareList.clear();

	auto dir = GetExeDirectory();
	YAML::Node flaresRoot = YAML::LoadFile(dir + "\\scripts\\SunSetData\\Flares.yml");

	const auto& flares = flaresRoot["Flares"];

	for (const auto& flareNode : flares)
	{
		FlareModel flare;

		flare.Name = flareNode["Name"].as<std::string>();
		flare.Intensity = flareNode["Intensity"].as<float>();
		flare.Size = flareNode["Size"].as<float>();
		flare.Type = (eLightFlareType)flareNode["Type"].as<int>();
		flare.TextureName = Game::bStringHash(flareNode["Texture"].as<std::string>().c_str());

		FlareList.push_back(flare);
	}
}