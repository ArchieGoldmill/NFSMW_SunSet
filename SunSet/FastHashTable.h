#pragma once

template<typename Value, uint32_t TableSize, typename Key = uint32_t>
class FastHashTable
{
public:
	struct Entry
	{
		Key key;
		Value value;
	};

	FastHashTable() noexcept
	{
		this->entries = new Entry[TableSize];
		clear();
	}

	inline void clear() noexcept
	{
		memset(entries, 0, sizeof(Entry) * TableSize);

		this->lastKey = 0;
		this->lastValue = nullptr;
	}

	inline Value* insert(Key key, const Value& value) noexcept
	{
		key = mix(key);

		uint32_t idx = index(key);

		for (;;)
		{
			Key k = entries[idx].key;

			if (k == 0 || k == key)
			{
				entries[idx].key = key;
				entries[idx].value = value;
				return &entries[idx].value;
			}

			idx = nextIndex(idx);
		}
	}

	inline Value* find(Key key, bool create = false) noexcept
	{
		key = mix(key);

		if (key == lastKey)
		{
			return lastValue;
		}

		uint32_t idx = index(key);

		for (;;)
		{
			Key k = entries[idx].key;

			if (k == key)
			{
				lastKey = key;
				lastValue = &entries[idx].value;
				return lastValue;
			}

			if (k == 0)
			{
				if (create)
				{
					entries[idx].key = key;
					return &entries[idx].value;
				}
				else
				{
					return nullptr;
				}
			}

			idx = nextIndex(idx);
		}
	}

private:
	Key lastKey = 0;
	Value* lastValue = nullptr;
	Entry* entries = nullptr;

	static constexpr uint32_t mask = TableSize - 1;

	static inline Key mix(Key key) noexcept
	{
		key ^= key >> 16;
		return key;
	}

	static inline uint32_t index(Key key) noexcept
	{
		return static_cast<uint32_t>(key) & mask;
	}

	static constexpr uint32_t nextIndex(uint32_t idx) noexcept
	{
		return (idx + 1) & mask;
	}
};