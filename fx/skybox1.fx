//
// World Effects
//

#include "global.h"
#include "lightscattering.h"

float4x4 WorldView : WORLDVIEW;
float4x4 World : WORLDMAT;

float		Brightness : STANDARD_BRIGHTNESS;
float3		LightDirVec			: LOCALLIGHTDIRVEC;
float4		DiffuseColour		: DIFFUSECOLOUR;
float4		CloudIntensity		: SKY_DIFFUSESCALE;
float		SkyFogScale : SKY_FOGSCALE;
float3		LocalEyePos			: LOCALEYEPOS;
float		TimeTicker : CLOUDSCROLL;
float		MipMapBias : MIPMAPBIAS;
float		SkyAlphaTag : BASEALPHAREF;

texture skymisc1 : SKY_TEXTURE_MISC_1;
texture skymisc2 : SKY_TEXTURE_MISC_2;
texture skymisc3 : SKY_TEXTURE_MISC_3;

sampler DIFFUSEMAP_SAMPLER = sampler_state
{
	MWPCC_DIFFUSE;

	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

sampler MISCMAP1_SAMPLER = sampler_state
{
	texture = <skymisc1>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

sampler MISCMAP2_SAMPLER = sampler_state
{
	texture = <skymisc2>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

sampler MISCMAP3_SAMPLER = sampler_state
{
	texture = <skymisc3>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture3D VolumeMapTexture : VOLUMEMAPTEX3D;

sampler3D VOLUMEMAP_SAMPLER = sampler_state
{
	texture = <VolumeMapTexture>;

	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	MIPFILTER = NONE;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture SkyNoiseTexture : SKYNOISETEX;

sampler SKY_NOISE_SAMPLER = sampler_state
{
	texture = <SkyNoiseTexture>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color    : COLOR;
	float4 tex0		: TEXCOORD0;
	float4 tex1		: TEXCOORD1;
	float3 normal	: NORMAL;
};

//-----------------------------------------------------------------------------
// SKY RENDERING
//
struct VtoP
{
	float4 position		: POSITION;
	float4 vertexColour	: COLOR0;
	float4 diffuse		: COLOR1;
	float4 tex0			: TEXCOORD0;
	float4 tex1			: TEXCOORD1;
	float4 tex2			: TEXCOORD2;
	float4 tex3			: TEXCOORD3;
	float3 FogAdd		: TEXCOORD4;
	float3 FogMod		: TEXCOORD5;
	float3 Viewer		: TEXCOORD6;
};

float SkyFogPos : SKYFOGPOS;

VtoP vertex_shader(VS_INPUT IN)
{
	VtoP OUT;

	IN.position = normalize(IN.position);
	IN.position *= float4(1.4, 1.4, 0.10, 1);

	float4 p = world_position(IN.position);
	OUT.position = p / 2;
	OUT.tex0 = IN.tex0;
	OUT.tex0.w = MipMapBias;
	OUT.tex1 = IN.tex1;
	OUT.tex1.w = MipMapBias;
	OUT.tex2 = IN.tex0;
	OUT.tex2.w = MipMapBias;
	OUT.tex3 = IN.tex0;
	OUT.tex3.w = MipMapBias;

	OUT.tex2.x -= TimeTicker;
	OUT.tex0.x -= TimeTicker * 0.4;
	OUT.tex3.x -= TimeTicker * 0.2 + 0.3;
	OUT.tex3.y -= 0.05;

	OUT.vertexColour = IN.color;// * float4(AmbientColour.xyz * AmbientColour.w, 1);

	float3 lightDir = normalize(LightDirVec);

	float3 Viewer = LocalEyePos.xyz - IN.position.xyz;
	Viewer = normalize(Viewer);

	OUT.Viewer = IN.position.xyz;

	OUT.diffuse = DiffuseColour;

	// Light Scattering	
	//

	// Fake the distance extrapolating further to the horizon.  This results in the
	// light scatters colour becoming lighter closer to the horizon
	const float kSkyDistance = 10000.f;
	float dist = kSkyDistance + kSkyDistance * (1 - IN.position.z / 3300) * SkyFogPos;

	float cos_theta = dot(lightDir, Viewer);
	CalcFogNoDistScale((dist) / 5000.f, cos_theta, OUT.FogAdd.xyz, OUT.FogMod.xyz);

	return OUT;
}

// sun disk rendering:
// no sun disk - the fastest option
#define SKYBOX_SUNDISK_NONE 0
// simplistic sun disk - without mie phase function
#define SKYBOX_SUNDISK_SIMPLE 1
// full calculation - uses mie phase function
#define SKYBOX_SUNDISK_HQ 2

#define SKYBOX_SUNDISK SKYBOX_SUNDISK_HQ

#define SKYBOX_COLOR_IN_TARGET_COLOR_SPACE

#define MIE_G (-0.990)
#define MIE_G2 0.9801

#define _SunSize 0.24f
#define _SunSizeConvergence 400.f

// Calculates the Mie phase function
half getMiePhase(half eyeCos, half eyeCos2)
{
	half temp = 1.0 + MIE_G2 - 2.0 * MIE_G * eyeCos;
	temp = pow(temp, pow(_SunSize, 0.65) * 10);
	temp = max(temp, 1.0e-4); // prevent division by zero, esp. in half precision
	temp = 1.5 * ((1.0 - MIE_G2) / (2.0 + MIE_G2)) * (1.0 + eyeCos2) / temp;
	temp = pow(temp, .454545);
	return temp;
}

// Calculates the sun shape
half calcSunAttenuation(half3 lightPos, half3 ray)
{
#if SKYBOX_SUNDISK == SKYBOX_SUNDISK_SIMPLE
	half3 delta = lightPos - ray;
	half dist = distance(lightPos, ray) / _SunSize * 2;// length(delta) / _SunSize * 2;
	half spot = 1.0 - lerp(0.0, _SunSize, dist);
	return saturate(saturate(smoothstep(0.0, _SunSize, 1 - saturate(dist))) + lerp(0.0, _SunSize * 3, 1 - saturate(dist)) * 0.2f);
#else // SKYBOX_SUNDISK_HQ
	half focusedEyeCos = pow(saturate(dot(lightPos, ray)), _SunSizeConvergence);
	return getMiePhase(-focusedEyeCos, focusedEyeCos * focusedEyeCos);
#endif
}

//==========================================================================================
// hashes (low quality, do NOT use in production)
//==========================================================================================

float hash1(float2 p)
{
	p = 50.0 * frac(p * 0.3183099);
	return frac(p.x * p.y * (p.x + p.y));
}

float hash1(float n)
{
	return frac(n * 17.0 * frac(n * 0.3183099));
}

float2 hash2(float2 p)
{
	const float2 k = float2(0.3183099, 0.3678794);
	float n = 111.0 * p.x + 113.0 * p.y;
	return frac(n * frac(k * n));
}

float3 noised(float2 x)
{
	float2 p = floor(x);
	float2 w = frac(x);
#if 1
	float2 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
	float2 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);
#else
	vec2 u = w * w * (3.0 - 2.0 * w);
	vec2 du = 6.0 * w * (1.0 - w);
#endif

	float a = hash1(p + float2(0, 0));
	float b = hash1(p + float2(1, 0));
	float c = hash1(p + float2(0, 1));
	float d = hash1(p + float2(1, 1));

	float k0 = a;
	float k1 = b - a;
	float k2 = c - a;
	float k4 = a - b - c + d;

	return float3(-1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k4 * u.x * u.y),
		2.0 * du * float2(k1 + k4 * u.y,
			k2 + k4 * u.x));
}

//==========================================================================================
// fbm constructions
//==========================================================================================

const float3x3 m3 = float3x3(0.00, 0.80, 0.60,
	-0.80, 0.36, -0.48,
	-0.60, -0.48, 0.64);
const float3x3 m3i = float3x3(0.00, -0.80, -0.60,
	0.80, 0.36, -0.48,
	0.60, -0.48, 0.64);
const float2x2 m2 = float2x2(0.80, 0.60,
	-0.60, 0.80);
const float2x2 m2i = float2x2(0.80, -0.60,
	0.60, 0.80);

float4 fbmd_8(float3 x)
{
	float f = 2.0;
	float s = 0.65;
	float a = 0.0;
	float b = 0.5;
	float3  d = 0;
	float3x3  m = float3x3(1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0);
	for (int i = 0; i < 8; i++)
	{
		float4 n = float4(noised(x), 1);
		a += b * n.x;          // accumulate values		
		if (i < 4)
			d += mul((b * m), n.yzw);      // accumulate derivatives
		b *= s;
		x = mul(f * m3, x);
		m = f * m3i * m;
	}
	return float4(a, d);
}

//------------------------------------------------------------------------------------------
// clouds
//------------------------------------------------------------------------------------------

float4 cloudsFbm(float3 pos)
{
	return fbmd_8(pos + float3(20.0, 10.1, 10.0) + 5 * float3(TimeTicker, 0.5 * TimeTicker, -0.15 * TimeTicker));
}

float4 cloudsMap(in float3 pos, in float fbmCoeff, out float nnd)
{
	float d = abs(pos.z - 900.0) - 40.0;
	float3 gra = float3(0.0, sign(pos.z - 900.0), 0.0);

	float4 n = cloudsFbm(pos * fbmCoeff);
	d += 400.0 * n.x * (0.7 + 0.3 * gra.y);

	if (d > 0.0) return float4(-d, 0.0, 0.0, 0.0);

	nnd = -d;
	d = min(-d / 100.0, 0.25);

	//gra += 0.1*n.yzw *  (0.7+0.3*gra.y);

	return float4(d, gra);
}

#define WIND_DIR float3( .3, .3, .3)
#define WIND_SPEED 1.1

#define CLOUD_BASE_FREQ 0.6

float remap01(float x, float a, float b)
{
	return ((x - a) / (b - a));
}


/*
// Hash functions by Dave_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 uvec3(UI0, UI1, 2798796415U)
#define UIF (1. / float(0xffffffffU))

vec3 hash33(vec3 p)
{
	uvec3 q = uvec3(ivec3(p)) * UI3;
	q = (q.x ^ q.y ^ q.z) * UI3;
	return -1. + 2. * vec3(q) * UIF;
}


// Tileable 3D worley noise
float worleyNoise(vec3 uv, float freq, bool tileable)
{
	vec3 id = floor(uv);
	vec3 p = fract(uv);
	float minDist = 10000.;

	for (float x = -1.; x <= 1.; ++x)
	{
		for (float y = -1.; y <= 1.; ++y)
		{
			for (float z = -1.; z <= 1.; ++z)
			{
				vec3 offset = vec3(x, y, z);
				vec3 h = vec3(0.);
				if (tileable)
					h = hash33(mod(id + offset, vec3(freq))) * .4 + .3; // [.3, .7]
				else
					h = hash33(id + offset) * .4 + .3; // [.3, .7]
				h += offset;
				vec3 d = p - h;
				minDist = min(minDist, dot(d, d));
			}
		}
	}

	// inverted worley noise
	return 1. - minDist;
}

// Tileable Worley fbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
// chapter in GPU Pro 7.
float worleyFbm(vec3 p, float freq, bool tileable)
{
	float fbm = worleyNoise(p * freq, freq, tileable) * .625 +
		worleyNoise(p * freq * 2., freq * 2., tileable) * .25 +
		worleyNoise(p * freq * 4., freq * 4., tileable) * .125;
	return max(0., fbm * 1.1 - .1);
}

float cloudDetail(float3 p, float c, float y)
{
	p -= WIND_DIR * 3. * TimeTicker * WIND_SPEED;
	// this is super expensive :(
	float hf = worleyFbm(p, CLOUD_DETAIL_FREQ, false) * .625 +
		worleyFbm(p, CLOUD_DETAIL_FREQ * 2., false) * .25 +
		worleyFbm(p, CLOUD_DETAIL_FREQ * 4., false) * .125;
	hf = mix(hf, 1. - hf, y * 4.);
	return remap01(c, hf * .5, 1.);
}
*/

float cloudBase(float3 p, float y)
{
	float3 noise = tex2D(SKY_NOISE_SAMPLER, (p.xy - (WIND_DIR.xy * TimeTicker * WIND_SPEED)) * CLOUD_BASE_FREQ);
	float n = y * y * noise.b + pow(1. - y, 12.);
	float cloud = remap01(noise.r - n, noise.g - 1., 1.);
	return cloud;
}

// #define CLOUD_COVERAGE .1 // lower means more cloud coverage, and vice versa
float CLOUD_COVERAGE = 0.55;
float RainIntensity : RAIN_INTENSITY;
#define CLOUD_TOP_OFFSET 250.

float cloudGradient(float h)
{
	  return smoothstep(0., .05, h) * smoothstep(1.25, .5, h);
}

#define CLOUD_DETAIL_FREQ 0.0218

float cloudDetail(float3 p, float c, float y)
{
	p -= WIND_DIR * 3. * TimeTicker * WIND_SPEED;

	p *= 50;

	// this is super expensive :(
	float hf = tex3D(VOLUMEMAP_SAMPLER, p * CLOUD_DETAIL_FREQ).r * .625 +
		tex3D(VOLUMEMAP_SAMPLER, p * CLOUD_DETAIL_FREQ * 2).r * .25 +
		tex3D(VOLUMEMAP_SAMPLER, p * CLOUD_DETAIL_FREQ * 4).r * .125;
	// hf = lerp(hf, 1. - hf, y * 4.);
	return c * hf;// remap01(c, hf * .5, 1.);
}

float getCloudDensity(float3 p, float y, bool detail)
{
	// p.xy -= WIND_DIR.xy * y * CLOUD_TOP_OFFSET;
	float d = cloudBase(p, y);
	d = remap01(d, CLOUD_COVERAGE, 1.) * (CLOUD_COVERAGE);
	d *= cloudGradient(y);
	bool cloudDetailTest = (d > 0. && d < .3) && detail;
	return ((cloudDetailTest) ? cloudDetail(p, d, y) : d);
}

#define CLOUD_ABSORPTION_TOP 7.8
#define CLOUD_ABSORPTION_BOTTOM 8.6

const float3 noiseKernel[6] = {
	float3(.38051305, .92453449, -.02111345),
	float3(-.50625799, -.03590792, -.86163418),
	float3(-.32509218, -.94557439, .01428793),
	float3(.09026238, -.27376545, .95755165),
	float3(.28128598, .42443639, -.86065785),
	float3(-.16852403, .14748697, .97460106)
};

float marchToLight(float3 p, float3 sunDir, float sunDot, float scatterHeight)
{
	float lightRayStepSize = 0.4;
	float3 lightRayDir = sunDir * lightRayStepSize;
	float3 lightRayDist = lightRayDir * .5;
	float coneSpread = length(lightRayDir);
	float totalDensity = 0.1;
	for (int i = 0; i < 6; ++i)
	{
		// cone sampling as explained in GPU Pro 7 article
		float3 cp = p + lightRayDist * 0.025 + coneSpread * noiseKernel[i] * float(i);
		float y = p.z;
		//cloudHeightFract(length(p));
		if (y > 10000.95 || totalDensity > 0.95) break; // early exit
		totalDensity += getCloudDensity(cp, y, false) * lightRayStepSize;
		lightRayDist += lightRayDir;
	}

	return 32. * exp(-totalDensity * lerp(CLOUD_ABSORPTION_BOTTOM,
		CLOUD_ABSORPTION_TOP, scatterHeight)) * (1. - exp(-totalDensity * 2.));
}

float henyeyGreenstein(float sunDot, float g) {
	float g2 = g * g;
	return (.25 / 03.1415) * ((1. - g2) / pow(1. + g2 - 2. * g * sunDot, 1.5));
}

#define CLOUDS_AMBIENT_TOP float3(1.04, 0.48, 0.12)
#define CLOUDS_AMBIENT_BOTTOM float3(1.04, 0.08, 0.42)

float3 CloudColor : CLOUD_COLOR;

// The per-color weighting to be used for luminance	calculations in	RGB	order.
const float3	LUMINANCE_VECTOR = float3(0.2125f, 0.7154f, 0.0721f);

float getCloudFinalDensity(float3 denP, float denD, float scale)
{
	float density = abs(getCloudDensity(denP * float3(scale, scale, 1) - WIND_DIR * 6. * TimeTicker * WIND_SPEED, denD, true));

	density *= 2;
	density = pow(density, 1.5);
	density *= 2;

	return density;
}

//-------------------------------------------------------------------------------------

float4 pixel_shader(const VtoP IN) : COLOR0
{
	float4 cloudA = 0;// tex2Dbias(DIFFUSEMAP_SAMPLER, IN.tex0);
	float4 cloudAShift = tex2Dbias(DIFFUSEMAP_SAMPLER, IN.tex3);
	float4 cloudACap = 0;// tex2Dbias(MISCMAP1_SAMPLER,   IN.tex1);
	float4 cloudB = 0;//tex2Dbias(MISCMAP2_SAMPLER,   IN.tex2);
	float4 cloudBCap =0;// tex2Dbias(MISCMAP3_SAMPLER,   IN.tex1);

	float4 result = float4(atmosphericScattering(normalize(IN.Viewer), normalize(LightDirVec - float3(0, 0, 0.05)), false), 1);
	result *= 12.0;
	result.xyz = lerp(result.xyz, dot(result.xyz, LUMINANCE_VECTOR), RainIntensity * 0.75);

	result.xyz += calcSunAttenuation(normalize(LightDirVec), normalize(IN.Viewer)) * float3(1.9, 1.5, 1.2);

	float3 rereky = IN.Viewer * float3(1, 1, 1);
	rereky *= 1.5;
	 
  // Render the whispy clouds
	  cloudA.w *= cloudAShift.w * 2;
	  result.xyz = lerp(result.xyz, cloudA.xyz,  cloudA.w * CloudIntensity[0.05]);
     result.xyz = lerp(result.xyz, cloudACap.xyz, cloudACap.w * CloudIntensity[0.05]);

  // Render the overcast clouds
	  result.xyz = lerp(result.xyz, cloudB.xyz,  cloudB.w * CloudIntensity[0.05]);
	  result.xyz = lerp(result.xyz, cloudBCap.xyz,cloudBCap.w * CloudIntensity[0.05]);


	float cloudsValue = 0;

	const int N = 4;
	const int C = 8;

	float3 ro = normalize(IN.Viewer);

	for (int i = 0; i < N; i++)
	{
		cloudsValue += saturate(cloudsFbm(ro * (1 + i * C / N)).x) / N * lerp(ro.z, 1, 0.5f);
	}

	cloudsValue += saturate(cloudsFbm(ro * C).z) / N * (1 - ro.z) * 0.1;

	float nnd;

	float volCV = 0;

	for (int i = 0; i < 7; i++)
		volCV += saturate(cloudsMap(IN.Viewer, (0.0015 + (i - 4) * 0.00005), nnd)) * 2 / 16;

	cloudsValue += pow(volCV * 8, 2);

	cloudsValue = getCloudDensity(rereky, rereky.z, true);
	cloudsValue = getCloudDensity(rereky, rereky.z, true);//  remap01(cloudBase(rereky, rereky.z), CLOUD_COVERAGE, 1.)* cloudGradient(rereky.z);

	cloudsValue = saturate(cloudsValue);

	float4 intScatterTrans = float4(0., 0., 0., 1.);

	float cameraRayStepSize = lerp(0.005, 0.1, RainIntensity * 0.3);

	float sunScatterHeight = smoothstep(600.15, 100.4, clamp(LightDirVec.x, 50., 40.8));
	float sunHeight = 1;// smoothstep(.01, .1, LightDirVec.z + 10.025);

	float sunDot = saturate(dot(normalize(LightDirVec), normalize(rereky)));

	float hgPhase = lerp(henyeyGreenstein(sunDot, .4),
		henyeyGreenstein(sunDot, -.1), .5);
	// sunrise/sunset hack
	hgPhase = max(hgPhase, 1.6 * henyeyGreenstein(sqrt(sunDot),
		saturate(.8 - sunScatterHeight)));
	// shitty night time hack
	hgPhase = lerp(pow(sunDot, .25), hgPhase, sunHeight);

	float totalDensity = 13;

	float3 ambient = 100;
	for (int i = 0; i < 12; ++i)
	{
		float3 p = rereky.xyz + i * cameraRayStepSize * normalize(rereky);
		float heightFract = normalize(rereky.z);// cloudHeightFract(length(p));
		float density = getCloudDensity(p / 10, heightFract, true);
		//if (density > 10.)
		{
			ambient = lerp(CLOUDS_AMBIENT_BOTTOM, CLOUDS_AMBIENT_TOP,
				heightFract);

			float lightMarch = marchToLight(p, normalize(LightDirVec), sunDot, sunScatterHeight);
			lightMarch = clamp(lightMarch, 0, 150);

				// cloud illumination
			float3 luminance = (ambient * saturate(pow(LightDirVec.y + 0.24, 1.4))
				+ result.xyz * .125 + (sunHeight * result.xyz + float3(.0075, .015, .03))
				* SUN_COLOR * hgPhase
				* lightMarch) * (abs(density));

				float transmittance = exp(-density * cameraRayStepSize);
				float3 integScatter = (luminance - luminance * transmittance)
					* (1. / density);
				intScatterTrans.rgb += intScatterTrans.a* integScatter;
				intScatterTrans.a *= transmittance;

			}

		if (intScatterTrans.a < .05)
			break;
	}

	intScatterTrans *= 35;

	cloudsValue = clamp(intScatterTrans, 10, 10);
	//cloudsValue *= 0.5f;
	// 

    intScatterTrans *= lerp(0.35, 0.25, RainIntensity);


	// result.xyz = lerp(result.xyz, CloudColor * 2 * lerp(0.6, 0.5, saturate(clamp(intScatterTrans, 0, 1000) / 300)), saturate(cloudsValue / 2));
	float3 finalClouds = clamp(intScatterTrans, 0, 50) * 4;
	finalClouds = lerp(finalClouds, dot(finalClouds, LUMINANCE_VECTOR), 0.95f);

	float3 denP = (rereky.xyz + 6 * cameraRayStepSize * normalize(rereky));
	float denD = normalize(rereky.z);

	float density = 20;

	density += getCloudFinalDensity(denP, denD, 1);// *0.5;
	density += getCloudFinalDensity(denP, denD, 3);// *0.2;
	density += getCloudFinalDensity(denP, denD, 5); //*0.2;
	density += getCloudFinalDensity(denP, denD, 15);// *0.1;

	density /= 4;

	density = pow(density, 0.2);

	density = lerp(density, 1, 1.1);
	density *= 8;

	density *= 15;


	density = saturate(density);

	finalClouds.b = dot(finalClouds * float3(1, 1, 0), LUMINANCE_VECTOR);

	// finalClouds *= lerp(float3(1, 1, 1), float3(1, 0, 1), saturate(LightDirVec.z));

	float3 cloudsColor = lerp(float3(1.6, 0.3, 0.1), float3(0.3, 0.3, 0.3), saturate(LightDirVec.z));

	result.xyz = lerp(result.xyz, (result.xyz * lerp(0.5, 0.1, RainIntensity * 2) + pow(min(finalClouds, 0.8) * lerp(4, 0.1, saturate(sqrt(sqrt(sqrt(RainIntensity * 0.002))))), lerp(1.7, 1.5, RainIntensity * 0.2)) * cloudsColor), clamp(saturate(dot(min(finalClouds, 1.8) * lerp(3.5, 0.5, sqrt(sqrt(RainIntensity * 1.2))), LUMINANCE_VECTOR)), 0, 0.65) * density) * intScatterTrans;
	//result.xyz = density;

	result.xyz *= lerp(1, 0.1, saturate(RainIntensity * 2));

	//result.xyz = IN.FogAdd.xyz;					// base texture

	// The vertex colour contains white for the sky area and black for the floor
	// of the skydome - this effectively clears the screen.
	// result.xyz *= IN.vertexColour;

	float3 cloudStuff = normalize(IN.Viewer) *(1 + sin(TimeTicker) * 0.5f);
	cloudStuff *= 200;

	float3 cloudSample = tex3Dbias(VOLUMEMAP_SAMPLER, float4(cloudStuff * 0.5 + 0.5, -3));


	result.xyz = pow(result.xyz, 1 / 2.2);
	// result.xyz *= 0.85f;

	// result.xyz = calcSunAttenuation(normalize(LightDirVec), IN.Viewer);

	// result.xyz = DiffuseColour.xyz;

	result.w = 0;

	return result;
}

technique sky <int shader = 1; >
{
	pass p0
	{
		CullMode = NONE;
		ColorWriteEnable = (CWModeFromGame);
		VertexShader = compile vs_1_1 vertex_shader();
		PixelShader = compile ps_3_0 pixel_shader();
	}
}
