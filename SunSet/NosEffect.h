#pragma once
#include "EmitterGroup.h"

D3DXVECTOR3 GetPosition(D3DXMATRIX* matrix)
{
	D3DXVECTOR3 pos;
	pos.x = matrix->_41;
	pos.y = matrix->_42;
	pos.z = matrix->_43;
	return pos;
}

inline bool IsVec3NotZero(D3DXVECTOR3& vec)
{
	return vec.x != 0 || vec.y != 0 || vec.z != 0;
}

void __stdcall UpdateEffects(size_t conn)
{
	auto exhaustEffectsList = (bNode<CarEffect>*)(conn + 0x3E4);
	auto carMatrix = (D3DXMATRIX*)(conn + 0x330);

	auto p = exhaustEffectsList->Next;
	while (p != exhaustEffectsList)
	{
		auto carEffect = (CarEffect*)p;
		auto emitterGroup = carEffect->pEmitterGroup;

		if (emitterGroup)
		{
			D3DXMATRIX newMatrix;
			D3DXMatrixMultiply(&newMatrix, &carEffect->Matrix, carMatrix);

			D3DXVECTOR3 newPos = GetPosition(&newMatrix);
			D3DXVECTOR3 currentPos = GetPosition(&emitterGroup->mLocalWorld);

			D3DXVECTOR3 diff = newPos - currentPos;

			if (IsVec3NotZero(diff))
			{
				emitterGroup->SetLocalWorld(&newMatrix);

				auto pemitter = emitterGroup->Emmiters.Next;
				while (pemitter != &emitterGroup->Emmiters)
				{
					auto emitter = (Emitter*)pemitter;

					auto pparticles = emitter->Particles.Next;
					while (pparticles != &emitter->Particles)
					{
						auto particle = (Particle*)pparticles;
						particle->Position += diff;

						pparticles = pparticles->Next;
					}

					pemitter = pemitter->Next;
				}
			}
		}

		p = p->Next;
	}
}

void __declspec(naked) UpdateEffectsHook()
{
	static constexpr auto cExit = 0x0075632C;

	__asm
	{
		pushad;
		push esi;
		call UpdateEffects;
		popad;

		mov edi, [esi + 0x3E4];

		jmp cExit;
	}
}

void __declspec(naked) NosEffectCave()
{
	static constexpr auto cExit = 0x0075634B;

	__asm
	{
		mov al, [ecx + 0x71];
		mov ebp, 0x6B7916BD; // fxcar_nos
		test al, al;
		je NosEffectCave_Exit;
		mov ebp, 0x138f6983; // fxcar_nos_2

	NosEffectCave_Exit:
		jmp cExit;
	}
}

void InitNosEffect()
{
	if (g_Config.NosFix)
	{
		injector::MakeJMP(0x00756326, UpdateEffectsHook);
	}

	if (g_Config.NosEmmiter)
	{
		injector::MakeJMP(0x00756346, NosEffectCave);
	}
}