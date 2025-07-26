#include "global.fx"
#include "normalmap.fx"

float4 cvSunDirection;
float4 cvSkyBetaR;
float4 cvSkyBetaM;
float4 cvSkyParams;
float4 cvCloudColor;
float cfCloudScroll : CLOUDSCROLL;

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL0;
	float4 uv : TEXCOORD0;
	float3 local_position : TEXCOORD3;
	float4 color : COLOR0;
};

void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.local_position = IN.position.xyz;
	OUT.local_position.z += 2000;
	OUT.uv = float4(IN.tex, IN.tex1);
}

#define Gamma cvSkyBetaM.w
#define Rayleigh cvSkyParams.x
#define RayleighAtt cvSkyParams.y
#define Mie cvSkyParams.z
#define MieAtt cvSkyParams.w

float3 ACESFilm(float3 x)
{
	float tA = 2.51;
	float tB = 0.03;
	float tC = 2.43;
	float tD = 0.59;
	float tE = 0.14;
	return clamp((x * (tA * x + tB)) / (x * (tC * x + tD) + tE), 0.0, 1.0);
}

float4 PS_Main(PS_INPUT IN) : COLOR
{
	float3 _betaR = cvSkyBetaR.rgb;
	float3 _betaM = cvSkyBetaM.rgb;
	
	float3 D = normalize(IN.local_position);
	float3 Ds = normalize(cvSunDirection.xyz);
	
	float t = max(0.001, D.z) + max(-D.z, -0.001);

	// optical depth -> zenithAngle
	float sR = RayleighAtt / t;
	float sM = MieAtt / t;

	float cosine = saturate(dot(D, Ds));
	float3 extinction = exp(-(_betaR * sR + _betaM * sM));

	// scattering phase
	float g = cvSkyBetaR.w;
	float g2 = g * g;
	float fcos2 = cosine * cosine;
	float miePhase = Mie * pow(1. + g2 + 2. * g * cosine, -1.5) * (1. - g2) / (2. + g2);
	
	float rayleighPhase = Rayleigh;

	float3 inScatter = (1. + fcos2) * float3(rayleighPhase + _betaM / _betaR * miePhase);

	float3 color = inScatter * (1.0 - extinction);

	// sun
	color += 0.47 * float3(1.6, 1.4, 1.0) * pow(cosine, 350.0) * extinction;
	// sun haze
	color += 0.4 * float3(0.8, 0.9, 1.0) * pow(cosine, 0.0) * extinction;
	
	color = ACESFilm(color);
	
	color = pow(color, float3(Gamma, Gamma, Gamma));
	
	// stars
	if (Rayleigh < 0.2)
	{
		float2 starsUV;
		if (IN.uv.y > 0.08)
		{
			starsUV = IN.uv;
		}
		else
		{
			starsUV = IN.local_position.xy / 10000;
		}
	
		float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, starsUV);
		color += pow(diffuse_tex.rgb, 2) * smoothstep(0.2, 0.1, Rayleigh);
	}
	
	// clouds
	float2 cloudUV = IN.uv.xy;
	cloudUV.x += cfCloudScroll * 0.1;
	float4 clouds = tex2D(NORMALMAP_SAMPLER, cloudUV) * cvCloudColor;
	color += clouds.rgb * clouds.a;
	
	return float4(color, 1.0);
}

technique skybox<int shader = 1;>
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_Main();
	}
}