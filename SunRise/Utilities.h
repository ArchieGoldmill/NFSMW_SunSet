#pragma once
#include <cmath>
#include <stdio.h>
#include <yaml-cpp/yaml.h>
#include "Hashes.h"

inline const char* ModName = "NFSMW - Sun Set 1.9";

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

#define SAVE_REGS_EDI __asm\
{\
	__asm push ebx\
	__asm push ecx\
	__asm push edx\
	__asm push eax\
	__asm push esi\
}\

#define RESTORE_REGS_EDI __asm\
{\
	__asm pop esi\
	__asm pop eax\
	__asm pop edx\
	__asm pop ecx\
	__asm pop ebx\
}\

#define SAVE_REGS_EDX __asm\
{\
	__asm push ebx\
	__asm push ecx\
	__asm push edi\
	__asm push eax\
	__asm push esi\
}\

#define RESTORE_REGS_EDX __asm\
{\
	__asm pop esi\
	__asm pop eax\
	__asm pop edi\
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

inline float ConvertRange(float value, float inMin, float inMax, float outMin, float outMax)
{
	float scaled = (value - inMin) / (inMax - inMin);
	return outMin + (scaled * (outMax - outMin));
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

D3DXVECTOR4 ParseVec3To4(const YAML::Node& node, float a = 0.0f)
{
	return D3DXVECTOR4(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), a);
}

D3DXVECTOR4 ParseVec4(const YAML::Node& node)
{
	return D3DXVECTOR4(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>());
}

inline std::string GetExeDirectory()
{
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

template <typename T>
inline T YmlGet(const YAML::Node& spot, const char* section, T def)
{
	auto node = spot[section];
	if (node.IsDefined())
	{
		return node.as<T>();
	}

	return def;
}

D3DXVECTOR4 ParseVec3To4(const YAML::Node& node, D3DXVECTOR4& def)
{
	if (node.IsDefined())
	{
		return D3DXVECTOR4(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), 0.0f);
	}

	return def;
}

D3DXVECTOR4 ParseVec4(const YAML::Node& node, D3DXVECTOR4& def)
{
	if (node.IsDefined())
	{
		return D3DXVECTOR4(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>());
	}

	return def;
}

inline float Smoothstep(float edge0, float edge1, float x) {
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}

inline std::string GetConfigFolder(const char* fileName)
{
#ifdef _DEBUG
	return std::string("D:\\Programming\\NFSMW\\NFSMW_SunRise\\mod\\SunSet\\scripts\\SunSetData\\") + fileName;
#endif

	auto dir = GetExeDirectory();
	return std::string(dir + "\\scripts\\SunSetData\\") + fileName;
}

inline bool FileExists(const char* fileName)
{
	FILE* fin = fopen(fileName, "rb");
	if (fin == NULL)
	{
		return false;
	}

	fclose(fin);
	return true;
}

inline YAML::Node SerializeVector3(const D3DXVECTOR4& vec) {
	YAML::Node node(YAML::NodeType::Sequence);
	node.push_back(vec.x);
	node.push_back(vec.y);
	node.push_back(vec.z);
	node.SetStyle(YAML::EmitterStyle::Flow);
	return node;
}

inline YAML::Node SerializeVector3(const D3DXVECTOR3& vec) {
	YAML::Node node(YAML::NodeType::Sequence);
	node.push_back(vec.x);
	node.push_back(vec.y);
	node.push_back(vec.z);
	node.SetStyle(YAML::EmitterStyle::Flow);
	return node;
}

inline YAML::Node SerializeVector4(const D3DXVECTOR4& vec) {
	YAML::Node node(YAML::NodeType::Sequence);
	node.push_back(vec.x);
	node.push_back(vec.y);
	node.push_back(vec.z);
	node.push_back(vec.w);
	node.SetStyle(YAML::EmitterStyle::Flow);
	return node;
}

bool StartsWith0x(const std::string& str)
{
	return str.size() > 2 && str.size() <= 10 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X');
}