#pragma once
#include "EmitterGroup.h"

void __fastcall SetLocalWorld(EmitterGroup* emitterGroup, CarEffect* carEffect, D3DXMATRIX* matrix)
{
	bool applyDiff = emitterGroup->mLocalWorld != *Game::IdentityMatrix && g_Config.NosFix;

	D3DXVECTOR3 currentPos;
	currentPos.x = emitterGroup->mLocalWorld._41;
	currentPos.y = emitterGroup->mLocalWorld._42;
	currentPos.z = emitterGroup->mLocalWorld._43;

	FUNC(0x00503BC0, void, __thiscall, _SetLocalWorld, EmitterGroup*, D3DXMATRIX*);
	_SetLocalWorld(emitterGroup, matrix);

	if (applyDiff && (carEffect->Hash == 0x6B7916BD || carEffect->Hash == 0x138F6983))
	{
		D3DXVECTOR3 newPos;
		newPos.x = matrix->_41;
		newPos.y = matrix->_42;
		newPos.z = matrix->_43;

		D3DXVECTOR3 diff = newPos - currentPos;

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

void __declspec(naked) SetLocalWorldHook()
{
	static constexpr auto cExit = 0x00744B65;

	__asm
	{
		mov edx, esi;
		call SetLocalWorld;

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
		injector::MakeJMP(0x00744B60, SetLocalWorldHook);
	}

	if (g_Config.NosEmmiter)
	{
		injector::MakeJMP(0x00756346, NosEffectCave);
	}
}