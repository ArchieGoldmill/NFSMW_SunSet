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

FlareModel* GetFlareModel(std::string flareName)
{
	for (int i = 0; i < FlareList.size(); i++)
	{
		if (FlareList[i].Name == flareName)
		{
			return &FlareList[i];
		}
	}

	return NULL;
}

FlareModel* GetFlareModel(const YAML::Node& lightNode)
{
	auto flareNode = lightNode["Flare"];
	if (flareNode.IsDefined())
	{
		auto flareName = flareNode.as<std::string>();
		return GetFlareModel(flareName);
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
		solid.NameA = lightNode["Solid"].as<std::string>();
		solid.HashA = Game::bStringHash(solid.NameA.c_str());

		solid.Blink = YmlGet(lightNode, "Blink", 0);

		auto nameB = lightNode["SolidLod"];
		if (nameB.IsDefined())
		{
			solid.NameB = nameB.as<std::string>();
			solid.HashB = Game::bStringHash(solid.NameB.c_str());
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

void SaveSpotLight(YAML::Node& lightNode, SpotLight& light)
{
	lightNode["Position"] = SerializeVector3(light.Position);
	lightNode["Direction"] = SerializeVector3(light.Direction);
	lightNode["Color"] = SerializeVector3(light.Color);
	lightNode["InnerAngle"] = light.InnerAngle;
	lightNode["OuterAngle"] = light.OuterAngle;
	lightNode["Intensity"] = light.Intensity;
	lightNode["Range"] = light.Range;
}

void SaveSpotlights(YAML::Node& spotlights, std::vector<SpotLight>& lights)
{
	for (auto& light : lights)
	{
		YAML::Node lightNode;
		SaveSpotLight(lightNode, light);

		spotlights.push_back(lightNode);
	}
}

void SaveSpotLightConfig()
{
	YAML::Node list;
	for (auto& solidLight : SolidLightsList)
	{
		YAML::Node spotlights;
		SaveSpotlights(spotlights, solidLight.Lights);

		YAML::Node node;
		node["Solid"] = solidLight.NameA;
		node["SolidLod"] = solidLight.NameB;
		node["Blink"] = solidLight.Blink;
		node["Flare"] = solidLight.Flare ? solidLight.Flare->Name : "";
		node["SpotLights"] = spotlights;

		list.push_back(node);
	}

	YAML::Node felist;
	for (auto& felight : FrontEndLights)
	{
		YAML::Node spotlights;
		SaveSpotlights(spotlights, felight.second.Lights);

		YAML::Node node;
		node["Type"] = felight.first;
		node["Flare"] = felight.second.Flare ? felight.second.Flare->Name : "";
		node["SpotLights"] = spotlights;

		felist.push_back(node);
	}

	YAML::Node carHeadlights;
	SaveSpotLight(carHeadlights, CarHeadlighsConfig);

	YAML::Node carBrakeLightsOn;
	SaveSpotLight(carBrakeLightsOn, CarBrakeLightsOnConfig);

	YAML::Node carBrakeLightsOff;
	SaveSpotLight(carBrakeLightsOff, CarBrakeLightsOffConfig);

	YAML::Node helicopterLight;
	helicopterLight["Flare"] = HelicopterLightConfig.Flare ? HelicopterLightConfig.Flare->Name : "";
	SaveSpotLight(helicopterLight, HelicopterLightConfig.Light);

	YAML::Node root;
	root["CarHeadLights"] = carHeadlights;
	root["CarBrakeLightsOn"] = carBrakeLightsOn;
	root["CarBrakeLightsOff"] = carBrakeLightsOff;
	root["HelicopterLight"] = helicopterLight;
	root["FrontEndLights"] = felist;
	root["SolidLights"] = list;

	std::ofstream fout(GetConfigFolder("SpotLights.yml"));
	fout << root;
	fout.close();
}
