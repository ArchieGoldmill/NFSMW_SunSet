#pragma once

namespace injector
{
	inline DWORD UnprotectMemory(LPVOID address, size_t size)
	{
		DWORD oldProtect = 0;

		if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			return 0;
		}

		return oldProtect;
	}

	inline bool RestoreMemoryProtection(void* address, size_t size, DWORD oldProtect)
	{
		DWORD temp;
		return VirtualProtect(address, size, oldProtect, &temp) != 0;
	}

	inline void MakeJMP(uint32_t target, void* detour)
	{
		auto oldProtect = UnprotectMemory((LPVOID)target, 5);

		uint8_t* p = (uint8_t*)target;
		intptr_t rel = (intptr_t)detour - (intptr_t)target - 5;

		p[0] = 0xE9;
		*(uint32_t*)(p + 1) = (uint32_t)rel;

		RestoreMemoryProtection((LPVOID)target, 5, oldProtect);
	}

	inline void MakeCALL(uint32_t target, void* detour)
	{
		auto oldProtect = UnprotectMemory((LPVOID)target, 5);

		uint8_t* p = (uint8_t*)target;
		intptr_t rel = (intptr_t)detour - (intptr_t)target - 5;

		p[0] = 0xE8;
		*(uint32_t*)(p + 1) = (uint32_t)rel;

		RestoreMemoryProtection((LPVOID)target, 5, oldProtect);
	}

	inline void MakeNOP(uint32_t target, size_t count)
	{
		auto oldProtect = UnprotectMemory((LPVOID)target, count);

		uint8_t* p = (uint8_t*)target;
		for (size_t i = 0; i < count; i++)
		{
			p[i] = 0x90;
		}

		RestoreMemoryProtection((LPVOID)target, count, oldProtect);
	}

	template<typename T>
	inline void WriteMemory(LPVOID target, const T& value)
	{
		auto oldProtect = UnprotectMemory(target, sizeof(T));

		*(T*)target = value;

		RestoreMemoryProtection(target, sizeof(T), oldProtect);
	}

	template<typename T>
	inline void WriteMemory(uint32_t target, const T& value)
	{
		WriteMemory((LPVOID)target, value);
	}

	inline void MakeRangedNOP(uint32_t start, uint32_t end)
	{
		MakeNOP(start, end - start);
	}

	inline void WriteMemoryRaw(uint32_t target, LPVOID mem, size_t count)
	{
		auto oldProtect = UnprotectMemory((LPVOID)target, count);

		memcpy((LPVOID)target, mem, count);

		RestoreMemoryProtection((LPVOID)target, count, oldProtect);
	}
}