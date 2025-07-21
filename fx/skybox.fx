float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 cmWorldMat : LOCALWORLD;
float4 ScreenOffset : SCREENOFFSET;
int Cull_Mode : CULL_MODE;
int BaseBlendState[5] : BLENDSTATE;
int BaseTextureFilterParam : BASETEXTUREFILTERPARAM;
int BaseMinTextureFilter : BASEMINTEXTUREFILTER;
int BaseMagTextureFilter : BASEMAGTEXTUREFILTER;

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float3 local_position : TEXCOORD3;
	float4 color : COLOR0;
};

void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.local_position = IN.position.xyz;
}

#define Gamma 2.2
#define Rayleigh 1.
#define Mie 1.
#define RayleighAtt 1.
#define MieAtt 1.2

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
	float3 _betaR = float3(1.95e-2, 1.1e-1, 2.94e-1);
	float3 _betaM = float3(4e-2, 4e-2, 4e-2);
	
	float3 D = normalize(IN.local_position);
	float3 Ds = normalize(float3(0, 1, 1));
	
	float t = max(0.001, D.z) + max(-D.z, -0.001);

	// optical depth -> zenithAngle
	float sR = RayleighAtt / t;
	float sM = MieAtt / t;

	float cosine = clamp(dot(D, Ds), 0.0, 1.0);
	float3 extinction = exp(-(_betaR * sR + _betaM * sM));

	// scattering phase
	float g = -0.9;
	float g2 = g * g;
	float fcos2 = cosine * cosine;
	float miePhase = Mie * pow(1. + g2 + 2. * g * cosine, -1.5) * (1. - g2) / (2. + g2);
	
	float rayleighPhase = Rayleigh;

	float3 inScatter = (1. + fcos2) * float3(rayleighPhase + _betaM / _betaR * miePhase);

	float3 color = inScatter * (1.0 - extinction);

	// sun
	color += 0.47 * float3(1.6, 1.4, 1.0) * pow(cosine, 350.0) * extinction;
	// sun haze
	color += 0.4 * float3(0.8, 0.9, 1.0) * pow(cosine, 2.0) * extinction;
	
	color = ACESFilm(color);
	
	color = pow(color, float3(Gamma, Gamma, Gamma));
	
	return float4(color, 1);
}

technique skybox<int shader = 1;>
{
	pass p0
	{
		CullMode = <Cull_Mode>;
		AlphaTestEnable = (BaseBlendState[0]);
		AlphaRef = (BaseBlendState[1]);
		AlphaBlendEnable = (BaseBlendState[2]);
		SrcBlend = (BaseBlendState[3]);
		DestBlend = (BaseBlendState[4]);

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_Main();
	}
}