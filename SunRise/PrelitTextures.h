#pragma once
#include <vector>
#include <string>
#include "Hashes.h"
#include "Weather.h"

struct PrelitTexture
{
	std::string Name;
	Hash NameHash;

	HashField Mask;
	TextureInfo* MaskTexture = NULL;

	D3DXVECTOR3 Color = { 1, 1, 1 };
	float Brightness = 1.0f;

	bool AlwaysOn;
	bool Prelit;
	bool IgnoreWeather;
	bool UseVertexColor;

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
		auto hash = this->Mask.hash;
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

	D3DXVECTOR4 GetColor()
	{
		D3DXVECTOR4 color = D3DXVECTOR4(this->Color, 1);

		if (!this->IgnoreWeather)
		{
			color *= g_Weather.GetTextureLightPower();
		}

		if (this->Prelit)
		{
			// For prelit textures embed brightness into color
			color *= this->Brightness;
			color.w = this->UseVertexColor ? 1.0 : 0.0;
		}
		else
		{
			// For emissive textures pass brightness to shader since base color might be used
			color.w = this->Brightness;
		}

		return color;
	}
};

struct PrelitTextureContainer
{
private:
	std::vector<PrelitTexture> list;
	std::vector<PrelitTexture*> uilist;
	FastHashTable<PrelitTexture*, 1024, Hash> table;

public:

	void Add(PrelitTexture& t)
	{
		this->list.push_back(t);
	}

	void Clear()
	{
		this->list.clear();
		this->table.clear();
	}

	void Sort()
	{
		this->table.clear();

		for (auto& tex : this->list)
		{
			this->table.insert(tex.NameHash, &tex);
		}
	}

	void PopulateUiList()
	{
		uilist.clear();

		for (int i = 0; i < this->list.size(); i++)
		{
			this->uilist.push_back(&this->list[i]);
		}

		std::sort(uilist.begin(), uilist.end(), [](const PrelitTexture* a, const PrelitTexture* b) { return a->Name < b->Name; });
	}

	std::vector<PrelitTexture*>& GetUiList()
	{
		return this->uilist;
	}

	std::vector<PrelitTexture>& GetList()
	{
		return this->list;
	}

	PrelitTexture* Get(Hash targetHash)
	{
		auto tex = this->table.find(targetHash);
		return tex ? *tex : nullptr;
	}

	void Remove(Hash targetHash)
	{
		int idx = -1;
		for (int i = 0; i < list.size(); i++)
		{
			if (list[i].NameHash == targetHash)
			{
				idx = i;
				break;
			}
		}

		if (idx != -1)
		{
			list.erase(list.begin() + idx);
		}

		this->Sort();
	}
};

inline PrelitTextureContainer PrelitTextures;