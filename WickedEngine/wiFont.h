#pragma once 
#include "CommonInclude.h"
#include "wiGraphicsAPI.h"
#include "ConstantBufferMapping.h"
#define MAX_TEXT 20000

// Do not alter order because it is bound to lua manually
enum wiFontAlign
{
	WIFALIGN_LEFT,
	// same as mid
	WIFALIGN_CENTER,
	// same as center
	WIFALIGN_MID,
	WIFALIGN_RIGHT,
	WIFALIGN_TOP,
	WIFALIGN_BOTTOM,
	WIFALIGN_COUNT,
};

class wiFontProps
{
public:
	float size;
	float spacingX, spacingY;
	float posX, posY;
	wiFontAlign h_align, v_align;

	wiFontProps(float posX = 0, float posY = 0, float size = 14, wiFontAlign h_align = WIFALIGN_LEFT, wiFontAlign v_align = WIFALIGN_TOP
		, float spacingX = 0, float spacingY = 0)
		:posX(posX), posY(posY), size(size), h_align(h_align), v_align(v_align), spacingX(spacingX), spacingY(spacingY)
	{}
};

class wiFont
{
protected:
	GFX_STRUCT Vertex
	{
		XMFLOAT2 Pos;
		XMFLOAT2 Tex;

		ALIGN_16
	};
	static vector<Vertex> vertexList;
	GFX_STRUCT ConstantBuffer
	{
		XMMATRIX mTransform;

		CB_SETBINDSLOT(CBSLOT_FONT_FONT)
		
		ALIGN_16
	};
	static wiGraphicsTypes::GPUBuffer           *vertexBuffer, *indexBuffer;

	static wiGraphicsTypes::VertexLayout		*vertexLayout;
	static wiGraphicsTypes::VertexShader		*vertexShader;
	static wiGraphicsTypes::PixelShader			*pixelShader;
	static wiGraphicsTypes::BlendState			*blendState;
	static wiGraphicsTypes::GPUBuffer			*constantBuffer;
	static wiGraphicsTypes::RasterizerState		*rasterizerState;
	static wiGraphicsTypes::RasterizerState		*rasterizerState_Scissor;
	static wiGraphicsTypes::DepthStencilState	*depthStencilState;
	
	static void SetUpStates();
	static void SetUpCB();
public:
	static void LoadShaders();
	static void BindPersistentState(GRAPHICSTHREAD threadID);
private:
	static void LoadVertexBuffer();
	static void LoadIndices();



	struct wiFontStyle{
		string name;
		wiGraphicsTypes::Texture2D* texture;
		
		struct LookUp{
			int code;
			int offX,offY,width;
		};
		LookUp lookup[128];
		int texWidth,texHeight,charSize,recSize;

		wiFontStyle(){}
		wiFontStyle(const string& newName);
		void CleanUp();
	};
	static vector<wiFontStyle> fontStyles;


	static void ModifyGeo(const wstring& text, wiFontProps props, int style, GRAPHICSTHREAD threadID);

public:
	static void Initialize();
	static void SetUpStaticComponents();
	static void CleanUpStatic();

	wstring text;
	wiFontProps props;
	int style;

	wiFont(const string& text = "", wiFontProps props = wiFontProps(), int style = 0);
	wiFont(const wstring& text, wiFontProps props = wiFontProps(), int style = 0);
	~wiFont();

	
	void Draw(GRAPHICSTHREAD threadID = GRAPHICSTHREAD_IMMEDIATE, bool scissorTest = false);


	int textWidth();
	int textHeight();

	static void addFontStyle( const string& toAdd );
	static int getFontStyleByName( const string& get );

	void SetText(const string& text);
	void SetText(const wstring& text);
	wstring GetText();
	string GetTextA();

	void CleanUp();
};
