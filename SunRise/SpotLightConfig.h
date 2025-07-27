#pragma once
#include <d3dx9.h>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "Spotlight.h"

inline std::vector<SolidLights> SolidLightsList;
inline SpotLight CarHeadlighsConfig;
inline SpotLight CarBrakeLightsOnConfig;
inline SpotLight CarBrakeLightsOffConfig;
inline SpotLight HelicopterLightConfig;

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

void LoadSpotLightConfig()
{
	SolidLightsList.clear();

#ifdef _DEBUG 
	YAML::Node spotlightsRoot = YAML::LoadFile("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\scripts\\SunRiseData\\SpotLights.yml");
#else
	auto dir = GetExeDirectory();
	YAML::Node spotlightsRoot = YAML::LoadFile(dir + "\\scripts\\SunRiseData\\SpotLights.yml");
#endif

	ParseSpotLight(CarHeadlighsConfig, spotlightsRoot["CarHeadLights"]);
	ParseSpotLight(CarBrakeLightsOnConfig, spotlightsRoot["CarBrakeLightsOn"]);
	ParseSpotLight(CarBrakeLightsOffConfig, spotlightsRoot["CarBrakeLightsOff"]);
	ParseSpotLight(HelicopterLightConfig, spotlightsRoot["HelicopterLight"]);

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

		auto flareNode = lightNode["Flare"];
		if (flareNode.IsDefined())
		{
			auto flareName = flareNode.as<std::string>();
			for (int i = 0; i < FlareList.size(); i++)
			{
				if (FlareList[i].Name == flareName)
				{
					solid.Flare = &FlareList[i];
					break;
				}
			}
		}

		const auto& spotLights = lightNode["SpotLights"];
		for (const auto& spot : spotLights)
		{
			SpotLight spotLight;
			ParseSpotLight(spotLight, spot);

			solid.Lights.push_back(spotLight);
		}

		SolidLightsList.push_back(solid);
	}
}
