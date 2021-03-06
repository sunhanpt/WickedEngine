#include "wiEmittedParticle.h"
#include "wiMath.h"
#include "wiLoader.h"
#include "wiRenderer.h"
#include "wiResourceManager.h"
#include "wiFrustum.h"
#include "wiRandom.h"
#include "ResourceMapping.h"

using namespace wiGraphicsTypes;

VertexLayout	*wiEmittedParticle::vertexLayout = nullptr;
VertexShader  *wiEmittedParticle::vertexShader = nullptr;
PixelShader   *wiEmittedParticle::pixelShader = nullptr,*wiEmittedParticle::simplestPS = nullptr;
GeometryShader		*wiEmittedParticle::geometryShader = nullptr;
GPUBuffer           *wiEmittedParticle::constantBuffer = nullptr;
BlendState		*wiEmittedParticle::blendStateAlpha = nullptr,*wiEmittedParticle::blendStateAdd = nullptr;
RasterizerState		*wiEmittedParticle::rasterizerState = nullptr,*wiEmittedParticle::wireFrameRS = nullptr;
DepthStencilState	*wiEmittedParticle::depthStencilState = nullptr;
set<wiEmittedParticle*> wiEmittedParticle::systems;

wiEmittedParticle::wiEmittedParticle(std::string newName, std::string newMat, Object* newObject, float newSize, float newRandomFac, float newNormalFac
		,float newCount, float newLife, float newRandLife, float newScaleX, float newScaleY, float newRot){
	name=newName;
	object=newObject;
	for(Material*mat : object->mesh->materials)
		if(!mat->name.compare(newMat)){
			material=mat;
			break;
		}

	size=newSize;
	random_factor=newRandomFac;
	normal_factor=newNormalFac;

	count=newCount;
	points.resize(0);
	life=newLife;
	random_life=newRandLife;
	emit=0;
	
	scaleX=newScaleX;
	scaleY=newScaleY;
	rotation = newRot;

	bounding_box=new AABB();
	lastSquaredDistMulThousand=0;
	light=nullptr;
	if(material->blendFlag==BLENDMODE_ADDITIVE){
		light=new Light();
		light->SetUp();
		light->color.x=material->diffuseColor.x;
		light->color.y=material->diffuseColor.y;
		light->color.z=material->diffuseColor.z;
		light->type=Light::POINT;
		light->name="particleSystemLight";
		//light->shadowMap.resize(1);
		//light->shadowMap[0].InitializeCube(wiRenderer::POINTLIGHTSHADOWRES,0,true);
	}

	LoadVertexBuffer();

	XMFLOAT4X4 transform = XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	transform4 = transform;
	transform3 = XMFLOAT3X3(
		transform._11,transform._12,transform._13
		,transform._21,transform._22,transform._23
		,transform._31,transform._32,transform._33
		);

	motionBlurAmount = 0.0f;
}
long wiEmittedParticle::getCount(){return points.size();}


int wiEmittedParticle::getRandomPointOnEmitter(){ return wiRandom::getRandom(object->mesh->indices.size()-1); }


