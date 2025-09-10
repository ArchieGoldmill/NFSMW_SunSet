#define kCompressCoeff ( 1.0f )
float3 CompressColourSpace(float3 colour)
{
	return colour / (kCompressCoeff + colour);
}

float3 DeCompressColourSpace(float3 colour)
{
	float3 clr = max(0.01, kCompressCoeff - colour);
	return colour / clr;
}

bool BloomThreshold(float3 hdrColor)
{
	return any(hdrColor > 1.0);
}

float3 ACESFilm(float3 x)
{
	float tA = 2.51;
	float tB = 0.03;
	float tC = 2.43;
	float tD = 0.59;
	float tE = 0.14;
	return clamp((x * (tA * x + tB)) / (x * (tC * x + tD) + tE), 0.0, 1.0);
}

static const float3 LuminanceVector = float3(0.2125f, 0.7154f, 0.0721f);