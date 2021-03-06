#ifndef _SHADER_GLOBALS_
#define _SHADER_GLOBALS_
#include "ConstantBufferMapping.h"
#include "SamplerMapping.h"
#include "ResourceMapping.h"

TEXTURE2D(texture_depth, float, TEXSLOT_DEPTH)
TEXTURE2D(texture_lineardepth, float, TEXSLOT_LINEARDEPTH)
TEXTURE2D(texture_gbuffer0, float4, TEXSLOT_GBUFFER0)
TEXTURE2D(texture_gbuffer1, float4, TEXSLOT_GBUFFER1)
TEXTURE2D(texture_gbuffer2, float4, TEXSLOT_GBUFFER2)
TEXTURE2D(texture_gbuffer3, float4, TEXSLOT_GBUFFER3)
TEXTURE2D(texture_gbuffer4, float4, TEXSLOT_GBUFFER4)
TEXTURECUBE(texture_env_global, float4, TEXSLOT_ENV_GLOBAL)
TEXTURECUBE(texture_env0, float4, TEXSLOT_ENV0)
TEXTURECUBE(texture_env1, float4, TEXSLOT_ENV1)
TEXTURECUBE(texture_env2, float4, TEXSLOT_ENV2)
TEXTURE2D(texture_shadow0, float, TEXSLOT_SHADOW0)
TEXTURE2D(texture_shadow1, float, TEXSLOT_SHADOW1)
TEXTURE2D(texture_shadow2, float, TEXSLOT_SHADOW2)
TEXTURECUBE(texture_shadow_cube, float, TEXSLOT_SHADOW_CUBE)
TEXTURE2D(texture_0, float4, TEXSLOT_ONDEMAND0)
TEXTURE2D(texture_1, float4, TEXSLOT_ONDEMAND1)
TEXTURE2D(texture_2, float4, TEXSLOT_ONDEMAND2)
TEXTURE2D(texture_3, float4, TEXSLOT_ONDEMAND3)
TEXTURE2D(texture_4, float4, TEXSLOT_ONDEMAND4)
TEXTURE2D(texture_5, float4, TEXSLOT_ONDEMAND5)
TEXTURE2D(texture_6, float4, TEXSLOT_ONDEMAND6)
TEXTURE2D(texture_7, float4, TEXSLOT_ONDEMAND7)
TEXTURE2D(texture_8, float4, TEXSLOT_ONDEMAND8)
TEXTURE2D(texture_9, float4, TEXSLOT_ONDEMAND9)

SAMPLERSTATE(			sampler_linear_clamp,	SSLOT_LINEAR_CLAMP	)
SAMPLERSTATE(			sampler_linear_wrap,	SSLOT_LINEAR_WRAP	)
SAMPLERSTATE(			sampler_linear_mirror,	SSLOT_LINEAR_MIRROR	)
SAMPLERSTATE(			sampler_point_clamp,	SSLOT_POINT_CLAMP	)
SAMPLERSTATE(			sampler_point_wrap,		SSLOT_POINT_WRAP	)
SAMPLERSTATE(			sampler_point_mirror,	SSLOT_POINT_MIRROR	)
SAMPLERSTATE(			sampler_aniso_clamp,	SSLOT_ANISO_CLAMP	)
SAMPLERSTATE(			sampler_aniso_wrap,		SSLOT_ANISO_WRAP	)
SAMPLERSTATE(			sampler_aniso_mirror,	SSLOT_ANISO_MIRROR	)
SAMPLERCOMPARISONSTATE(	sampler_cmp_depth,		SSLOT_CMP_DEPTH		)

