#include "Shader_Defines.hpp"


cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

texture2D			g_DiffuseTexture;

cbuffer Camera
{
	vector		g_vCamPosition;
};

cbuffer EffectInfo
{
	bool			g_isDistAlpha = false;

	float3			g_vColor = float3(0.f, 0.f, 0.f);
	float3			g_vEndColor = float3(0.f, 0.f, 0.f);

	float2			g_vMaxUV = float2(1.f, 1.f);

	float2			g_vSize = float2(1.f, 1.f);

	float			g_fTimeAcc = 0.f;
}

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
	float		fPSize : PSIZE;

	float4		vRight : TEXCOORD0;
	float4		vUp : TEXCOORD1;
	float4		vLook : TEXCOORD2;
	float4		vTranslation : TEXCOORD3;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);

	Out.fPSize = In.fPSize;	

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Stream)
{
	GS_OUT			Out[4];

	vector		vLook = normalize(g_vCamPosition - In[0].vPosition);
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * g_vSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * g_vSize.y * 0.5f;


	float4x4	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);

	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);

	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);

	Stream.Append(Out[0]);
	Stream.Append(Out[1]);
	Stream.Append(Out[2]);
	Stream.RestartStrip();

	Stream.Append(Out[0]);
	Stream.Append(Out[2]);
	Stream.Append(Out[3]);
	Stream.RestartStrip();
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

	float4 vAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	float3 vColor = g_vColor;

	if (g_isDistAlpha)
	{
		vColor.r = saturate(vColor.r - ((1.f - saturate(vAlpha.a - g_fTimeAcc)) * saturate((vColor.r - g_vEndColor.r))));
		vColor.g = saturate(vColor.g - ((1.f - saturate(vAlpha.a - g_fTimeAcc)) * saturate((vColor.g - g_vEndColor.g))));
		vColor.b = saturate(vColor.b - ((1.f - saturate(vAlpha.a - g_fTimeAcc)) * saturate((vColor.b - g_vEndColor.b))));
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

PS_OUT PS_MAIN_R(PS_IN In)
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

technique11			DefaultTechnique
{
	pass Default
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Default2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_R();
	}
}