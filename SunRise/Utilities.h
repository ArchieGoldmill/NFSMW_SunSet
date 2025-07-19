#pragma once
#include <stdio.h>

#define ASSERT_SIZE(T, N) static_assert(sizeof(T) == N, "sizeof("#T") != "#N)
#define INLINE_FUNC(address, return_t, callconv, name, ...) inline return_t (callconv* name)(__VA_ARGS__) = reinterpret_cast<decltype(name)>(address)
#define FUNC(address, return_t, callconv, name, ...) return_t (callconv* name)(__VA_ARGS__) = reinterpret_cast<decltype(name)>(address)

typedef unsigned int Hash;

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

bool ConeIntersectsSphere(const D3DXVECTOR3& coneTip, const D3DXVECTOR3& coneDir, float coneAngleRad, float coneRange, const D3DXVECTOR3& sphereCenter, float sphereRadius)
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