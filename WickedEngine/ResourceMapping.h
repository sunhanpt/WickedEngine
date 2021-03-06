#ifndef _RESOURCEBUFFER_MAPPING_H_
#define _RESOURCEBUFFER_MAPPING_H_

// Slot matchings:
////////////////////////////////////////////////////

// Structured Buffers (t slot):

#define SBSLOT_BONE			0


// Textures (t slot):

#define TEXSLOT_DEPTH		0
#define TEXSLOT_LINEARDEPTH	1

#define TEXSLOT_GBUFFER0	2
#define TEXSLOT_GBUFFER1	3
#define TEXSLOT_GBUFFER2	4
#define TEXSLOT_GBUFFER3	5
#define TEXSLOT_GBUFFER4	6

#define TEXSLOT_ENV_GLOBAL	7
#define TEXSLOT_ENV0		8
#define TEXSLOT_ENV1		9
#define TEXSLOT_ENV2		10

#define TEXSLOT_SHADOW0		11
#define TEXSLOT_SHADOW1		12
#define TEXSLOT_SHADOW2		13
#define TEXSLOT_SHADOW_CUBE	14

#define TEXSLOT_ONDEMAND0	15
#define TEXSLOT_ONDEMAND1	16
#define TEXSLOT_ONDEMAND2	17
#define TEXSLOT_ONDEMAND3	18
#define TEXSLOT_ONDEMAND4	19
#define TEXSLOT_ONDEMAND5	20
#define TEXSLOT_ONDEMAND6	21
#define TEXSLOT_ONDEMAND7	22
#define TEXSLOT_ONDEMAND8	23
#define TEXSLOT_ONDEMAND9	24
#define TEXSLOT_ONDEMAND_COUNT	(TEXSLOT_ONDEMAND9 - TEXSLOT_ONDEMAND0 + 1)

#define TEXSLOT_COUNT		TEXSLOT_ONDEMAND9


///////////////////////////
// Helpers:
///////////////////////////

// CPP:
/////////

#define STRUCTUREDBUFFER_BINDSLOT __StructuredBuffer_BindSlot__
// Add this to a struct to match that with a bind slot:
#define STRUCTUREDBUFFER_SETBINDSLOT(slot) static const int STRUCTUREDBUFFER_BINDSLOT = (slot);
// Get bindslot from a struct which is matched with a bind slot:
#define STRUCTUREDBUFFER_GETBINDSLOT(structname) structname::STRUCTUREDBUFFER_BINDSLOT



// Shader:
//////////

// Automatically binds resources on the shader side:

#define STRUCTUREDBUFFER_X(name, type, slot) StructuredBuffer< type > name : register(t ## slot)
#define STRUCTUREDBUFFER(name, slot) STRUCTUREDBUFFER_X(name, type, slot)

#define TEXTURE2D_X(name, type, slot) Texture2D< type > name : register(t ## slot);
#define TEXTURE2D(name, type, slot) TEXTURE2D_X(name, type, slot)
#define TEXTURECUBE_X(name, type, slot) TextureCube< type > name : register(t ## slot);
#define TEXTURECUBE(name, type, slot) TEXTURECUBE_X(name, type, slot)


#endif // _RESOURCEBUFFER_MAPPING_H_
