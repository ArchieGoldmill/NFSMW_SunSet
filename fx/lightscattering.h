#define fog_g1 0
#define fog_g2 1
#define fog_g3 2
#define fog_multiplier 3

float3 Fog_Br_Plus_Bm : FOG_BR_PLUS_BM; // br+bm
float3 Fog_One_Over_BrBm : FOG_ONE_OVER_BRBM; // 1/(br+bm)
float3 Fog_Const_1 : FOG_CONST1; // 3/(16 * PI) * br, w=distance scale
float3 Fog_Const_2 : FOG_CONST2; // 1/(4 * PI) * bm

float4 Fog_Const_3 : FOG_CONST3; // (1-g)^2, 1+g*g, -2 * g, multiplier
//float4 SunColor : SUNCOLOR; // Sun color and intensity

const float kMaxDist = 7000;
// I tried to put this in Fog_Const_1.w but is wasn't being transfered through
// correctly to some of the shaders... probably a D3D bug.
float   Fog_DistanceScale : FOGVALUE;		


float3 miePhase(float dist, float3 sunL)
{
	return max(exp(-pow(dist, .3)) * sunL - .4, 0.);
}

#define SUN_INTENSITY 80.66
#define SUN_COLOR float3(2.6, 1.3, 1.1)
#define SKY_COLOR float3(.25, .5, 1.75)
#define SUN_SPEED .04

float3 atmosphericScatteringImpl(float3 uv, float3 sunPos, float dist, bool isSun)
{
	float sunDistance = distance(uv, sunPos);
	float scatterMult = saturate(sunDistance);

	dist = (.5 * lerp(scatterMult, 1., dist)) / dist;
	float3 mieScatter = miePhase(sunDistance, 1) * SUN_COLOR;
	float3 color = dist * SKY_COLOR;
	color = max(color, 0.);
	float3 sun = .0002 / pow(length(uv - sunPos), 1.7) * SUN_COLOR;

	color = max(lerp(pow(color, .8 - color),
		color / (2. * color + .5 - color * 1.3),
		saturate(sunPos.z * 2.5)), 0.)
		+ (isSun ? (sun + mieScatter) : 0);

	color *= (pow(1. - scatterMult, 5.) * 10. * saturate(.666 - sunPos.z)) + 1.5;

	float underscatter = distance(sunPos.z, 1.);
	color = lerp(color, 0, saturate(underscatter));

	float underscatter2 = distance(sunPos.z, 0.);
	color = lerp(color, 0, saturate(underscatter2) / 1.15f);

	return color;
}

float3 atmosphericScattering(float3 uv, float3 sunPos, bool isSun)
{
	return atmosphericScatteringImpl(uv, sunPos, max(uv.z, 0.01), isSun);
}

float b = 0.001, c = 1;

#define CALC_PASHKA_FOG_CUSTOM(result, localpos, LightDirVec) \
	float3 _FOG_LocalWorldPos = mul(LocalEyePos, LocalWorld); \
	float _FOG_dist = distance(LocalEyePos.xyz, localpos.xyz); \
	float _FOG_fogAmount = saturate(1 - exp(_FOG_dist * b)); \
	result = lerp(result, atmosphericScattering(-normalize(_FOG_LocalWorldPos.xyz - worldPosition.xyz), normalize(LightDirVec), false), saturate(_FOG_dist / 500))

#define CALC_PASHKA_FOG(result, localpos) \
	CALC_PASHKA_FOG_CUSTOM(result, localpos, LightDirVec)

void CalcFogNoDistScale(in float dist, in float cos_theta, out float3 fogAdd, out float3 fogMod)
{
	//float3 t = Fog_Br_Plus_Bm.xyz * -min(dist, kMaxDist);
	float3 t = Fog_Br_Plus_Bm.xyz * -dist;//-min(dist, 7000.0);

	float3 extinct = exp(t.xyz);

	//OUT.FogMod.xyz = saturate(extinct.xyz * SunColor.xyz * SunColor.w);
	fogMod.xyz = saturate(extinct.xyz);

	float cos_sq = cos_theta * cos_theta;

	float phase1 = 1.0 + cos_sq;
	float phase2 = Fog_Const_3[fog_g2] + Fog_Const_3[fog_g3] * cos_theta;
	phase2 = rsqrt(phase2);
	phase2 = phase2 * phase2 * phase2 * Fog_Const_3[fog_g1];

	float3 br_theta, bm_theta;

	br_theta = Fog_Const_1.xyz * phase1;
	bm_theta = Fog_Const_2 * phase2;
	float3 lin = (br_theta + bm_theta) * Fog_One_Over_BrBm * (1.0 - extinct);

	//OUT.FogAdd = saturate(SunColor.xyz * lin * SunColor.w * Fog_Const_3[fog_multiplier]);
	fogAdd = saturate(lin * Fog_Const_3[fog_multiplier]);
}

void CalcFog(in float dist, in float cos_theta, out float3 fogAdd, out float3 fogMod)
{
	CalcFogNoDistScale(min(dist, 1/Fog_DistanceScale * 2) * Fog_DistanceScale, cos_theta, fogAdd, fogMod);
}
