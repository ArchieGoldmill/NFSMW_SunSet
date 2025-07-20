#pragma once
#include <yaml-cpp/yaml.h>
#include "Spotlight.h"
#include "Game.h"

inline std::vector<SolidLights> SolidLightsList;
inline std::vector<FlareModel> FlareList;

D3DXVECTOR3 ParseVec3(const YAML::Node& node) {
	return D3DXVECTOR3(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
}

std::string GetExeDirectory() {
	char path[MAX_PATH] = { 0 };
	// Get the full path of the executable
	DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
	if (length == 0 || length == MAX_PATH) {
		// handle error
		return "";
	}

	std::string fullPath(path, length);
	// Remove the executable name to get the directory
	size_t pos = fullPath.find_last_of("\\/");
	if (pos == std::string::npos) {
		return "";  // unexpected, no directory separator
	}
	return fullPath.substr(0, pos);
}

void LoadSpotLightConfig()
{
	SolidLightsList.clear();

#ifdef _DEBUG 
	YAML::Node spotlightsRoot = YAML::LoadFile("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\scripts\\SunRiseData\\SpotLights.yml");
#else
	auto dir = GetExeDirectory();
	YAML::Node spotlightsRoot = YAML::LoadFile(dir + "\\scripts\\SunRiseData\\SpotLights.yml");
#endif

	const auto& spotLights = spotlightsRoot["SolidLights"];

	for (const auto& lightNode : spotLights)
	{
		SolidLights solid;
		solid.Name = lightNode["Solid"].as<std::string>();
		solid.Hash = Game::bStringHash(solid.Name.c_str());

		auto flareName = lightNode["Flare"].as<std::string>();
		for (int i =0; i< FlareList.size();i++)
		{
			if(FlareList[i].Name == flareName )
			{
				solid.Flare = &FlareList[i];
				break;
			}
		}

		const auto& spotLights = lightNode["SpotLights"];
		for (const auto& spot : spotLights)
		{
			SpotLight spotLight;
			spotLight.Position = ParseVec3(spot["Position"]);
			spotLight.Direction = ParseVec3(spot["Direction"]);
			spotLight.Color = ParseVec3(spot["Color"]);
			spotLight.Power = spot["Power"].as<float>();
			spotLight.Intensity = spot["Intensity"].as<float>();
			spotLight.Range = spot["Range"].as<float>();

			solid.Lights.push_back(spotLight);
		}

		SolidLightsList.push_back(solid);
	}
}

void LoadLightFlareConfig()
{
	FlareList.clear();

#ifdef _DEBUG 
	YAML::Node flaresRoot = YAML::LoadFile("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\scripts\\SunRiseData\\Flares.yml");
#else
	auto dir = GetExeDirectory();
	YAML::Node spotlightsRoot = YAML::LoadFile(dir + "\\scripts\\SunRiseData\\Flares.yml");
#endif

	const auto& flares = flaresRoot["Flares"];

	for (const auto& flareNode : flares)
	{
		FlareModel flare;

		flare.Name = flareNode["Name"].as<std::string>();
		flare.Intensity = flareNode["Intensity"].as<float>();
		flare.Size = flareNode["Size"].as<float>();
		flare.Type = (eLightFlareType)flareNode["Type"].as<int>();
		flare.Color = ParseVec3(flareNode["Color"]);


		FlareList.push_back(flare);
	}
}

void LoadConfig()
{
	LoadLightFlareConfig();

	LoadSpotLightConfig();
}