CBUFFER(WorldCB, CBSLOT_RENDERER_WORLD)
{
	float4		g_xWorld_SunDir;
	float4		g_xWorld_SunColor;
	float3		g_xWorld_Horizon;				float xPadding0_WorldCB;
	float3		g_xWorld_Zenith;				float xPadding1_WorldCB;
	float3		g_xWorld_Ambient;				float xPadding2_WorldCB;
	float3		g_xWorld_Fog;					float xPadding3_WorldCB;
	float2		g_xWorld_ScreenWidthHeight;
	float		xPadding_WorldCB[2];
};
CBUFFER(FrameCB, CBSLOT_RENDERER_FRAME)
{
	float3		g_xFrame_WindDirection;			float xPadding0_FrameCB;
	float		g_xFrame_WindTime;
	float		g_xFrame_WindWaveSize;
	float		g_xFrame_WindRandomness;
	float		xPadding_FrameCB[1];
};
CBUFFER(CameraCB, CBSLOT_RENDERER_CAMERA)
{
	float4x4	g_xCamera_View;
	float4x4	g_xCamera_Proj;
	float4x4	g_xCamera_VP;					// View*Projection
	float4x4	g_xCamera_PrevV;
	float4x4	g_xCamera_PrevP;
	float4x4	g_xCamera_PrevVP;				// PrevView*PrevProjection
	float4x4	g_xCamera_ReflVP;				// ReflectionView*ReflectionProjection
	float4x4	g_xCamera_InvVP;				// Inverse View-Projection
	float3		g_xCamera_CamPos;				float xPadding0_CameraCB;
	float3		g_xCamera_At;					float xPadding1_CameraCB;
	float3		g_xCamera_Up;					float xPadding2_CameraCB;
	float		g_xCamera_ZNearP;
	float		g_xCamera_ZFarP;
	float		xPadding_CameraCB[2];
};
CBUFFER(MaterialCB, CBSLOT_RENDERER_MATERIAL)
{
	float4		g_xMat_diffuseColor;
	float4		g_xMat_specular;
	float4		g_xMat_texMulAdd;
	uint		g_xMat_hasRef;
	uint		g_xMat_hasNor;
	uint		g_xMat_hasTex;
	uint		g_xMat_hasSpe;
	uint		g_xMat_shadeless;
	uint		g_xMat_specular_power;
	uint		g_xMat_toon;
	uint		g_xMat_matIndex;
	float		g_xMat_refractionIndex;
	float		g_xMat_metallic;
	float		g_xMat_emissive;
	float		g_xMat_roughness;
};
CBUFFER(DirectionalLightCB, CBSLOT_RENDERER_DIRLIGHT)
{
	float4		g_xDirLight_direction;
	float4		g_xDirLight_col;
	float4		g_xDirLight_mBiasResSoftshadow;
	float4x4	g_xDirLight_ShM[3];
};
CBUFFER(MiscCB, CBSLOT_RENDERER_MISC)
{
	float4x4	g_xTransform;
	float4		g_xColor;
};
CBUFFER(ShadowCB, CBSLOT_RENDERER_SHADOW)
{
	float4x4	g_xShadow_VP;
};

CBUFFER(APICB, CBSLOT_API)
{
	float4		g_xClipPlane;
};


#define ALPHATEST(x)	clip((x)-0.1);
static const float		g_GammaValue = 2.2;
#define DEGAMMA(x)		pow(abs(x),g_GammaValue)
#define GAMMA(x)		pow(abs(x),1.0/g_GammaValue)

inline float3 GetSunColor() { return GAMMA(g_xWorld_SunColor.rgb); }
inline float3 GetHorizonColor() { return GAMMA(g_xWorld_Horizon.rgb); }
inline float3 GetZenithColor() { return GAMMA(g_xWorld_Zenith.rgb); }
inline float3 GetAmbientColor() { return GAMMA(g_xWorld_Ambient.rgb); }
inline float2 GetScreenResolution() { return g_xWorld_ScreenWidthHeight; }
inline float GetScreenWidth() { return g_xWorld_ScreenWidthHeight.x; }
inline float GetScreenHeight() { return g_xWorld_ScreenWidthHeight.y; }
inline float GetTime() { return g_xFrame_WindTime; }

#endif // _SHADER_GLOBALS_