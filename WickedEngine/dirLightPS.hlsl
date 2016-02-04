#include "lightHF.hlsli"
#include "dirLightHF.hlsli"



float4 main( VertexToPixel PSIn ) : SV_TARGET
{
	DEFERREDLIGHT_MAKEPARAMS(g_xDirLight_col)

	DEFERRED_DIRLIGHT_MAIN

	DEFERREDLIGHT_RETURN
}