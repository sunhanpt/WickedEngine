#include "reconstructPositionHF.hlsli"
#include "tangentComputeHF.hlsli"

//Texture2D<float> xSceneDepthMap:register(t1);
//Texture2D<float4> xTexture:register(t2);
//Texture2D<float4> xNormal:register(t3);

// texture_0	: texture map
// texture_1	: normal map

struct VertexToPixel{
	float4 pos			: SV_POSITION;
	float4 pos2D		: POSITION2D;
};
struct PixelOutputType
{
	float4 col	: SV_TARGET0;
	float4 nor	: SV_TARGET1;
};

CBUFFER(DecalCB, CBSLOT_RENDERER_DECAL)
{
	float4x4 xDecalVP;
	int hasTexNor;
	float3 eye;
	float opacity;
	float3 front;
}

PixelOutputType main(VertexToPixel PSIn)
{
	PixelOutputType Out = (PixelOutputType)0;

	float2 screenPos;
		screenPos.x = PSIn.pos2D.x/PSIn.pos2D.w/2.0f + 0.5f;
		screenPos.y = -PSIn.pos2D.y/PSIn.pos2D.w/2.0f + 0.5f;
	float depth = texture_depth.Load(int3(PSIn.pos.xy,0)).r;
	float3 pos3D = getPosition(screenPos,depth);

	float4 projPos;
		projPos = mul(float4(pos3D,1),xDecalVP);
	float3 projTex;
	float3 clipSpace = projPos.xyz / projPos.w;
		projTex.xy =  clipSpace.xy*float2(0.5f,-0.5f) + 0.5f;
		projTex.z =  clipSpace.z;
	clip( ((saturate(projTex.x) == projTex.x) && (saturate(projTex.y) == projTex.y) && (saturate(projTex.z) == projTex.z))?1:-1 );



	if (hasTexNor & 0x0000010){
		float3 normal = normalize(cross(ddx(pos3D), ddy(pos3D)));
		//clip( dot(normal,front)>-0.2?-1:1 ); //clip at oblique angle
		float4 nortex=texture_1.Sample(sampler_aniso_clamp,projTex.xy);
		float3 eyevector = normalize( eye - pos3D );
		if(nortex.a>0){
			float3x3 tangentFrame = compute_tangent_frame(normal, eyevector, -projTex.xy);
			float3 bumpColor = 2.0f * nortex.rgb - 1.0f;
			//bumpColor.g*=-1;
			normal = normalize(mul(bumpColor, tangentFrame));
		}
		Out.nor.a=min(nortex.b,nortex.a);
		Out.nor.xyz=normal;
	}
	if(hasTexNor & 0x0000001){
		Out.col=texture_0.Sample(sampler_aniso_clamp,projTex.xy);
		Out.col.a*=opacity;
		float3 edgeBlend = clipSpace.xyz;
		edgeBlend.z = edgeBlend.z * 2 - 1;
		edgeBlend = abs(edgeBlend);
		Out.col.a *= 1 - pow(max(max(edgeBlend.x, edgeBlend.y), edgeBlend.z), 8);
		//Out.col.a *= pow(saturate(-dot(normal,front)), 4);
		ALPHATEST(Out.col.a)
		if(hasTexNor & 0x0000010)
			Out.nor.a=Out.col.a;
	}


	return Out;
}