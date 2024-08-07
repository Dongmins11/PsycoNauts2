BlendState	AlphaBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
};

BlendState	Blending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

BlendState	NonBlending
{
	BlendEnable[0] = false;
};

//BOOL DepthEnable;
//D3D11_DEPTH_WRITE_MASK DepthWriteMask;
//D3D11_COMPARISON_FUNC DepthFunc;
//BOOL StencilEnable;
//UINT8 StencilReadMask;
//UINT8 StencilWriteMask;
//D3D11_DEPTH_STENCILOP_DESC FrontFace;
//D3D11_DEPTH_STENCILOP_DESC BackFace;

DepthStencilState DepthStencil_Default
{
	DepthEnable = true;
	DepthWriteMask = all;	
	DepthFunc = less;
};

DepthStencilState DepthStencil_Priority
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};

/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/

RasterizerState Rasterizer_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState Rasterizer_CullNone
{
	FillMode = solid;
	CullMode = NONE;
	FrontCounterClockwise = false;
};


RasterizerState Rasterizer_Default
{
	FillMode = solid;
	CullMode = back;	
	FrontCounterClockwise = false;
};

RasterizerState Rasterizer_SkyBox
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

