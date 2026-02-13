#pragma once
#include <vector>
#include "Spotlight.h"

class SolidLightsContainer
{
private:
	std::vector<SolidLights> list;
	std::vector<SolidLights*> uilist;
	FastHashTable<SolidLights*, 1024, Hash> table;

public:

	void Add(SolidLights& s)
	{
		this->list.push_back(s);
	}

	void Sort()
	{
		this->table.clear();

		for (auto& sl : this->list)
		{
			this->table.insert(sl.LodA.hash, &sl);
		}
	}

	void Clear()
	{
		this->list.clear();
		this->table.clear();
	}

	SolidLights* Get(Hash targetHash)
	{
		auto sl = this->table.find(targetHash);
		return sl ? *sl : nullptr;
	}

	std::vector<SolidLights>& GetList()
	{
		return this->list;
	}

	std::vector<SolidLights*>& GetUiList()
	{
		return this->uilist;
	}

	void PopulateUiList()
	{
		uilist.clear();

		for (int i = 0; i < this->list.size(); i++)
		{
			this->uilist.push_back(&this->list[i]);
		}

		std::sort(uilist.begin(), uilist.end(), [](const SolidLights* a, const SolidLights* b) { return a->LodA.str < b->LodA.str; });
	}

	void Remove(Hash targetHash)
	{
		int idx = -1;
		for (int i = 0; i < list.size(); i++)
		{
			if (list[i].LodA.hash == targetHash)
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

inline SolidLightsContainer SolidLightsList;