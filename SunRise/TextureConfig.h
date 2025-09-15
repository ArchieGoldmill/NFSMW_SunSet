#pragma once
#include <unordered_map>
#include <unordered_set>
#include <yaml-cpp/yaml.h>
#include "Utilities.h"
#include "PrelitTextures.h"

void LoadTextureConfig()
{
	PrelitTextures.Clear();

	YAML::Node texturesRoot = YAML::LoadFile(GetConfigFolder("Textures.yml"));

	auto prelit = texturesRoot["Textures"];
	for (const auto& node : prelit)
	{
		PrelitTexture prelit;

		prelit.Name = node["Name"].as<std::string>();
		prelit.Color = ParseVec3To4(node["Color"]);
		prelit.Color.w = YmlGet(node, "Brightness", 1.0f);
		prelit.Mask.SetString(node["Mask"].as<std::string>());
		prelit.AlwaysOn = YmlGet(node, "AlwaysOn", true);
		prelit.Prelit = YmlGet(node, "Prelit", true);
		prelit.IgnoreWeather = YmlGet(node, "IgnoreWeather", false);
		prelit.ParseTextureName();

		PrelitTextures.Add(prelit);
	}

	PrelitTextures.Sort();
}

void SaveTextureConfig()
{
	YAML::Node list;
	for (auto& tex : PrelitTextures.GetList())
	{
		YAML::Node node;

		node["Name"] = tex.Name;
		node["Brightness"] = tex.Color.w;
		node["Color"] = SerializeVector3(tex.Color);
		node["Mask"] = tex.Mask.GetString();
		node["AlwaysOn"] = tex.AlwaysOn;
		node["Prelit"] = tex.Prelit;
		node["IgnoreWeather"] = tex.IgnoreWeather;

		list.push_back(node);
	}

	YAML::Node root;
	root["Textures"] = list;

	std::ofstream fout(GetConfigFolder("Textures.yml"));
	fout << root;
	fout.close();
}