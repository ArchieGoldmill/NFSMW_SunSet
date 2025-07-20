#pragma once
#include <yaml-cpp/yaml.h>
#include "Spotlight.h"
#include "Game.h"

inline std::vector<SolidLights> SolidLightsList;

D3DXVECTOR3 ParseVec3(const YAML::Node& node) {
	return D3DXVECTOR3(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
}

void LoadConfig()
{
	SolidLightsList.clear();

	YAML::Node spotlightsRoot = YAML::LoadFile("SunRiseData\\SpotLights.yml");

	const auto& spotLights = spotlightsRoot["SolidLights"];

	for (const auto& lightNode : spotLights)
	{
		SolidLights solid;
		solid.Name = lightNode["Solid"].as<std::string>();
		solid.Flare = lightNode["Flare"].as<std::string>();
		solid.Hash = Game::bStringHash(solid.Name.c_str());

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