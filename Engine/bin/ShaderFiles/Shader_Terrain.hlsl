#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer TileDetail
{
	float			g_fTileDetail = 1.0f;
};

texture2D			g_SourTexture;
texture2D			g_DestTexture;
texture2D			g_FilterTexture;

sampler LinearSampler = sampler_state 
{
	filter = min_mag_mip_linear;

	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_point;

};



struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;	
	float4		vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexUV = In.vTexUV;

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;	
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};
struct PS_OUT_Default
{
	float4		vColor : SV_TARGET0;

};
PS_OUT PS_MAIN_2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vSourDiffuse = g_SourTexture.Sample(LinearSampler, In.vTexUV * g_fTileDetail);
	vector		vDestDiffuse = g_DestTexture.Sample(LinearSampler, In.vTexUV * g_fTileDetail);

	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vSourDiffuse * vFilter + vDestDiffuse * (1.f - vFilter);		
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0);
	// Out.vDepth = vector(0 ~ 1의 깊이(투영스페이스상의 깊이), n ~ f(뷰스페이스 상의깊이, 투영변환을 위해 사용했던 z))
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);

	return Out;
}
PS_OUT_Default PS_MAIN(PS_IN In)
{
	PS_OUT_Default		Out = (PS_OUT_Default)0;

	vector		vSourDiffuse = g_SourTexture.Sample(LinearSampler, In.vTexUV * g_fTileDetail);
	vector		vDestDiffuse = g_DestTexture.Sample(LinearSampler, In.vTexUV * g_fTileDetail);

	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vSourDiffuse * vFilter + vDestDiffuse * (1.f - vFilter);
	Out.vColor.a = 1.f;


	return Out;
}
technique11			DefaultTechnique
{
	pass Default
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass WireFrame
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_2();
	}
}