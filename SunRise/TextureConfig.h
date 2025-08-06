#pragma once
#include <unordered_map>
#include <unordered_set>
#include <yaml-cpp/yaml.h>
#include "Utilities.h"

struct PrelitTexture
{
	std::string Name;
	Hash NameHash;
	D3DXVECTOR4 Color = { 1, 1, 1, 1 };
	bool NightOnly = false;
	bool AlphaMask = false;

	void ParseTextureName()
	{
		if (StartsWith0x(this->Name))
		{
			this->NameHash = std::stoul(this->Name, nullptr, 16);
		}
		else
		{
			this->NameHash = Game::bStringHash(this->Name.c_str());
		}
	}
};

inline std::unordered_map<unsigned int, PrelitTexture> PrelitTextures;

void PopulateTextureMap(std::unordered_map<unsigned int, PrelitTexture>& textures, const YAML::Node& texturesRoot, const char* section)
{
	auto prelit = texturesRoot[section];
	if (!prelit.IsDefined())
	{
		return;
	}

	for (const auto& node : prelit)
	{
		PrelitTexture prelit;

		prelit.Name = node["Name"].as<std::string>();
		prelit.ParseTextureName();

		auto colorNode = node["Color"];
		if (colorNode.IsDefined())
		{
			prelit.Color = ParseVec3To4(colorNode);
		}

		prelit.Color.w = YmlGet(node, "Brightness", 1.0f);
		prelit.NightOnly = YmlGet(node, "NightOnly", false);
		prelit.AlphaMask = YmlGet(node, "AlphaMask", false);

		textures[prelit.NameHash] = prelit;
	}
}

void InitTextureConfig()
{
	PrelitTextures.clear();

	YAML::Node texturesRoot = YAML::LoadFile(GetConfigFolder("Textures.yml"));

	PopulateTextureMap(PrelitTextures, texturesRoot, "Textures");
}

void SaveTextureConfig()
{
	YAML::Node list;
	for (auto& tex : PrelitTextures)
	{
		YAML::Node node;

		node["Name"] = tex.second.Name;
		node["Brightness"] = tex.second.Color.w;
		node["NightOnly"] = tex.second.NightOnly;
		node["AlphaMask"] = tex.second.AlphaMask;
		node["Color"] = SerializeVector3(tex.second.Color);

		list.push_back(node);
	}

	YAML::Node root;
	root["Textures"] = list;

	std::ofstream fout(GetConfigFolder("Textures.yml"));
	fout << root;
	fout.close();
}