#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};

cbuffer Status
{
	float	g_fCurState;
	float3	g_vColor = float3(0.f, 0.f, 0.f);
	float3	g_vEndColor = float3(0.f, 0.f, 0.f);
};

texture2D			g_DiffuseTexture;
texture2D			g_AlphaMapTexture;
texture2D			g_NoiseMapTexture;


cbuffer EffectBuffer
{
	bool	g_isUVSprite = false;
	bool	g_isUVAnimation = false;
	bool	g_isFadeIn = false;
	bool	g_isFadeOut = false;
	bool	g_bEmissive = false;
	float	g_fRatio;

	float	g_fFadeInPower = 0.f;
	float	g_fFadeOutPower = 1.f;
	float	g_fGlowStrength = 0.f;

	int		g_fImgMaxCountX = 1;
	int		g_fCurIndexX = 0;
	int		g_fImgMaxCountY = 1;
	int		g_fCurIndexY = 0;

	float	g_fUVAnimX = 0.f;
	float	g_fUVAnimY = 0.f;
};

cbuffer Environment
{
	float	g_fAlpha = 0.f;
};

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;

};

sampler LinearWrapSampler = sampler_state
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
struct PS_OUT_EFFECT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vEmissive : SV_TARGET1;
};
PS_OUT PS_MAIN_ALPHATEST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = float4(g_vColor, 1.f);

	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ALPHAMAP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vDiffuseUV = In.vTexUV;

	if (true == g_isUVSprite)
	{
		vDiffuseUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}
	//0 == UV Animation
	if (true == g_isUVAnimation)
	{
		vDiffuseUV.x += g_fUVAnimX;
		vDiffuseUV.y += g_fUVAnimY;
	}



	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);

	if (true == g_isFadeIn)
	{
		vDiffuse *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vDiffuse *= g_fFadeOutPower;
	}

	Out.vColor = vDiffuse;
	return Out;
}

PS_OUT PS_MAIN_ALPHAANI(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vDiffuseUV = In.vTexUV;
	float2	vAlphaTexUV = In.vTexUV;


	if (true == g_isUVAnimation)
	{
		vAlphaTexUV.x += g_fUVAnimX;
		vAlphaTexUV.y += g_fUVAnimY;
	}

	if (true == g_isUVSprite)
	{
		vDiffuseUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);
	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	vDiffuse.a = vAlpha.r;
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_MAIN_DIFFUSEANI(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vDiffuseUV = In.vTexUV;
	float2	vAlphaTexUV = In.vTexUV;


	if (true == g_isUVAnimation)
	{
		vDiffuseUV.x += g_fUVAnimX;
		vDiffuseUV.y += g_fUVAnimY;
	}

	if (true == g_isUVSprite)
	{
		vDiffuseUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);
	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	vDiffuse.a = vAlpha.r;
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_MAIN_COLORALPHAMAP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	if (true == g_isUVSprite)
	{
		vAlphaTexUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vAlpha.r) * saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vAlpha.r) * saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vAlpha.r) * saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	Out.vColor.a = vAlpha.r* 2.0f;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_COLORALPHATEXTURE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	if (true == g_isUVSprite)
	{
		vAlphaTexUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vAlpha.a) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vAlpha.a) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vAlpha.a) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	Out.vColor.a = vAlpha.a * 2.0f;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_NONALPHA_COLORALPHAMAP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vAlpha.a) * saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vAlpha.a) * saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vAlpha.a) * saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	Out.vColor.a = vAlpha.r;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_NONALPHA_COLORALPHATEXTURE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vAlpha.a) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vAlpha.a) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vAlpha.a) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	Out.vColor.a = vAlpha.a;

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_PSIBLAST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	if (true == g_isUVSprite)
	{
		vAlphaTexUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vAlpha.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vAlpha.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vAlpha.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	Out.vColor.a = vAlpha.a;

	return Out;
}

PS_OUT PS_ENVIRONMENT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	if (true == g_isUVSprite)
	{
		vAlphaTexUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));
	}

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vAlpha.a) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vAlpha.a) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vAlpha.a) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vAlpha *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vAlpha *= g_fFadeOutPower;
	}

	Out.vColor.a = vAlpha.a * g_fAlpha;

	if (Out.vColor.a <= 0.f)
		discard;



	return Out;
}


PS_OUT PS_CLOUD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vDiffuseUV = In.vTexUV;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);

	Out.vColor = 0.3f * vDiffuse.b;
	if (0.f < vDiffuse.r)
	{
		Out.vColor = 1.f * vDiffuse.r;
		Out.vColor.a = 1.f;
	}

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}
PS_OUT_EFFECT PS_PAPER(PS_IN In)
{

	PS_OUT_EFFECT        Out = (PS_OUT_EFFECT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	if (true == g_bEmissive)
		Out.vEmissive = Out.vDiffuse;

	return Out;

}
technique11			DefaultTechnique
{
	pass Alpha_Test
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHATEST();
	}

	pass Alpha_Map
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHAMAP();
	}

	pass Alpha_AlphaAni
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHAANI();
	}

	pass Alpha_DiffuseAni
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIFFUSEANI();
	}

	pass Color_AlphaMap
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLORALPHAMAP();
	}
	pass Color_AlphaTexture
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLORALPHATEXTURE();
	}

	pass Color_NonBlandAlphaMap
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NONALPHA_COLORALPHAMAP();
	}
	pass Color_NonBlandTexture
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NONALPHA_COLORALPHATEXTURE();
	}

	pass PsiBlast
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PSIBLAST();
	}

	pass Environment
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ENVIRONMENT();
	}
	pass Cloud
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CLOUD();
	}

}