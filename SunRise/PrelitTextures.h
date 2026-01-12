#pragma once
#include <vector>
#include <string>
#include "Hashes.h"
#include "Config.h"

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
		return this->table.find(targetHash);
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