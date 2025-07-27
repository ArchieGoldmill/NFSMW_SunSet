#pragma once
#include <unordered_map>
#include <unordered_set>
#include <yaml-cpp/yaml.h>
#include "Utilities.h"

struct PrelitTexture
{
	Hash Name;
	D3DXVECTOR4 Color = { 1, 1, 1, 1 };
};

inline std::unordered_map<unsigned int, PrelitTexture> PrelitTextures;

void InitTextureConfig()
{
	PrelitTextures.clear();

#ifdef _DEBUG 
	YAML::Node texturesRoot = YAML::LoadFile("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\scripts\\SunRiseData\\Textures.yml");
#else
	auto dir = GetExeDirectory();
	YAML::Node texturesRoot = YAML::LoadFile(dir + "\\scripts\\SunRiseData\\Textures.yml");
#endif

	auto prelit = texturesRoot["Prelit"];
	for (const auto& node : prelit)
	{
		PrelitTexture prelit;

		auto nameNode = node["Name"];
		if (nameNode.IsDefined())
		{
			auto str = nameNode.as<std::string>();
			prelit.Name = Game::bStringHash(str.c_str());
		}
		else
		{
			prelit.Name = node["NameHash"].as<Hash>();
		}


		auto colorNode = node["Color"];
		if (colorNode.IsDefined())
		{
			prelit.Color = ParseVec3To4(colorNode);
		}

		prelit.Color.w = YmlGet(node, "Brightness", 1.0f);

		PrelitTextures[prelit.Name] = prelit;
	}
}