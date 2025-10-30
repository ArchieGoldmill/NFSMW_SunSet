/////////////////////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 3206 4717 )

float4x4 WorldViewProj   : WORLDVIEWPROJECTION ;
shared float4 ScreenOffset		: SCREENOFFSET;

float4 world_position( float4 screen_pos )
{
 	float4 p = mul(screen_pos, WorldViewProj);
	p.xy += ScreenOffset.xy * p.w;
    return p;
}

float4 screen_position( float4 screen_pos )
{
	screen_pos.xy += ScreenOffset.xy;
    return screen_pos;
}

texture DiffuseMapTexture : DiffuseMap;
texture DiffuseMiscTexture : DIFFUSETEX;
texture NormalMapTexture : NormalMapTexture;
texture EnvironmentMapTexture : EnvMapTexture;
texture SpecularMapTexture : SPECULARMAPTEXTURE;

#define MWPCC_DIFFUSE texture = <DiffuseMapTexture>
#define MWPCC_DIFFUSE_MISC texture = <DiffuseMiscTexture>
#define MWPCC_NORMAL texture = <NormalMapTexture>
#define MWPCC_ENVMAP texture = <EnvironmentMapTexture>
#define MWPCC_SPECULAR texture = <SpecularMapTexture>

int BlendStateFromGame[5] : BLENDSTATE;
int CullModeFromGame : CULL_MODE;
int CWModeFromGame : COLORWRITEMODE;

#define MWPCC_PARAMS \
		AlphaTestEnable = (BlendStateFromGame[0]); \
		AlphaRef = (BlendStateFromGame[1]); \
		AlphaBlendEnable = (BlendStateFromGame[2]); \
		SrcBlend = (BlendStateFromGame[3]); \
		DestBlend = (BlendStateFromGame[4]); \
		CullMode = (CullModeFromGame)

float4x4 LocalWorld : LOCALWORLD;

#define GAMMA_CORRECT(result) result = pow(result, 1/2.2)

// #define MIRRORED_WORLD
