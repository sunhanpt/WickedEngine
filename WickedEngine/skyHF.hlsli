#ifndef _SKY_HF_
#define _SKY_HF_
#include "globals.hlsli"
#include "objectHF.hlsli"

inline float4 GetSkyColor(in float3 normal)
{
	static const float overBright = 1.02f;

	normal = normalize(normal) * overBright;

	float3 col = DEGAMMA(texture_env_global.SampleLevel(sampler_linear_clamp, normal, 0).rgb);
	float3 sun = max(pow(abs(dot(g_xWorld_SunDir.xyz, normal)), 256)*GetSunColor(), 0) * saturate(dot(g_xWorld_SunDir.xyz, normal));

	return float4(col + sun, 1);
}


#endif // _SKY_HF_
