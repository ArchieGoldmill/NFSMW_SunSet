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
	bool sorted = true;
	std::vector<PrelitTexture> list;
	std::vector<PrelitTexture*> uilist;

public:

	void Add(PrelitTexture& t)
	{
		this->list.push_back(t);
		sorted = false;
	}

	void Clear()
	{
		this->list.clear();
		sorted = true;
	}

	void Sort()
	{
		std::sort(this->list.begin(), this->list.end(), [](const PrelitTexture& a, const PrelitTexture& b) { return a.NameHash < b.NameHash; });
		sorted = true;
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

	std::vector<PrelitTexture>& GetList(bool ignoreSort = false)
	{
#ifdef _DEBUG
		if (!sorted && !ignoreSort)
		{
			throw "list is not sorted";
		}
#endif 

		return this->list;
	}

	PrelitTexture* Get(Hash targetHash)
	{
#ifdef _DEBUG
		if (!sorted)
		{
			throw "list is not sorted";
		}
#endif

		int left = 0;
		int right = static_cast<int>(list.size()) - 1;

		while (left <= right)
		{
			int mid = left + (right - left) / 2;
			PrelitTexture& midItem = list[mid];

			if (midItem.NameHash == targetHash)
			{
				return &midItem;
			}
			else if (midItem.NameHash < targetHash)
			{
				left = mid + 1;
			}
			else
			{
				right = mid - 1;
			}
		}

		return nullptr;
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
	}
};

inline PrelitTextureContainer PrelitTextures;