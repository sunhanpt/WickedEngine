#include "globals.hlsli"

CBUFFER(FontCB, CBSLOT_FONT_FONT)
{
	float4x4 xTransform;
}

struct VertextoPixel
{
	float4 pos				: SV_POSITION;
	float2 tex				: TEXCOORD0;
};

VertextoPixel main(float2 inPos : POSITION, float2 inTex : TEXCOORD0)
{
	VertextoPixel Out = (VertextoPixel)0;

	Out.pos = mul(float4(inPos, 0, 1), xTransform);
	
	Out.tex=inTex;

	return Out;
}
