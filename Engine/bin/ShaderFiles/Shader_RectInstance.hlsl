#include "Shader_Defines.hpp"


cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

texture2D			g_DiffuseTexture;

cbuffer EffectInfo
{
	bool			g_isDistAlpha = false;
	bool	g_bEmissive = false;

	float3			g_vColor = float3(0.f, 0.f, 0.f);
	float3			g_vEndColor = float3(0.f, 0.f, 0.f);
	
	float2			g_vMaxUV = float2(1.f, 1.f);

	float			g_fTimeAcc = 0.f;
}

cbuffer CameraDesc
{
	vector			g_vCamPosition;
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

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float2		vCurUV : TEXCOORD5;
	float		fAlpha : TEXCOORD6;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fAlpha : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexUV = float2((In.vTexUV.x + In.vCurUV.x) / g_vMaxUV.x, (In.vTexUV.y + In.vCurUV.y) / g_vMaxUV.y);

	if (0.f >= In.fAlpha)
		In.fAlpha = 0.f;

	Out.fAlpha = In.fAlpha;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fAlpha : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};
struct PS_OUT_EFFECT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vEmissive : SV_TARGET1;
};
PS_OUT PS_MAIN_ALPHA_A(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	float3 vColor = g_vColor;

	if (g_isDistAlpha)
	{
		vColor.r = saturate(vColor.r - ((1.f -  saturate(vAlpha.a - g_fTimeAcc)) * saturate((vColor.r - g_vEndColor.r))));
		vColor.g = saturate(vColor.g - ((1.f -  saturate(vAlpha.a - g_fTimeAcc)) * saturate((vColor.g - g_vEndColor.g))));
		vColor.b = saturate(vColor.b - ((1.f -  saturate(vAlpha.a - g_fTimeAcc)) * saturate((vColor.b - g_vEndColor.b))));
		Out.vColor.a = saturate(vAlpha.a - g_fTimeAcc);
	}
	else
	{
		vColor.r = saturate(vColor.r - ((1.f - vAlpha.a) * saturate((vColor.r - g_vEndColor.r))));
		vColor.g = saturate(vColor.g - ((1.f - vAlpha.a) * saturate((vColor.g - g_vEndColor.g))));
		vColor.b = saturate(vColor.b - ((1.f - vAlpha.a) * saturate((vColor.b - g_vEndColor.b))));
		Out.vColor.a = vAlpha.a;
	}
	Out.vColor.rgb = vColor;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ALPHA_R(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	float3 vColor = g_vColor;
	if (g_isDistAlpha)
	{
		vColor.r = saturate(vColor.r - ((1.f - saturate(vAlpha.r - g_fTimeAcc)) * saturate((vColor.r - g_vEndColor.r))));
		vColor.g = saturate(vColor.g - ((1.f - saturate(vAlpha.r - g_fTimeAcc)) * saturate((vColor.g - g_vEndColor.g))));
		vColor.b = saturate(vColor.b - ((1.f - saturate(vAlpha.r - g_fTimeAcc)) * saturate((vColor.b - g_vEndColor.b))));
		Out.vColor.a = saturate(vAlpha.r - g_fTimeAcc);
	}
	else
	{
		vColor.r = saturate(vColor.r - ((1.f - vAlpha.r) * saturate((vColor.r - g_vEndColor.r))));
		vColor.g = saturate(vColor.g - ((1.f - vAlpha.r) * saturate((vColor.g - g_vEndColor.g))));
		vColor.b = saturate(vColor.b - ((1.f - vAlpha.r) * saturate((vColor.b - g_vEndColor.b))));
		Out.vColor.a = vAlpha.r;
	}
	Out.vColor.rgb = vColor;
	
	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DIFFUSE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float3 vColor = vAlpha.rgb;
	if (g_isDistAlpha)
	{
		Out.vColor.a = saturate(vAlpha.r - g_fTimeAcc);
	}
	else
	{
		Out.vColor.a = vAlpha.r;
	}
	Out.vColor.rgb = vColor;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_PARTICE_A(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float3 vColor = g_vColor;
	
	vColor.r = saturate(vColor.r - ((1.f - saturate(vAlpha.a - In.fAlpha)) * saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - saturate(vAlpha.a - In.fAlpha)) * saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - saturate(vAlpha.a - In.fAlpha)) * saturate((vColor.b - g_vEndColor.b))));
	Out.vColor.a = saturate(vAlpha.a - In.fAlpha);

	Out.vColor.rgb = vColor;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_PARTICE_R(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float3 vColor = g_vColor;

	vColor.r = saturate(vColor.r - ((1.f - saturate(vAlpha.r - In.fAlpha)) * saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - saturate(vAlpha.r - In.fAlpha)) * saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - saturate(vAlpha.r - In.fAlpha)) * saturate((vColor.b - g_vEndColor.b))));
	Out.vColor.a = saturate(vAlpha.r - In.fAlpha);

	Out.vColor.rgb = vColor;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}
PS_OUT_EFFECT PS_PAPER(PS_IN In)
{
	PS_OUT_EFFECT		Out = (PS_OUT_EFFECT)0;

	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float3 vColor = g_vColor;

	vColor.r = saturate(vColor.r - ((1.f - saturate(vAlpha.r - In.fAlpha)) * saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - saturate(vAlpha.r - In.fAlpha)) * saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - saturate(vAlpha.r - In.fAlpha)) * saturate((vColor.b - g_vEndColor.b))));
	Out.vDiffuse.a = saturate(vAlpha.r - In.fAlpha);

	Out.vDiffuse.rgb = vColor;
	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	if (true == g_bEmissive)
		Out.vEmissive = Out.vDiffuse;

	if (Out.vDiffuse.a <= 0.f)
		discard;

	return Out;
}
technique11			DefaultTechnique
{
	pass Default
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_A();
	}
	pass Default2
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_R();
	}
	pass Diffuse
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIFFUSE();
	}
	pass Particle_A
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICE_A();
	}
	pass Particle_R
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICE_R();
	}
	pass Paper
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PAPER();
	}
}