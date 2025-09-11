#ifndef HDR_HEADER
#define HDR_HEADER

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

static const float3 LuminanceVector = float3(0.2125f, 0.7154f, 0.0721f);

#endif // HDR_HEADER