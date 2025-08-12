#pragma once
#include <unordered_map>
#include <unordered_set>
#include <yaml-cpp/yaml.h>
#include "Utilities.h"

struct PrelitTexture
{
	std::string Name;
	Hash NameHash;

	HashField Mask;
	TextureInfo* MaskTexture = NULL;

	D3DXVECTOR4 Color = { 1, 1, 1, 1 };

	bool AlwaysOn;
	bool Prelit;

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

	TextureInfo* GetMaskTexture()
	{
		auto hash = this->Mask.GetHash();
		if (!hash || hash == -1)
		{
			hash = Hashes::WHITE32X32;
		}

		if (!this->MaskTexture)
		{
			this->MaskTexture = TextureInfo::Get(hash, false, false);
		}

		return this->MaskTexture;
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
		prelit.Color = ParseVec3To4(node["Color"]);
		prelit.Color.w = YmlGet(node, "Brightness", 1.0f);
		prelit.Mask.SetString(node["Mask"].as<std::string>());
		prelit.AlwaysOn = YmlGet(node, "AlwaysOn", true);
		prelit.Prelit = YmlGet(node, "Prelit", true);

		prelit.ParseTextureName();
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
		node["Color"] = SerializeVector3(tex.second.Color);
		node["Mask"] = tex.second.Mask.GetString();
		node["AlwaysOn"] = tex.second.AlwaysOn;
		node["Prelit"] = tex.second.Prelit;

		list.push_back(node);
	}

	YAML::Node root;
	root["Textures"] = list;

	std::ofstream fout(GetConfigFolder("Textures.yml"));
	fout << root;
	fout.close();
}