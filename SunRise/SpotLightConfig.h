#pragma once
#include <d3dx9.h>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "Spotlight.h"

inline std::unordered_map<int, SolidLights> FrontEndLights;
inline std::vector<SolidLights> SolidLightsList;
inline SpotLight CarHeadlighsConfig;
inline SpotLight CarBrakeLightsOnConfig;
inline SpotLight CarBrakeLightsOffConfig;
inline SpotLightModel HelicopterLightConfig;

void ParseSpotLight(SpotLight& spotLight, const YAML::Node& spot)
{
	spotLight.Position = ParseVec3(spot["Position"]);
	spotLight.Direction = ParseVec3(spot["Direction"]);
	spotLight.Color = ParseVec3(spot["Color"]);
	spotLight.InnerAngle = spot["InnerAngle"].as<float>();
	spotLight.OuterAngle = spot["OuterAngle"].as<float>();
	spotLight.Intensity = spot["Intensity"].as<float>();
	spotLight.Range = spot["Range"].as<float>();
}

FlareModel* GetFlareModel(const YAML::Node& lightNode)
{
	auto flareNode = lightNode["Flare"];
	if (flareNode.IsDefined())
	{
		auto flareName = flareNode.as<std::string>();

		for (int i = 0; i < FlareList.size(); i++)
		{
			if (FlareList[i].Name == flareName)
			{
				return &FlareList[i];
			}
		}
	}

	return NULL;
}

void LoadSpotLightConfig()
{
	SolidLightsList.clear();
	FrontEndLights.clear();

	YAML::Node spotlightsRoot = YAML::LoadFile(GetConfigFolder("SpotLights.yml"));

	ParseSpotLight(CarHeadlighsConfig, spotlightsRoot["CarHeadLights"]);
	ParseSpotLight(CarBrakeLightsOnConfig, spotlightsRoot["CarBrakeLightsOn"]);
	ParseSpotLight(CarBrakeLightsOffConfig, spotlightsRoot["CarBrakeLightsOff"]);

	auto heliNode = spotlightsRoot["HelicopterLight"];
	ParseSpotLight(HelicopterLightConfig.Light, heliNode);
	HelicopterLightConfig.Flare = GetFlareModel(heliNode);

	const auto& spotLights = spotlightsRoot["SolidLights"];
	for (const auto& lightNode : spotLights)
	{
		SolidLights solid;
		solid.Name = lightNode["Solid"].as<std::string>();
		solid.HashA = Game::bStringHash(solid.Name.c_str());

		solid.Blink = YmlGet(lightNode, "Blink", 0);

		auto nameB = lightNode["SolidLod"];
		if (nameB.IsDefined())
		{
			solid.HashB = Game::bStringHash(nameB.as<std::string>().c_str());
		}

		solid.Flare = GetFlareModel(lightNode);

		const auto& spotLights = lightNode["SpotLights"];
		for (const auto& spot : spotLights)
		{
			SpotLight spotLight;
			ParseSpotLight(spotLight, spot);

			solid.Lights.push_back(spotLight);
		}

		SolidLightsList.push_back(solid);
	}

	const auto& frontEndLights = spotlightsRoot["FrontEndLights"];
	for (const auto& felightNode : frontEndLights)
	{
		int type = felightNode["Type"].as<int>();
		const auto& spotLights = felightNode["SpotLights"];

		FrontEndLights[type].Flare = GetFlareModel(felightNode);

		for (const auto& spot : spotLights)
		{
			SpotLight spotLight;
			ParseSpotLight(spotLight, spot);

			FrontEndLights[type].Lights.push_back(spotLight);
		}
	}
}
