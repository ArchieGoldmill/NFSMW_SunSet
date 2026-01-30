#pragma once
#include <d3dx9.h>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "Spotlight.h"
#include "SolidLights.h"

void ParseSpotLight(SpotLight& spotLight, const YAML::Node& spot)
{
	spotLight.Position = ParseVec3(spot["Position"]);
	spotLight.Direction = ParseVec3(spot["Direction"]);
	spotLight.Color = ParseVec3(spot["Color"]);
	spotLight.InnerAngle = spot["InnerAngle"].as<float>();
	spotLight.OuterAngle = spot["OuterAngle"].as<float>();
	spotLight.Intensity = spot["Intensity"].as<float>();
	spotLight.Range = spot["Range"].as<float>();
	spotLight.Specular = YmlGet(spot, "Specular", 1.0f);
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
	SolidLightsList.Clear();
	FrontEndLights.clear();

	YAML::Node spotlightsRoot = YAML::LoadFile(GetConfigFolder("SpotLights.yml"));

	ParseSpotLight(CarHeadlighsConfig, spotlightsRoot["CarHeadLights"]);
	ParseSpotLight(CarAiHeadlighsConfig, spotlightsRoot["CarAiHeadLights"]);
	ParseSpotLight(CarBrakeLightsOnConfig, spotlightsRoot["CarBrakeLightsOn"]);
	ParseSpotLight(CarBrakeLightsOffConfig, spotlightsRoot["CarBrakeLightsOff"]);
	ParseSpotLight(CarReverseConfig, spotlightsRoot["CarReverse"]);
	ParseSpotLight(CopLightBlueConfig, spotlightsRoot["CopLightBlue"]);
	ParseSpotLight(CopLightRedConfig, spotlightsRoot["CopLightRed"]);
	ParseSpotLight(ExhaustLightConfig, spotlightsRoot["ExhaustLight"]);

	auto heliNode = spotlightsRoot["HelicopterLight"];
	ParseSpotLight(HelicopterLightConfig.Light, heliNode);
	HelicopterLightConfig.Flare = GetFlareModel(heliNode);

	const auto& spotLights = spotlightsRoot["SolidLights"];
	for (const auto& lightNode : spotLights)
	{
		SolidLights solid;

		solid.LodA.SetString(lightNode["SolidLodA"].as<std::string>());
		solid.Blink = YmlGet(lightNode, "Blink", 0);
		solid.AlwaysOn = YmlGet(lightNode, "AlwaysOn", false);
		solid.UseFirstLight = YmlGet(lightNode, "UseFirstLight", false);
		solid.Flare = GetFlareModel(lightNode);

		const auto& spotLights = lightNode["SpotLights"];
		for (const auto& spot : spotLights)
		{
			SpotLight spotLight;
			ParseSpotLight(spotLight, spot);

			solid.Lights.push_back(spotLight);
		}

		SolidLightsList.Add(solid);
	}

	SolidLightsList.Sort();

	const auto& frontEndLights = spotlightsRoot["FrontEndLights"];
	for (const auto& felightNode : frontEndLights)
	{
		int type = felightNode["Type"].as<int>();

		FrontEndLights[type].Flare = GetFlareModel(felightNode);
		FrontEndLights[type].Time = YmlGet(felightNode, "Time", -1.0f);

		const auto& spotLights = felightNode["SpotLights"];
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
	lightNode["Specular"] = light.Specular;
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
	for (auto& solidLight : SolidLightsList.GetList())
	{
		YAML::Node spotlights;
		SaveSpotlights(spotlights, solidLight.Lights);

		YAML::Node node;
		node["SolidLodA"] = solidLight.LodA.GetString();
		node["Blink"] = solidLight.Blink;
		node["AlwaysOn"] = solidLight.AlwaysOn;
		node["UseFirstLight"] = solidLight.UseFirstLight;
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
		node["Time"] = felight.second.Time;
		node["SpotLights"] = spotlights;

		felist.push_back(node);
	}

	YAML::Node carHeadlights;
	SaveSpotLight(carHeadlights, CarHeadlighsConfig);

	YAML::Node carAiHeadlights;
	SaveSpotLight(carAiHeadlights, CarAiHeadlighsConfig);

	YAML::Node carBrakeLightsOn;
	SaveSpotLight(carBrakeLightsOn, CarBrakeLightsOnConfig);

	YAML::Node carBrakeLightsOff;
	SaveSpotLight(carBrakeLightsOff, CarBrakeLightsOffConfig);

	YAML::Node carReverse;
	SaveSpotLight(carReverse, CarReverseConfig);

	YAML::Node copLightBlue;
	SaveSpotLight(copLightBlue, CopLightBlueConfig);

	YAML::Node copLightRed;
	SaveSpotLight(copLightRed, CopLightRedConfig);

	YAML::Node exhaustLight;
	SaveSpotLight(exhaustLight, ExhaustLightConfig);

	YAML::Node helicopterLight;
	helicopterLight["Flare"] = HelicopterLightConfig.Flare ? HelicopterLightConfig.Flare->Name : "";
	SaveSpotLight(helicopterLight, HelicopterLightConfig.Light);

	YAML::Node root;
	root["CarHeadLights"] = carHeadlights;
	root["CarAiHeadLights"] = carAiHeadlights;
	root["CarBrakeLightsOn"] = carBrakeLightsOn;
	root["CarBrakeLightsOff"] = carBrakeLightsOff;
	root["CarReverse"] = carReverse;
	root["CopLightBlue"] = copLightBlue;
	root["CopLightRed"] = copLightRed;
	root["ExhaustLight"] = exhaustLight;
	root["HelicopterLight"] = helicopterLight;
	root["FrontEndLights"] = felist;
	root["SolidLights"] = list;

	std::ofstream fout(GetConfigFolder("SpotLights.yml"));
	fout << root;
	fout.close();
}
