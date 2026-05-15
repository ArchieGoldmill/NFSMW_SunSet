#pragma once

struct Particle : bTNode<Particle>
{
	int field_8;
	float Size;
	D3DXVECTOR3 Rotation;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	D3DXVECTOR3 Direction;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	D3DXVECTOR3 Position;
	int field_5C;
	int field_60;
	int field_64;
	int mAngle;
	int field_6C;
};

struct Emitter : bTNode<Emitter>
{
	int mControl[2];
	float mParticleAccumulation;
	int mRandomSeed;
	int mFlags;
	__int16 mNumParticles;
	__int16 mListIndex;
	D3DXMATRIX mLocalWorld;
	D3DXVECTOR4 mInheritVelocity;
	float mMinIntensity;
	float mMaxIntensity;
	void* mTexturePageRange;
	void* mDynamicData;
	bTNode<Particle> Particles;
	void* mTexPageTokenNode;
	struct EmitterGroup* mGroup;
};

struct EmitterGroup : bTNode<EmitterGroup>
{
	bTNode<Emitter> Emmiters;
	Hash mGroupKey;
	Hash Padding;
	Hash mFlags;
	short mNumEmitters;
	short mSectionNumber;
	D3DXMATRIX mLocalWorld;
};

struct CarEffect : bTNode<CarEffect>
{
	int field_8;
	int field_C;
	D3DXMATRIX Matrix;
	EmitterGroup* pEmitterGroup;
	int Hash;
};