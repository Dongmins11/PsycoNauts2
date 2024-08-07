#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer EffectOption
{
	float3	g_vColor = float3(0.f, 0.f, 0.f);
	float3	g_vEndColor = float3(0.f, 0.f, 0.f);
};

texture2D			g_DiffuseTexture;

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

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - saturate(vDiffuse.r + 0.2f)) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - saturate(vDiffuse.r + 0.2f)) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - saturate(vDiffuse.r + 0.2f)) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = vDiffuse.r;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ALPHADOWN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - saturate(vDiffuse.r + 0.2f)) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - saturate(vDiffuse.r + 0.2f)) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - saturate(vDiffuse.r + 0.2f)) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = saturate(vDiffuse.r - 0.5f);
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

technique11			DefaultTechnique
{
	pass Default_0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass AlphaDown_1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN();
	}
}