void wiEmittedParticle::addPoint(const XMMATRIX& t4, const XMMATRIX& t3)
{
	vector<SkinnedVertex>& emitterVertexList = object->mesh->vertices;
	int gen[3];
	gen[0] = getRandomPointOnEmitter();
	switch(gen[0]%3)
	{
	case 0:
		gen[1]=gen[0]+1;
		gen[2]=gen[0]+2;
		break;
	case 1:
		gen[0]=gen[0]-1;
		gen[1]=gen[0]+1;
		gen[2]=gen[0]+2;
		break;
	case 2:
		gen[0]=gen[0]-2;
		gen[1]=gen[0]+1;
		gen[2]=gen[0]+2;
		break;
	default:
		break;
	}
	float f = wiRandom::getRandom(0, 1000) * 0.001f, g = wiRandom::getRandom(0, 1000) * 0.001f;
	if (f + g > 1)
	{
		f = 1 - f;
		g = 1 - g;
	}

	XMFLOAT3 pos;
	XMFLOAT3 vel;
	XMVECTOR& vbar=XMVectorBaryCentric(
			XMLoadFloat4(&emitterVertexList[object->mesh->indices[gen[0]]].pos)
		,	XMLoadFloat4(&emitterVertexList[object->mesh->indices[gen[1]]].pos)
		,	XMLoadFloat4(&emitterVertexList[object->mesh->indices[gen[2]]].pos)
		,	f
		,	g
		);
	XMVECTOR& nbar=XMVectorBaryCentric(
			XMLoadFloat4(&emitterVertexList[object->mesh->indices[gen[0]]].nor)
		,	XMLoadFloat4(&emitterVertexList[object->mesh->indices[gen[1]]].nor)
		,	XMLoadFloat4(&emitterVertexList[object->mesh->indices[gen[2]]].nor)
		,	f
		,	g
		);
	XMStoreFloat3( &pos, XMVector3Transform( vbar, t4 ) );
	XMStoreFloat3( &vel, XMVector3Normalize( XMVector3Transform( nbar, t3 ) ));
			
	float vrand = (normal_factor*getNewVelocityModifier())/60.0f;

	vel.x*=vrand;
	vel.y*=vrand;
	vel.z*=vrand;

		//pos.x+=getNewPositionModifier();
		//pos.y+=getNewPositionModifier();
		//pos.z+=getNewPositionModifier();


	points.push_back(Point(pos, XMFLOAT4(size, 1, (float)wiRandom::getRandom(0, 1), (float)wiRandom::getRandom(0, 1)), vel/*, XMFLOAT3(1,1,1)*/, getNewLifeSpan()
		,rotation*getNewRotationModifier(),scaleX,scaleY ) );
}
void wiEmittedParticle::Update(float gamespeed)
{
	systems.insert(this);


	XMFLOAT3 minP=XMFLOAT3(FLOAT32_MAX,FLOAT32_MAX,FLOAT32_MAX)
		,maxP=XMFLOAT3(-FLOAT32_MAX,-FLOAT32_MAX,-FLOAT32_MAX);

	for (unsigned int i = 0; i<points.size(); ++i){
		Point &point = points[i];

		point.pos.x += point.vel.x*gamespeed;
		point.pos.y += point.vel.y*gamespeed;
		point.pos.z += point.vel.z*gamespeed;
		point.rot += point.rotVel*gamespeed;
		

		/*if(point.maxLife-point.life<point.maxLife*0.1f)
			point.sizOpaMir.y-=0.05f*gamespeed;
		if(point.maxLife-point.life>point.maxLife*0.9f)
			point.sizOpaMir.y+=0.05f*gamespeed;
		if(point.sizOpaMir.y<=0) point.sizOpaMir.y=0;
		if(point.sizOpaMir.y>=1) point.sizOpaMir.y=1;*/

		point.life-=/*1.0f/60.0f**/gamespeed;
		point.life=wiMath::Clamp(point.life,0,point.maxLife);

		float lifeLerp = point.life/point.maxLife;
		point.sizOpaMir.x=wiMath::Lerp(point.sizBeginEnd[1],point.sizBeginEnd[0],lifeLerp);
		point.sizOpaMir.y=wiMath::Lerp(1,0,lifeLerp);

		
		minP=wiMath::Min(XMFLOAT3(point.pos.x-point.sizOpaMir.x,point.pos.y-point.sizOpaMir.x,point.pos.z-point.sizOpaMir.x),minP);
		maxP=wiMath::Max(XMFLOAT3(point.pos.x+point.sizOpaMir.x,point.pos.y+point.sizOpaMir.x,point.pos.z+point.sizOpaMir.x),maxP);
	}
	bounding_box->create(minP,maxP);

	while(!points.empty() && points.front().life<=0)
		points.pop_front();


	XMFLOAT4X4& transform = object->world;
	transform4 = transform;
	transform3 = XMFLOAT3X3(
		transform._11,transform._12,transform._13
		,transform._21,transform._22,transform._23
		,transform._31,transform._32,transform._33
		);
	XMMATRIX t4=XMLoadFloat4x4(&transform4), t3=XMLoadFloat3x3(&transform3);
	
	emit += (float)count/60.0f*gamespeed;

	bool clearSpace=false;
	if(points.size()+emit>=MAX_PARTICLES)
		clearSpace=true;

	for(int i=0;i<(int)emit;++i)
	{
		if(clearSpace)
			points.pop_front();

		addPoint(t4,t3);
	}
	if((int)emit>0)
		emit=0;
	
	if(light!=nullptr){
		light->translation_rest=bounding_box->getCenter();
		light->enerDis=XMFLOAT4(5,bounding_box->getRadius()*3,0,0);
		light->UpdateLight();
	}
	
	//MAPPED_SUBRESOURCE mappedResource;
	//Point* vertexPtr;
	//GRAPHICSTHREAD_IMMEDIATE->Map(vertexBuffer,0,MAP_WRITE_DISCARD,0,&mappedResource);
	//vertexPtr = (Point*)mappedResource.pData;
	//memcpy(vertexPtr,renderPoints.data(),sizeof(Point)* renderPoints.size());
	//GRAPHICSTHREAD_IMMEDIATE->Unmap(vertexBuffer,0);
}
void wiEmittedParticle::Burst(float num)
{
	XMMATRIX t4=XMLoadFloat4x4(&transform4), t3=XMLoadFloat3x3(&transform3);

	static float burst = 0;
	burst+=num;
	for(int i=0;i<(int)burst;++i)
		addPoint(t4,t3);
	burst-=(int)burst;
}


