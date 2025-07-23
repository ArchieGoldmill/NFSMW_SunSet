#pragma once
#include <cmath>
#include <stdio.h>
#include <yaml-cpp/yaml.h>
#include "Hashes.h"

#define ASSERT_SIZE(T, N) static_assert(sizeof(T) == N, "sizeof("#T") != "#N)
#define INLINE_FUNC(address, return_t, callconv, name, ...) inline return_t (callconv* name)(__VA_ARGS__) = reinterpret_cast<decltype(name)>(address)
#define FUNC(address, return_t, callconv, name, ...) return_t (callconv* name)(__VA_ARGS__) = reinterpret_cast<decltype(name)>(address)

#define SAVE_REGS_EAX __asm\
{\
	__asm push ebx\
	__asm push ecx\
	__asm push edx\
	__asm push edi\
	__asm push esi\
}\

#define RESTORE_REGS_EAX __asm\
{\
	__asm pop esi\
	__asm pop edi\
	__asm pop edx\
	__asm pop ecx\
	__asm pop ebx\
}\

bool WriteFileFromMemory(const char* FileName, const void* buffer, long size)
{
	FILE* fout = fopen(FileName, "wb");
	if (fout == NULL)
		return 0;

	fwrite(buffer, 1, size, fout);

	fclose(fout);
	return 1;
}

inline bool ConeIntersectsSphere(const D3DXVECTOR3& coneTip, const D3DXVECTOR3& coneDir, float coneAngleRad, float coneRange, const D3DXVECTOR3& sphereCenter, float sphereRadius)
{
	D3DXVECTOR3 V = sphereCenter - coneTip;

	float Vlen = D3DXVec3Length(&V);

	if (Vlen > (coneRange + sphereRadius))
	{
		return false;
	}

	D3DXVECTOR3 Vn = V / Vlen;
	float cosTheta = cosf(coneAngleRad);
	float cosAlpha = D3DXVec3Dot(&Vn, &coneDir);

	if (cosAlpha < cosTheta && Vlen > sphereRadius)
	{
		return false;
	}

	return true;
}

bool StringEqual(const char* s1, const char* s2)
{
	if (s1 == NULL && s2 == NULL)
	{
		return true;
	}

	if (s1 == NULL || s2 == NULL)
	{
		return false;
	}

	return strcmp(s1, s2) != 0;
}

inline float ConvertRange(float value, float srcMin, float srcMax, float destMin, float destMax)
{
	return destMin + (value - srcMin) * (destMax - destMin) / (srcMax - srcMin);
}

inline D3DXVECTOR4 LerpVector(D3DXVECTOR4 a, D3DXVECTOR4 b, float t)
{
	return D3DXVECTOR4(std::lerp(a.x, b.x, t), std::lerp(a.y, b.y, t), std::lerp(a.z, b.z, t), std::lerp(a.w, b.w, t));
}

inline D3DXVECTOR3 LerpVector(D3DXVECTOR3 a, D3DXVECTOR3 b, float t)
{
	return D3DXVECTOR3(std::lerp(a.x, b.x, t), std::lerp(a.y, b.y, t), std::lerp(a.z, b.z, t));
}

D3DXVECTOR3 ParseVec3(const YAML::Node& node)
{
	return D3DXVECTOR3(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
}

D3DXVECTOR4 ParseVec3To4(const YAML::Node& node)
{
	return D3DXVECTOR4(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), 0);
}

inline std::string GetExeDirectory() {
	char path[MAX_PATH] = { 0 };
	// Get the full path of the executable
	DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
	if (length == 0 || length == MAX_PATH) {
		// handle error
		return "";
	}

	std::string fullPath(path, length);
	// Remove the executable name to get the directory
	size_t pos = fullPath.find_last_of("\\/");
	if (pos == std::string::npos) {
		return "";  // unexpected, no directory separator
	}
	return fullPath.substr(0, pos);
}

inline void MoveTowards(float& a, float b, float step)
{
	if (a < b)
	{
		a += step;
		if (a > b)
		{
			a = b;
		}
	}
	else if (a > b)
	{
		a -= step;
		if (a < b)
		{
			a = b;
		}
	}
}