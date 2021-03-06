#include "globals.hlsli"

#define SKINNING_ON

typedef matrix<float,4,4> bonetype;

//CBUFFER(BoneCB, CBSLOT_RENDERER_BONEBUFFER)
//{
//	bonetype pose[MAXBONECOUNT];
//	bonetype prev[MAXBONECOUNT];
//}

struct Bone
{
	bonetype pose, prev;
};

StructuredBuffer<Bone> boneBuffer:register(t0);

inline void Skinning(inout float4 pos, inout float4 posPrev, inout float4 inNor, inout float4 inBon, inout float4 inWei)
{
	[branch]if(inWei.x || inWei.y || inWei.z || inWei.w){
		bonetype sump=0, sumpPrev=0;
		float sumw = 0.0f;
		inWei=normalize(inWei);
		for(uint i=0;i<4;i++){
			sumw += inWei[i];
			sump += boneBuffer.Load((uint)inBon[i]).pose * inWei[i];
			sumpPrev += boneBuffer.Load((uint)inBon[i]).prev * inWei[i];
		}
		sump/=sumw;
		sumpPrev/=sumw;
		pos = mul(pos,sump);
		posPrev = mul(posPrev,sumpPrev);
		inNor.xyz = mul(inNor.xyz,(float3x3)sump);
	}
}
