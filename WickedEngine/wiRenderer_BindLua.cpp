#include "wiRenderer_BindLua.h"
#include "wiRenderer.h"
#include "wiLines.h"
#include "wiLoader.h"
#include "wiHelper.h"
#include "wiLoader_BindLua.h"
#include "Vector_BindLua.h"
#include "Matrix_BindLua.h"
#include "wiWaterPlane.h"
#include "Texture_BindLua.h"
#include "wiEmittedParticle.h"
#include "wiHairParticle.h"
#include "wiPHYSICS.h"

using namespace wiGraphicsTypes;

namespace wiRenderer_BindLua
{

	void AddTransformName(Transform* root, stringstream& ss)
	{
		if (root == nullptr)
			return;
		ss << root->name << endl;
		for (auto x : root->children)
		{
			if (x != nullptr)
			{
				AddTransformName(x, ss);
			}
		}
	}
	int GetTransforms(lua_State* L)
	{
		stringstream ss("");
		AddTransformName(wiRenderer::GetScene().GetWorldNode(), ss);
		wiLua::SSetString(L, ss.str());
		return 1;
	}
	int GetTransform(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			string name = wiLua::SGetString(L, 1);
			Transform* transform = wiRenderer::getTransformByName(name);
			if (transform != nullptr)
			{
				Object* object = dynamic_cast<Object*>(transform);
				if (object != nullptr)
				{
					Luna<Object_BindLua>::push(L, new Object_BindLua(object));
					return 1;
				}
				Armature* armature = dynamic_cast<Armature*>(transform);
				if (armature != nullptr)
				{
					Luna<Armature_BindLua>::push(L, new Armature_BindLua(armature));
					return 1;
				}

				Luna<Transform_BindLua>::push(L, new Transform_BindLua(transform));
				return 1;
			}
			else
			{
				wiLua::SError(L, "GetTransform(String name) transform not found!");
				return 0;
			}
		}
		return 0;
	}
	int GetArmatures(lua_State* L)
	{
		stringstream ss("");
		for (auto& m : wiRenderer::GetScene().models)
		{
			for (auto& x : m->armatures)
			{
				ss << x->name << endl;
			}
		}
		wiLua::SSetString(L, ss.str());
		return 1;
	}
	int GetArmature(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			string name = wiLua::SGetString(L, 1);
			Armature* armature = wiRenderer::getArmatureByName(name);
			if (armature != nullptr)
			{
				Luna<Armature_BindLua>::push(L, new Armature_BindLua(armature));
				return 1;
			}
			else
			{
				wiLua::SError(L, "GetArmature(String name) armature not found!");
				return 0;
			}
		}
		return 0;
	}
	int GetObjects(lua_State* L)
	{
		stringstream ss("");
		for (auto& m : wiRenderer::GetScene().models)
		{
			for (auto& x : m->objects)
			{
				ss << x->name << endl;
			}
		}
		wiLua::SSetString(L, ss.str());
		return 1;
	}
	int GetObjectLua(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			string name = wiLua::SGetString(L, 1);
			Object* object = wiRenderer::getObjectByName(name);
			if (object != nullptr)
			{
				Luna<Object_BindLua>::push(L, new Object_BindLua(object));
				return 1;
			}
			else
			{
				wiLua::SError(L, "GetObject(String name) object not found!");
				return 0;
			}
		}
		return 0;
	}
	int GetEmitters(lua_State* L)
	{
		stringstream ss("");
		for (auto& x : wiRenderer::emitterSystems)
		{
			ss << x->name << endl;
		}
		wiLua::SSetString(L, ss.str());
		return 1;
		return 0;
	}
	int GetEmitter(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			string name = wiLua::SGetString(L, 1);
			int i = 0;
			for (auto& x : wiRenderer::emitterSystems)
			{
				if (!x->name.compare(name))
				{
					Luna<EmittedParticle_BindLua>::push(L, new EmittedParticle_BindLua(x));
					++i;
				}
			}
			if (i > 0)
			{
				return i;
			}
			wiLua::SError(L, "GetEmitter(string name) no emitter by that name!");
		}
		else
		{
			wiLua::SError(L, "GetEmitter(string name) not enough arguments!");
		}
		return 0;
	}
	int GetMeshes(lua_State* L)
	{
		stringstream ss("");
		for (auto& m : wiRenderer::GetScene().models)
		{
			for (auto& x : m->meshes)
			{
				ss << x.first << endl;
			}
		}
		wiLua::SSetString(L, ss.str());
		return 1;
	}
	int GetLights(lua_State* L)
	{
		stringstream ss("");
		for (auto& m : wiRenderer::GetScene().models)
		{
			for (auto& x : m->lights)
			{
				ss << x->name << endl;
			}
		}
		wiLua::SSetString(L, ss.str());
		return 1;
	}
	int GetMaterials(lua_State* L)
	{
		stringstream ss("");
		for (auto& m : wiRenderer::GetScene().models)
		{
			for (auto& x : m->materials)
			{
				ss << x.first << endl;
			}
		}
		wiLua::SSetString(L, ss.str());
		return 1;
	}
	int GetMaterial(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			string name = wiLua::SGetString(L, 1);
			Material* mat = wiRenderer::getMaterialByName(name);
			if (mat != nullptr)
			{
				Luna<Material_BindLua>::push(L, new Material_BindLua(mat));
				return 1;
			}
		}
		return 0;
	}
	int GetGameSpeed(lua_State* L)
	{
		wiLua::SSetFloat(L, wiRenderer::GetGameSpeed());
		return 1;
	}
	int GetScreenWidth(lua_State* L)
	{
		wiLua::SSetInt(L, wiRenderer::GetDevice()->GetScreenWidth());
		return 1;
	}
	int GetScreenHeight(lua_State* L)
	{
		wiLua::SSetInt(L, wiRenderer::GetDevice()->GetScreenHeight());
		return 1;
	}
	int GetCamera(lua_State* L)
	{
		Luna<Camera_BindLua>::push(L, new Camera_BindLua(wiRenderer::getCamera()));
		return 1;
	}

	int SetGameSpeed(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			wiRenderer::SetGameSpeed(wiLua::SGetFloat(L, 1));
		}
		else
		{
			wiLua::SError(L,"SetGameSpeed(float) not enough arguments!");
		}
		return 0;
	}

	int LoadModel(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 1)
		{
			string dir = wiLua::SGetString(L, 1);
			string name = wiLua::SGetString(L, 2);
			string identifier = "common";
			XMMATRIX transform = XMMatrixIdentity();
			if (argc > 2)
			{
				identifier = wiLua::SGetString(L, 3);
				if (argc > 3)
				{
					Matrix_BindLua* matrix = Luna<Matrix_BindLua>::lightcheck(L, 4);
					if (matrix != nullptr)
					{
						transform = matrix->matrix;
					}
					else
					{
						wiLua::SError(L, "LoadModel(string directory, string name, opt string identifier, opt Matrix transform) argument is not a matrix!");
					}
				}
			}
			Model* model = wiRenderer::LoadModel(dir, name, transform, identifier);
			Luna<Model_BindLua>::push(L, new Model_BindLua(model));
			return 1;
		}
		else
		{
			wiLua::SError(L, "LoadModel(string directory, string name, opt string identifier, opt Matrix transform) not enough arguments!");
		}
		return 0;
	}
	int LoadWorldInfo(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 1)
		{
			string dir = wiLua::SGetString(L, 1);
			string name = wiLua::SGetString(L, 2);
			wiRenderer::LoadWorldInfo(dir, name);
		}
		else
		{
			wiLua::SError(L, "LoadWorldInfo(string directory, string name) not enough arguments!");
		}
		return 0;
	}
	int FinishLoading(lua_State* L)
	{
		wiRenderer::FinishLoading();
		return 0;
	}
	int SetEnvironmentMap(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex != nullptr)
			{
				wiRenderer::SetEnviromentMap(tex->texture);
			}
			else
				wiLua::SError(L, "SetEnvironmentMap(Texture cubemap) argument is not a texture!");
		}
		else
			wiLua::SError(L, "SetEnvironmentMap(Texture cubemap) not enough arguments!");
		return 0;
	}
	int SetColorGrading(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex != nullptr)
			{
				wiRenderer::SetColorGrading(tex->texture);
			}
			else
				wiLua::SError(L, "SetColorGrading(Texture texture2D) argument is not a texture!");
		}
		else
			wiLua::SError(L, "SetColorGrading(Texture texture2D) not enough arguments!");
		return 0;
	}
	int HairParticleSettings(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		int lod0 = 20, lod1 = 50, lod2 = 200;
		if (argc > 0)
		{
			lod0 = wiLua::SGetInt(L, 1);
			if (argc > 1)
			{
				lod1 = wiLua::SGetInt(L, 2);
				if (argc > 2)
				{
					lod2 = wiLua::SGetInt(L, 3);
				}
			}
		}
		wiHairParticle::Settings(lod0, lod1, lod2);
		return 0;
	}
	int SetDirectionalLightShadowProps(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 1)
		{
			wiRenderer::SetDirectionalLightShadowProps(wiLua::SGetInt(L, 1), wiLua::SGetInt(L, 2));
		}
		else
			wiLua::SError(L, "SetDirectionalLightShadowProps(int resolution, int softshadowQuality) not enough arguments!");
		return 0;
	}
	int SetPointLightShadowProps(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 1)
		{
			wiRenderer::SetPointLightShadowProps(wiLua::SGetInt(L, 1), wiLua::SGetInt(L, 2));
		}
		else
			wiLua::SError(L, "SetPointLightShadowProps(int shadowMapCount, int resolution) not enough arguments!");
		return 0;
	}
	int SetSpotLightShadowProps(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 1)
		{
			wiRenderer::SetSpotLightShadowProps(wiLua::SGetInt(L, 1), wiLua::SGetInt(L, 2));
		}
		else
			wiLua::SError(L, "SetSpotLightShadowProps(int shadowMapCount, int resolution) not enough arguments!");
		return 0;
	}
	int SetDebugBoxesEnabled(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			wiRenderer::SetToDrawDebugBoxes(wiLua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetDebugBonesEnabled(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			wiRenderer::SetToDrawDebugBoneLines(wiLua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetVSyncEnabled(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			wiRenderer::GetDevice()->SetVSyncEnabled(wiLua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetPhysicsParams(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			wiRenderer::physicsEngine->rigidBodyPhysicsEnabled = wiLua::SGetBool(L, 1);
			if (argc > 1)
			{
				wiRenderer::physicsEngine->softBodyPhysicsEnabled = wiLua::SGetBool(L, 2);
				if (argc > 2)
				{
					wiRenderer::physicsEngine->softBodyIterationCount = wiLua::SGetInt(L, 3);
				}
			}
		}
		return 0;
	}

	int Pick(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			Ray_BindLua* ray = Luna<Ray_BindLua>::lightcheck(L, 1);
			if (ray != nullptr)
			{
				int pickType = PICKTYPE::PICK_OPAQUE;
				string layer = "", layerDisable = "";
				if (argc > 1)
				{
					pickType = wiLua::SGetInt(L, 2);
					if (argc > 2)
					{
						layer = wiLua::SGetString(L, 3);
						if (argc > 3)
						{
							layerDisable = wiLua::SGetString(L, 4);
						}
					}
				}
				wiRenderer::Picked pick = wiRenderer::Pick(ray->ray, pickType, layer, layerDisable);
				Luna<Object_BindLua>::push(L, new Object_BindLua(pick.object));
				Luna<Vector_BindLua>::push(L, new Vector_BindLua(XMLoadFloat3(&pick.position)));
				Luna<Vector_BindLua>::push(L, new Vector_BindLua(XMLoadFloat3(&pick.normal)));
				wiLua::SSetFloat(L, pick.distance);
				return 4;
			}
		}
		return 0;
	}
	int DrawLine(lua_State* L)
		{
			int argc = wiLua::SGetArgCount(L);
			if (argc > 1)
			{
				Vector_BindLua* a = Luna<Vector_BindLua>::lightcheck(L, 1);
				Vector_BindLua* b = Luna<Vector_BindLua>::lightcheck(L, 2);
				if (a && b)
				{
					XMFLOAT3 xa, xb;
					XMFLOAT4 xc = XMFLOAT4(1, 1, 1, 1);
					XMStoreFloat3(&xa, a->vector);
					XMStoreFloat3(&xb, b->vector);
					if (argc > 2)
					{
						Vector_BindLua* c = Luna<Vector_BindLua>::lightcheck(L, 3);
						if (c)
							XMStoreFloat4(&xc, c->vector);
						else
							wiLua::SError(L, "DrawLine(Vector origin,end, opt Vector color) one or more arguments are not vectors!");
					}
					wiRenderer::linesTemp.push_back(new Lines(xa, xb, xc));
				}
				else
					wiLua::SError(L, "DrawLine(Vector origin,end, opt Vector color) one or more arguments are not vectors!");
			}
			else
				wiLua::SError(L, "DrawLine(Vector origin,end, opt Vector color) not enough arguments!");
			return 0;
		}
	int PutWaterRipple(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 1)
		{
			string name = wiLua::SGetString(L, 1);
			Vector_BindLua* v = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (v)
			{
				XMFLOAT3 pos;
				XMStoreFloat3(&pos, v->vector);
				wiRenderer::PutWaterRipple(name, pos, wiWaterPlane());
			}
			else
				wiLua::SError(L, "PutWaterRipple(String imagename, Vector position) argument is not a Vector!");
		}
		else
			wiLua::SError(L, "PutWaterRipple(String imagename, Vector position) not enough arguments!");
		return 0;
	}

	int PutDecal(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			Decal_BindLua* decal = Luna<Decal_BindLua>::lightcheck(L, 1);
			if (decal != nullptr)
			{
				wiRenderer::PutDecal(decal->decal);
			}
			else
			{
				wiLua::SError(L, "PutDecal(Decal decal) argument is not a Decal!");
			}
		}
		else
		{
			wiLua::SError(L, "PutDecal(Decal decal) not enough arguments!");
		}
		return 0;
	}

	int PutEnvProbe(lua_State* L)
	{
		int argc = wiLua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* pos = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (pos != nullptr)
			{
				XMFLOAT3 p;
				XMStoreFloat3(&p, pos->vector);
				int res = 256;
				if (argc > 1)
				{
					res = wiLua::SGetInt(L, 2);
				}
				wiRenderer::PutEnvProbe(p,res);
			}
			else
			{
				wiLua::SError(L, "PutEnvProbe(Vector pos, opt int resolution = 256) argument is not a Vector!");
			}
		}
		else
		{
			wiLua::SError(L, "PutEnvProbe(Vector pos, opt int resolution = 256) not enough arguments!");
		}
		return 0;
	}


	int ClearWorld(lua_State* L)
	{
		wiRenderer::CleanUpStaticTemp();
		return 0;
	}
	int ReloadShaders(lua_State* L)
	{
		if (wiLua::SGetArgCount(L) > 0)
		{
			wiRenderer::ReloadShaders(wiLua::SGetString(L, 1));
		}
		else 
		{
			wiRenderer::ReloadShaders();
		}
		return 0;
	}

	void Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			wiLua::GetGlobal()->RegisterFunc("GetTransforms", GetTransforms);
			wiLua::GetGlobal()->RegisterFunc("GetTransform", GetTransform);
			wiLua::GetGlobal()->RegisterFunc("GetArmatures", GetArmatures);
			wiLua::GetGlobal()->RegisterFunc("GetArmature", GetArmature);
			wiLua::GetGlobal()->RegisterFunc("GetObjects", GetObjects);
			wiLua::GetGlobal()->RegisterFunc("GetObject", GetObjectLua);
			wiLua::GetGlobal()->RegisterFunc("GetEmitters", GetEmitters);
			wiLua::GetGlobal()->RegisterFunc("GetEmitter", GetEmitter);
			wiLua::GetGlobal()->RegisterFunc("GetMeshes", GetMeshes);
			wiLua::GetGlobal()->RegisterFunc("GetLights", GetLights);
			wiLua::GetGlobal()->RegisterFunc("GetMaterials", GetMaterials);
			wiLua::GetGlobal()->RegisterFunc("GetMaterial", GetMaterial);
			wiLua::GetGlobal()->RegisterFunc("GetGameSpeed", GetGameSpeed);
			wiLua::GetGlobal()->RegisterFunc("GetScreenWidth", GetScreenWidth);
			wiLua::GetGlobal()->RegisterFunc("GetScreenHeight", GetScreenHeight);
			wiLua::GetGlobal()->RegisterFunc("GetCamera", GetCamera);

			wiLua::GetGlobal()->RegisterFunc("SetGameSpeed", SetGameSpeed);

			wiLua::GetGlobal()->RegisterFunc("LoadModel", LoadModel);
			wiLua::GetGlobal()->RegisterFunc("LoadWorldInfo", LoadWorldInfo);
			wiLua::GetGlobal()->RegisterFunc("FinishLoading", FinishLoading);
			wiLua::GetGlobal()->RegisterFunc("SetEnvironmentMap", SetEnvironmentMap);
			wiLua::GetGlobal()->RegisterFunc("SetColorGrading", SetColorGrading);
			wiLua::GetGlobal()->RegisterFunc("HairParticleSettings", HairParticleSettings);
			wiLua::GetGlobal()->RegisterFunc("SetDirectionalLightShadowProps", SetDirectionalLightShadowProps);
			wiLua::GetGlobal()->RegisterFunc("SetPointLightShadowProps", SetPointLightShadowProps);
			wiLua::GetGlobal()->RegisterFunc("SetSpotLightShadowProps", SetSpotLightShadowProps);
			wiLua::GetGlobal()->RegisterFunc("SetDebugBoxesEnabled", SetDebugBoxesEnabled);
			wiLua::GetGlobal()->RegisterFunc("SetDebugBonesEnabled", SetDebugBonesEnabled);
			wiLua::GetGlobal()->RegisterFunc("SetVSyncEnabled", SetVSyncEnabled);
			wiLua::GetGlobal()->RegisterFunc("SetPhysicsParams", SetPhysicsParams);

			wiLua::GetGlobal()->RegisterFunc("Pick", Pick);
			wiLua::GetGlobal()->RegisterFunc("DrawLine", DrawLine);
			wiLua::GetGlobal()->RegisterFunc("PutWaterRipple", PutWaterRipple);
			wiLua::GetGlobal()->RegisterFunc("PutDecal", PutDecal);
			wiLua::GetGlobal()->RegisterFunc("PutEnvProbe", PutEnvProbe);


			wiLua::GetGlobal()->RunText("PICK_VOID = 0x0000000");
			wiLua::GetGlobal()->RunText("PICK_OPAQUE = 0x0000001");
			wiLua::GetGlobal()->RunText("PICK_TRANSPARENT = 0x0000010");
			wiLua::GetGlobal()->RunText("PICK_WATER = 0x0000100");


			wiLua::GetGlobal()->RegisterFunc("ClearWorld", ClearWorld);
			wiLua::GetGlobal()->RegisterFunc("ReloadShaders", ReloadShaders);
		}
	}
};