void wiEmittedParticle::Draw(Camera* camera, GRAPHICSTHREAD threadID, int FLAG)
{
	if(!points.empty()){
		

		if(camera->frustum.CheckBox(bounding_box->corners))
		{
			GraphicsDevice* device = wiRenderer::GetDevice();
			device->EventBegin(L"EmittedParticle", threadID);
			
			vector<Point> renderPoints=vector<Point>(points.begin(),points.end());
			device->UpdateBuffer(vertexBuffer,renderPoints.data(),threadID,sizeof(Point)* renderPoints.size());

			bool additive = (material->blendFlag==BLENDMODE_ADDITIVE || material->premultipliedTexture);

			device->BindPrimitiveTopology(PRIMITIVETOPOLOGY::POINTLIST,threadID);
			device->BindVertexLayout(vertexLayout,threadID);
			device->BindPS(wireRender?simplestPS:pixelShader,threadID);
			device->BindVS(vertexShader,threadID);
			device->BindGS(geometryShader,threadID);
		
			//device->BindResourcePS(depth,1,threadID);

			ConstantBuffer cb;
			cb.mAdd.x = additive;
			cb.mAdd.y = (FLAG==DRAW_DARK?true:false);
			cb.mMotionBlurAmount = motionBlurAmount;
		

			device->UpdateBuffer(constantBuffer,&cb,threadID);
			device->BindConstantBufferGS(constantBuffer, CB_GETBINDSLOT(ConstantBuffer),threadID);

			device->BindRasterizerState(wireRender?wireFrameRS:rasterizerState,threadID);
			device->BindDepthStencilState(depthStencilState,1,threadID);
	
			device->BindBlendState((additive?blendStateAdd:blendStateAlpha),threadID);

			device->BindVertexBuffer(vertexBuffer,0,sizeof(Point),threadID);

			if(!wireRender && material->texture) 
				device->BindResourcePS(material->texture,TEXSLOT_ONDEMAND0,threadID);
			device->Draw(renderPoints.size(),threadID);


			device->BindGS(nullptr,threadID);
			device->EventEnd(threadID);
		}
	}
}
void wiEmittedParticle::DrawPremul(Camera* camera, GRAPHICSTHREAD threadID, int FLAG){
	if(material->premultipliedTexture)
		Draw(camera,threadID,FLAG);
}
void wiEmittedParticle::DrawNonPremul(Camera* camera, GRAPHICSTHREAD threadID, int FLAG){
	if(!material->premultipliedTexture)
		Draw(camera,threadID,FLAG);
}


void wiEmittedParticle::CleanUp()
{

	points.clear();

	SAFE_DELETE(vertexBuffer);

	systems.erase(this);

	delete bounding_box;
	bounding_box=nullptr;

	//delete(this);
}



