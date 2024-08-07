#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

textureCUBE			g_DiffuseTexture;

sampler LinearSampler = sampler_state 
{
	filter = min_mag_mip_linear;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_point;

};

cbuffer CubeColor
{
	vector		g_Color;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
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
	float3		vTexUV : TEXCOORD0;
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

PS_OUT PSTool_MAIN_Wireframe(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Color; //float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT PSTool_CheckMAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = float4(1.f, 1.f, 1.f, 0.3f);

	return Out;
}

PS_OUT PSTool_CubeLook(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Color; //float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

technique11			DefaultTechnique
{
	pass SkyBox
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_SkyBox);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Tool_Cude
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PSTool_MAIN_Wireframe();
	}
	pass Tool_Cude_Check /* 2  -> 트리거 */
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PSTool_CheckMAIN();
	}

	pass Tool_Cude_Look/* 3  -> 트리거Look 방향 체크용  */
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PSTool_CubeLook();
	}
}