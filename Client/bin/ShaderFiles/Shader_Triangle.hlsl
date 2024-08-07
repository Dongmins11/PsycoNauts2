#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

texture2D			g_DiffuseTexture;

cbuffer Rgba
{
	float4			g_Rgba; 
};
sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_point;

};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_RGBA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor= g_Rgba;

	return Out;
}

BlendState Blend
{

	//BlendOp[0] = ADD;
	//SrcBlendAlpha[0] = SRC_ALPHA;
	//DestBlendAlpha[0] = INV_SRC_ALPHA;
	//BlendOpAlpha[0] = ADD;
	//RenderTargetWriteMask[0] = 0x0F;
};

technique11			DefaultTechnique
{
	pass Default1
	{
	
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
		SetBlendState(NonBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetRasterizerState(Rasterizer_Default);
		SetDepthStencilState(DepthStencil_Default, 0);
	}

	pass Rgba_Wire
	{

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RGBA();
		SetBlendState(NonBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetRasterizerState(Rasterizer_CullNone);
		SetDepthStencilState(DepthStencil_Default,0);
	}

	pass Rgba_Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RGBA();
		SetBlendState(Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetRasterizerState(Rasterizer_CullNone);
		SetDepthStencilState(DepthStencil_Default, 0);
	}

	pass Rgba_DepthOff
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RGBA();
		SetBlendState(NonBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetRasterizerState(Rasterizer_Default);
		SetDepthStencilState(DepthStencil_Priority, 0);
	}
}