void wiEmittedParticle::LoadShaders()
{
	VertexLayoutDesc layout[] =
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, FORMAT_R32G32B32A32_FLOAT, 0, APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD", 2, FORMAT_R32G32B32A32_FLOAT, 0, APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION", 0, FORMAT_R32_FLOAT, 0, APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, FORMAT_R32G32B32_FLOAT, 0, APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	VertexShaderInfo* vsinfo = static_cast<VertexShaderInfo*>(wiResourceManager::GetShaderManager()->add(wiRenderer::SHADERPATH + "pointspriteVS.cso", wiResourceManager::VERTEXSHADER, layout, numElements));
	if (vsinfo != nullptr){
		vertexShader = vsinfo->vertexShader;
		vertexLayout = vsinfo->vertexLayout;
	}


	pixelShader = static_cast<PixelShader*>(wiResourceManager::GetShaderManager()->add(wiRenderer::SHADERPATH + "pointspritePS.cso", wiResourceManager::PIXELSHADER));
	simplestPS = static_cast<PixelShader*>(wiResourceManager::GetShaderManager()->add(wiRenderer::SHADERPATH + "pointspritePS_simplest.cso", wiResourceManager::PIXELSHADER));

	geometryShader = static_cast<GeometryShader*>(wiResourceManager::GetShaderManager()->add(wiRenderer::SHADERPATH + "pointspriteGS.cso", wiResourceManager::GEOMETRYSHADER));



}
void wiEmittedParticle::SetUpCB()
{
	GPUBufferDesc bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = CPU_ACCESS_WRITE;
	constantBuffer = new GPUBuffer;
    wiRenderer::GetDevice()->CreateBuffer( &bd, NULL, constantBuffer );
}
void wiEmittedParticle::SetUpStates()
{
	RasterizerStateDesc rs;
	rs.FillMode=FILL_SOLID;
	rs.CullMode=CULL_BACK;
	rs.FrontCounterClockwise=true;
	rs.DepthBias=0;
	rs.DepthBiasClamp=0;
	rs.SlopeScaledDepthBias=0;
	rs.DepthClipEnable=false;
	rs.ScissorEnable=false;
	rs.MultisampleEnable=false;
	rs.AntialiasedLineEnable=false;
	rasterizerState = new RasterizerState;
	wiRenderer::GetDevice()->CreateRasterizerState(&rs,rasterizerState);

	
	rs.FillMode=FILL_WIREFRAME;
	rs.CullMode=CULL_NONE;
	rs.FrontCounterClockwise=true;
	rs.DepthBias=0;
	rs.DepthBiasClamp=0;
	rs.SlopeScaledDepthBias=0;
	rs.DepthClipEnable=false;
	rs.ScissorEnable=false;
	rs.MultisampleEnable=false;
	rs.AntialiasedLineEnable=false;
	wireFrameRS = new RasterizerState;
	wiRenderer::GetDevice()->CreateRasterizerState(&rs,wireFrameRS);




	
	DepthStencilStateDesc dsd;
	dsd.DepthEnable = false;
	dsd.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = COMPARISON_LESS;

	dsd.StencilEnable = false;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	dsd.FrontFace.StencilFailOp = STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = STENCIL_OP_INCR;
	dsd.FrontFace.StencilPassOp = STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	dsd.BackFace.StencilFailOp = STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = STENCIL_OP_DECR;
	dsd.BackFace.StencilPassOp = STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = COMPARISON_ALWAYS;

	// Create the depth stencil state.
	depthStencilState = new DepthStencilState;
	wiRenderer::GetDevice()->CreateDepthStencilState(&dsd, depthStencilState);


	
	BlendStateDesc bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.RenderTarget[0].BlendEnable=true;
	bd.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	bd.IndependentBlendEnable=true;
	blendStateAlpha = new BlendState;
	wiRenderer::GetDevice()->CreateBlendState(&bd,blendStateAlpha);

	ZeroMemory(&bd, sizeof(bd));
	bd.RenderTarget[0].BlendEnable=true;
	bd.RenderTarget[0].SrcBlend = BLEND_ONE;
	bd.RenderTarget[0].DestBlend = BLEND_ONE;
	bd.RenderTarget[0].BlendOp = BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	bd.IndependentBlendEnable=true;
	blendStateAdd = new BlendState;
	wiRenderer::GetDevice()->CreateBlendState(&bd,blendStateAdd);
}
void wiEmittedParticle::LoadVertexBuffer()
{
	vertexBuffer=NULL;

	GPUBufferDesc bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( Point ) * MAX_PARTICLES;
    bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = CPU_ACCESS_WRITE;
	vertexBuffer = new GPUBuffer;
    wiRenderer::GetDevice()->CreateBuffer( &bd, NULL, vertexBuffer );
}
void wiEmittedParticle::SetUpStatic()
{
	LoadShaders();
	SetUpCB();
	SetUpStates();

	systems.clear();
}
void wiEmittedParticle::CleanUpStatic()
{
	SAFE_DELETE(vertexLayout);
	SAFE_DELETE(vertexShader);
	SAFE_DELETE(pixelShader);
	SAFE_DELETE(simplestPS);
	SAFE_DELETE(geometryShader);
	SAFE_DELETE(constantBuffer);
	SAFE_DELETE(blendStateAlpha);
	SAFE_DELETE(blendStateAdd);
	SAFE_DELETE(rasterizerState);
	SAFE_DELETE(wireFrameRS);
	SAFE_DELETE(depthStencilState);
}
long wiEmittedParticle::getNumParticles()
{
	long retval=0;
	for(wiEmittedParticle* e:systems)
		if(e)
			retval+=e->getCount();
	return retval;
}