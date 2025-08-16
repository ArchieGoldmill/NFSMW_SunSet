#include "global.fx"

float4 LocalEyePos : LOCALEYEPOS;

float4 cvSunDirection;
float4 cvSkyParams;
float4 cvCloudColor;
float4 cvSkyBeta;
float cfTimeTicker;

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
	float3 local_position : TEXCOORD1;
	float4 color : COLOR0;
	float3 view : TEXCOORD2;
};

texture MISCMAP1_TEXTURE;
samplerCUBE MISCMAP1_SAMPLER = sampler_state
{
	texture = <MISCMAP1_TEXTURE>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	MIPFILTER = LINEAR;
	MINFILTER = ANISOTROPIC;
	MAGFILTER = ANISOTROPIC;
	MaxAnisotropy = 8;
};

void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.local_position = IN.position.xyz;
	OUT.local_position.z += 300;
	OUT.uv = float4(IN.tex, IN.tex1);
	OUT.view = LocalEyePos.xyz - IN.position.xyz;
}

#define PI (3.14159265)
#define EARTH_RADIUS (6370997.0)
const float mie_g = -0.99;
const float mie_g2 = -0.99 * -0.99;

float mie_phase(float c, float cos2)
{
	float temp = 1.0 + mie_g2 - 2.0 * mie_g * c;
	temp = smoothstep(0.0, 0.01, temp) * temp;
	//temp = pow(temp, 1.5);
	temp = max(temp, 0.0001);
	return 1.5 * ((1.0 - mie_g2) / (2.0 + mie_g2)) * (1.0 + cos2) / temp;
}

float scale(float inCos)
{
	float x = 1.0 - inCos;
	return 0.25 * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

float3 renderSky(in float3 viewDir, in float3 lightDir)
{
	const float kOuterRadius = EARTH_RADIUS * 1.025;
	const float kOuterRadius2 = kOuterRadius * kOuterRadius;
	const float kInnerRadius = EARTH_RADIUS;
	const float kInnerRadius2 = kInnerRadius * kInnerRadius;

	const float kScale = 1.0 / (kOuterRadius - kInnerRadius);
	const float kScaleDepth = 0.2;
	const float kScaleOverScaleDepth = kScale / kScaleDepth;
	const float kCameraHeight = 0.0;

	const float kRAYLEIGH = cvSkyParams.x / 1000.0;
	const float kMIE = cvSkyParams.y / 1000.0;
	
	const float kR4PI = kRAYLEIGH * 4.0 * PI;
	
	const float kM4PI = kMIE * 4.0 * PI;
	
	viewDir = normalize(viewDir);
	
	float height = kInnerRadius + kCameraHeight;
	float3 cameraPos = float3(0.0, height, 0.0);
	
	float depth = exp(kScaleOverScaleDepth * (-kCameraHeight));
	
	// angle between eye ray and camera height
	float startAngle = dot(viewDir, cameraPos) / height;
	float startAngleScale = scale(startAngle);
	float startOffset = depth * startAngleScale;
	
	// Calculate the length of the "atmosphere"
	float far = sqrt(kOuterRadius2 + kInnerRadius2 * viewDir.y * viewDir.y - kInnerRadius2) - kInnerRadius * viewDir.y;

	float3 pos = cameraPos + far * viewDir;
	
	// Initialize the scattering loop variables
	float sampleLength = far / 2.0;
	float scaledLength = sampleLength * kScale;
	float3 sampleRay = viewDir * sampleLength;
	float3 samplePoint = cameraPos + sampleRay * 0.5;
	
	float3 invLambda = pow(cvSkyBeta.rgb, float3(-4.0, -4.0, -4.0));
	float3 front = float3(0.0, 0.0, 0.0);
	
	const float brightness = lerp(1.3, cvSkyParams.z, cvSkyParams.w);

	{
		float height = length(samplePoint);
		float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
		float lightAngle = dot(lightDir, samplePoint) / height;
		float cameraAngle = dot(viewDir, samplePoint) / height;
		float scatter = (startOffset + depth * (scale(lightAngle) - scale(cameraAngle)));
		float3 atten = exp(-clamp(scatter, 0.0, 50.0) * (invLambda * kR4PI + kM4PI));
		
		front += atten * (depth * scaledLength);
		samplePoint += sampleRay;
	}
		
	float3 c1 = front * invLambda * kRAYLEIGH * brightness;
	
	float3 c2 = front * kMIE * brightness;
	
	float eyeCos = -dot(viewDir, lightDir);
	float eyeCos2 = eyeCos * eyeCos;
	
	float rayleigh = 0.75 * (1.0 + eyeCos2);
	float mie = mie_phase(eyeCos, eyeCos2);
	
	float3 col = /*step(0.0, viewDir.y) * */sqrt(rayleigh * c1 + mie * c2);
	
	return col;
}

float3 GetCloudView(float3 view)
{
	float angle = cfTimeTicker * 0.005;
	float c = cos(angle);
	float s = sin(angle);

	float3x3 rotZ = float3x3(c, -s, 0, s, c, 0, 0, 0, 1);
	float3 cloudDir = mul(normalize(view), rotZ);
	
	return cloudDir;
}

float4 PS_Main(PS_INPUT IN) : COLOR
{
	float3 dir = normalize(IN.local_position.xzy);
	float3 sun = normalize(cvSunDirection.xzy);
	
	float3 color = renderSky(dir, sun);
	
	float noise = frac(sin(dot(dir.xz, float2(12.9898, 78.233))) * 43758.5453);
	color.rgb += noise * color * 0.05;
	
	// stars
	float2 starsUV;
	if (IN.uv.y > 0.09)
	{
		starsUV = IN.uv.xy;
		starsUV.y *= 4;
	}
	else
	{
		starsUV = IN.uv.zw * 4;
	}
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, starsUV);
	float stars_scale = cvSkyParams.w > 0 ? smoothstep(-0.1, -0.3, sun.y) : 1;
	color += pow(diffuse_tex.rgb, 2.2) * stars_scale;
	
	// clouds
	float3 cloudDir = GetCloudView(IN.view);
	float3 cloudTex = texCUBE(MISCMAP1_SAMPLER, cloudDir).rgb;
	
	float gray = dot(cloudTex, float3(0.299, 0.587, 0.114));
	float3 clouds = float3(gray, gray, gray) * cvCloudColor.rgb;
	color = lerp(color, clouds, cvCloudColor.a * gray);
	
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