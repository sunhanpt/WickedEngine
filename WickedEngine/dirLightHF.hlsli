#ifndef _DIRLIGHT_HF_
#define _DIRLIGHT_HF_


// dir light constant buffer is global

//Texture2D<float> xTextureSh[3]:register(t13);

inline float offset_lookup(Texture2D<float> intex, SamplerComparisonState map,
                     float2 loc,
                     float2 offset,
					 float scale,
					 float realDistance)
{
	float BiasedDistance = realDistance - g_xDirLight_mBiasResSoftshadow.x;

	return intex.SampleCmpLevelZero( map, loc + offset / scale, BiasedDistance).r;
}


inline float shadowCascade(float4 shadowPos, float2 ShTex, Texture2D<float> shadowTexture){
	float realDistance = shadowPos.z/shadowPos.w;
	float sum = 0;
	float scale = g_xDirLight_mBiasResSoftshadow.y;
	float retVal = 1;
	
#ifdef DIRECTIONALLIGHT_SOFT
	//[branch]if(g_xDirLight_mBiasResSoftshadow.z)
	{
		float samples = 0.0f;
		static const float range = 1.5f;
		for (float y = -range; y <= range; y += 1.0f)
			for (float x = -range; x <= range; x += 1.0f)
			{
				sum += offset_lookup(shadowTexture, sampler_cmp_depth, ShTex, float2(x, y), scale, realDistance);
				samples++;
			}

		retVal *= sum / samples;
	}
	//else 
#else
		retVal *= offset_lookup(shadowTexture, sampler_cmp_depth, ShTex, float2(0, 0), scale, realDistance);
#endif

	return retVal;
}

inline float dirLight(in float3 pos3D, in float3 normal, inout float4 color)
{
	float difLight=max(dot(normal.xyz, g_xDirLight_direction.xyz),0);
	float4 ShPos[3];
		ShPos[0] = mul(float4(pos3D,1),g_xDirLight_ShM[0]);
		ShPos[1] = mul(float4(pos3D,1),g_xDirLight_ShM[1]);
		ShPos[2] = mul(float4(pos3D,1),g_xDirLight_ShM[2]);
	float3 ShTex[3];
		ShTex[0] = ShPos[0].xyz*float3(1,-1,1)/ShPos[0].w/2.0f +0.5f;
		ShTex[1] = ShPos[1].xyz*float3(1,-1,1)/ShPos[1].w/2.0f +0.5f;
		ShTex[2] = ShPos[2].xyz*float3(1,-1,1)/ShPos[2].w/2.0f +0.5f;
	const float shadows[3]={
		shadowCascade(ShPos[0],ShTex[0].xy,texture_shadow0),
		shadowCascade(ShPos[1],ShTex[1].xy,texture_shadow1),
		shadowCascade(ShPos[2],ShTex[2].xy,texture_shadow2)
	};
	[branch]if((saturate(ShTex[2].x) == ShTex[2].x) && (saturate(ShTex[2].y) == ShTex[2].y) && (saturate(ShTex[2].z) == ShTex[2].z))
	{
		//color.r+=0.5f;
		const float2 lerpVal = abs( ShTex[2].xy*2-1 );
		difLight *= lerp( shadows[2],shadows[1], pow( max(lerpVal.x,lerpVal.y),4 ) );
	}
	else [branch]if((saturate(ShTex[1].x) == ShTex[1].x) && (saturate(ShTex[1].y) == ShTex[1].y) && (saturate(ShTex[1].z) == ShTex[1].z))
	{ 
		//color.g+=0.5f;
		const float2 lerpVal = abs( ShTex[1].xy*2-1 );
		difLight *= lerp( shadows[1],shadows[0], pow( max(lerpVal.x,lerpVal.y),4 ) );
	}
	else [branch]if((saturate(ShTex[0].x) == ShTex[0].x) && (saturate(ShTex[0].y) == ShTex[0].y) && (saturate(ShTex[0].z) == ShTex[0].z))
	{ 
		//color.b+=0.5f;
		difLight *= shadows[0];
	}
	return difLight;
}


// MACROS

#define DEFERRED_DIRLIGHT_MAIN																										\
	float lighting = dirLight(P,N,color);																		\
	color.rgb *= lighting;																											\
	applySpecular(color, (color*lighting).rgb, N, V, g_xDirLight_direction.xyz, 1, specular_power, specular);

#endif // _DIRLIGHT_HF_