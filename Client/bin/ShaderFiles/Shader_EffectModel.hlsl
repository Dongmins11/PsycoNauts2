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
	float	g_fDecreaseColor = 0.f;
	float	g_fFadeSet = 0.f;
	float	g_fAlphaDown = 0.f;
};

texture2D			g_DiffuseTexture;
texture2D			g_AlphaMapTexture;
texture2D			g_NoiseMapTexture;
texture2D			g_FadeMapTexture;

cbuffer EffectBuffer
{
	bool	g_isUVSprite = false;
	bool	g_isUVAnimation = false;
	bool	g_isFadeIn = false;
	bool	g_isFadeOut = false;

	float	g_fRatio;

	float	g_fDonutY = 1.f;

	float	g_fFadeInPower = 0.f;
	float	g_fFadeOutPower = 1.f;
	float	g_fGlowStrength = 0.f;

	int		g_fImgMaxCountX = 1;
	int		g_fCurIndexX = 0;
	int		g_fImgMaxCountY = 1;
	int		g_fCurIndexY = 0;

	float	g_fUVAnimX = 0.f;
	float	g_fUVAnimY = 0.f;

	float	g_fTimeAcc = 0.f;
	float	g_fUVTimeAcc = 0.f;
};

cbuffer Dissovle
{
	float g_fDissolveTime = 0.f;
	bool  g_bDissolve = false;
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

sampler LinearMirrorSampler = sampler_state
{
	filter = min_mag_mip_linear;

	AddressU = mirror;
	AddressV = mirror;
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
	float3		vTangent : TANGENT;
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

PS_OUT PS_MAIN_ALPHATEST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor.rgb = g_vColor;
	Out.vColor.a = 1.f;

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

PS_OUT PS_CHOP_EFFECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vNoiseUV = In.vTexUV;
	float2	vFadeUV = float2((1.f - In.vTexUV.x), In.vTexUV.y);
	vNoiseUV.x -= g_fTimeAcc * 0.2f;
	vFadeUV.x += g_fTimeAcc;

	float2 vDistortionUV = In.vTexUV.x + g_fTimeAcc;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vNoiseUV); //점박이
	float4 vDistortion = g_AlphaMapTexture.Sample(LinearWrapSampler, vDistortionUV); //디스토션 
	float fWeight = vDistortion.r * 0.2f;
	float4 vColor = g_NoiseMapTexture.Sample(LinearWrapSampler, vNoiseUV + fWeight); //디스토션 활성화

	vector vNoise = g_NoiseMapTexture.Sample(LinearSampler, vNoiseUV); //알록이
	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV); //알파
	vector vFade; //페이드

	if (1.3f <= vFadeUV.x)
		vFade = g_FadeMapTexture.Sample(LinearMirrorSampler, vFadeUV); //페이드
	else
		vFade = vector(0.f, 0.f, 0.f, 0.f); //페이드

	float3 vEmphasis1 = g_vColor / (1.f - vNoise);
	float3 vEmphasis2 = g_vColor / (1.f - vDiffuse);
	//Out.vColor.rgb = g_vColor / (1 - vColor);
	vAlpha.r = saturate(vAlpha.r + 0.2f);
	Out.vColor.a = (vDiffuse.r * vAlpha.r + 0.3f);
	Out.vColor.a *= saturate(vFade.r) ;

	if (0 >= Out.vColor.a)
		discard;

	float3 vEndColor = float3(1.f, 1.f, 0.f);
	vEndColor.r = saturate(vEndColor.r - ((1.f - Out.vColor.a) *   saturate((vEndColor.r - (vEmphasis1 * vEmphasis2).r))));
	vEndColor.g = saturate(vEndColor.g - ((1.f - Out.vColor.a) *   saturate((vEndColor.g - (vEmphasis1 * vEmphasis2).g))));
	vEndColor.b = saturate(vEndColor.b - ((1.f - Out.vColor.a) *   saturate((vEndColor.b - (vEmphasis1 * vEmphasis2).b))));

	Out.vColor.rgb = vEndColor;

	return Out;
}

PS_OUT PS_LEAPATTACK_EFFECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vNoiseUV = In.vTexUV;
	float2	vFadeUV = float2((1.f - In.vTexUV.x), In.vTexUV.y);
	//vNoiseUV.x -= g_fTimeAcc * 0.2f;
	vFadeUV.x += g_fTimeAcc;

	float2 vDistortionUV = In.vTexUV;
	vDistortionUV.x = g_fTimeAcc;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vNoiseUV); //점박이
	float4 vDistortion = g_AlphaMapTexture.Sample(LinearWrapSampler, vDistortionUV); //디스토션 
	float fWeight = vDistortion.r * 0.2f;
	float4 vColor = g_NoiseMapTexture.Sample(LinearWrapSampler, vNoiseUV + fWeight); //디스토션 활성화

	vector vNoise = g_NoiseMapTexture.Sample(LinearSampler, vNoiseUV); //알록이
	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV); //알파
	vector vFade; //페이드

	if (1.3f <= vFadeUV.x)
		vFade = g_FadeMapTexture.Sample(LinearMirrorSampler, vFadeUV); //페이드
	else
		vFade = vector(0.f, 0.f, 0.f, 0.f); //페이드

	float3 vEmphasis1 = g_vColor / (1.f - vNoise);
	float3 vEmphasis2 = g_vColor / (1.f - vDiffuse);
	Out.vColor.rgb = vEmphasis1 * vEmphasis2;
	//Out.vColor.rgb = g_vColor / (1 - vColor);
	vAlpha.r = saturate(vAlpha.r + 0.2f);
	Out.vColor.a = (vDiffuse.r * vAlpha.r);
	Out.vColor.a *= saturate(vFade.r);
	return Out;
}

PS_OUT PS_DIZZISTAR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vDiffuseUV = In.vTexUV;
	
	vDiffuseUV = float2((In.vTexUV.x / (float)g_fImgMaxCountX) + (g_fCurIndexX / (float)g_fImgMaxCountX), (In.vTexUV.y / (float)g_fImgMaxCountY) + ((g_fCurIndexY / (float)g_fImgMaxCountY)));

	//vDiffuseUV = (vDiffuseUV / g_fTimeAcc) + (0.5 - (vDiffuseUV / (2 * g_fTimeAcc))) - (1 - vDiffuseUV) / (2 * g_fTimeAcc);
	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);
	
	vDiffuse.rgb = g_vColor * vDiffuse.rgb;
	vDiffuse.a *= g_fTimeAcc;
	Out.vColor = vDiffuse;
	return Out;
}

PS_OUT PS_CHARGESINGLEWAVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	vAlphaTexUV.y = (1.f - In.vTexUV.y);
	vAlphaTexUV.y += g_fTimeAcc;
	
	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	float3 vColor = g_vColor;
	vColor.g = vColor.g - ((1.f - vAlpha.r) * g_fDecreaseColor);
	vAlpha *= 1.f - (In.vTexUV.y * g_fFadeSet);
	float4 vDiffuse;
	vDiffuse.rgb = vColor;
	vDiffuse.a = vAlpha.r * 2.5f;
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_CHARGEWAVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	vAlphaTexUV -= g_fTimeAcc;

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);
	vector vNoise = g_NoiseMapTexture.Sample(LinearWrapSampler, In.vTexUV);

	float3 vColor = g_vColor;
	vColor.g = vColor.g - ((1.f - vAlpha.r) * g_fDecreaseColor);
	float4 vDiffuse;
	vDiffuse.rgb = vColor;
	vDiffuse.a = saturate(vAlpha.r - 0.1f) * 0.5f  * vNoise;
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_IMPACTWAVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2	vAlphaTexUV = In.vTexUV;
	vAlphaTexUV += g_fTimeAcc;

	vector vAlpha = g_AlphaMapTexture.Sample(LinearWrapSampler, vAlphaTexUV);

	float3 vColor = g_vColor;
	vColor.g = vColor.g - ((1.f - vAlpha.r) * g_fDecreaseColor);
	float4 vDiffuse;
	vDiffuse.rgb = vColor;
	vDiffuse.a = saturate(vAlpha.r - 0.1f) * 0.5f;
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_PSIBLAST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDistortion = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV); //디스토션 
	float fWeight = vDistortion.r * 0.2f + vDistortion.g * 0.2f;
	vector vDiffuse = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV);
	float4 vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV + fWeight); //디스토션 활성화
			
	vColor.r = saturate(g_vColor.r - ((1.f - vColor.r) *   saturate((g_vColor.r - vColor.r))));
	vColor.g = saturate(g_vColor.g - ((1.f - vColor.g) *   saturate((g_vColor.g - vColor.g))));
	vColor.b = saturate(g_vColor.b - ((1.f - vColor.b) *   saturate((g_vColor.b - vColor.b))));

	Out.vColor.rgb = vColor.rgb;

	if (true == g_isFadeIn)
	{
		vDiffuse *= g_fFadeInPower;
	}
	if (true == g_isFadeOut && false == g_isFadeIn)
	{
		vDiffuse *= g_fFadeOutPower;
	}
	
	if (vDiffuse.a == 0.f)
	{
		Out.vColor.a = vDiffuse.a;
	}
	else
	{
		Out.vColor.a = vDiffuse.r;
	}


	return Out;
}

PS_OUT PS_DONUTRING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV.y *= g_fDonutY;
	vDiffuseUV.x += g_fTimeAcc;
	//vDiffuseUV.y += g_fTimeAcc / g_fDonutY;

	float4 vDistortion = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV); //디스토션 
	float fWeight = vDistortion.r * 0.2f + vDistortion.g * 0.2f;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV + fWeight);

	float3 vColor = g_vColor;
	if (0.5 > vDiffuse.r)
		vColor = g_vEndColor;

	Out.vColor.rgb = vColor;

	if (true == g_isFadeIn)
	{
		vDiffuse *= g_fFadeSet;
	}

	Out.vColor.a = saturate(vDiffuse.r - g_fAlphaDown);

	return Out;
}

PS_OUT PS_PURSERING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	
	vDiffuseUV.x += g_fTimeAcc;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vDiffuse.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vDiffuse.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vDiffuse.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;

	Out.vColor.a = saturate(vDiffuse.r * ((4.f + g_fTimeAcc) - vDiffuseUV.x));
	if (vDiffuseUV.x - g_fTimeAcc < 1.f)
		Out.vColor.a = saturate(vDiffuse.r * (vDiffuseUV.x - g_fTimeAcc));
	

	return Out;
}

PS_OUT PS_WATERWAVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vTexUvOne = In.vTexUV;

	vTexUvOne = (vTexUvOne / g_fTimeAcc) + (0.5f - (vTexUvOne / (2.f* g_fTimeAcc))) - (1.f - vTexUvOne) / (2.f * g_fTimeAcc);

	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vTexUvOne);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vDiffuse.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vDiffuse.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vDiffuse.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = vDiffuse.r;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_BLAINSTORM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV.x += g_fTimeAcc;

	float4 vDistortion = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV); //디스토션 
	float fWeight = vDistortion.r * 0.5f;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV + fWeight);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vDiffuse.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vDiffuse.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vDiffuse.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = vDiffuse.r;
	
	if (true == g_bDissolve)
	{
		vector vDissolve = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV);
		if (vDissolve.r - g_fDissolveTime <= 0.05f)
		{
			Out.vColor.rgb = float3(0.f, 0.2f, 1.f);
		}
		else if (vDissolve.r - g_fDissolveTime <= 0.10f)
		{
			Out.vColor.rgb = float3(0.f, 1.f, 1.f);
		}
		else if (vDissolve.r - g_fDissolveTime <= 0.15f)
		{
			Out.vColor.rgb = float3(0.f, 0.2f, 1.f);
		}

		if (vDissolve.r - g_fDissolveTime <= 0.0f)
		{
			discard;
		}
	}

	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_BALLOON(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV.x += g_fTimeAcc;

	float4 vDistortion = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV); //디스토션 
	float fWeight = vDistortion.r * 0.5f;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV + fWeight);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vColor.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vColor.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vColor.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = 0.5f;

	if (true == g_bDissolve)
	{
		vector vDissolve = g_AlphaMapTexture.Sample(LinearWrapSampler, In.vTexUV);
		if (vDissolve.r - g_fDissolveTime <= 0.05f)
		{
			Out.vColor.rgb = float3(0.f, 0.2f, 1.f);
		}
		else if (vDissolve.r - g_fDissolveTime <= 0.10f)
		{
			Out.vColor.rgb = float3(0.f, 1.f, 1.f);
		}
		else if (vDissolve.r - g_fDissolveTime <= 0.15f)
		{
			Out.vColor.rgb = float3(0.f, 0.2f, 1.f);
		}

		if (vDissolve.r - g_fDissolveTime <= 0.0f)
		{
			discard;
		}
	}

	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_SLIDINGMESH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV += g_fTimeAcc;

	vector vDiffuse = g_AlphaMapTexture.Sample(LinearWrapSampler, vDiffuseUV);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - saturate(vDiffuse.a + 0.2f)) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - saturate(vDiffuse.a + 0.2f)) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - saturate(vDiffuse.a + 0.2f)) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = vDiffuse.a;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_WATER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV += g_fTimeAcc;

	float4 vDistortion = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV); //디스토션 
	float fWeight = vDistortion.r * 0.25f;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV + fWeight);

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vDiffuse.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vDiffuse.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vDiffuse.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = vDiffuse.a;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_BLENDWAVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV.x += g_fUVTimeAcc;


	//float4 vDistortion = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV); //디스토션 
	//float fWeight = vDistortion.r * 0.25f;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, vDiffuseUV);
	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vDiffuse.r) *   saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vDiffuse.r) *   saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vDiffuse.r) *   saturate((vColor.b - g_vEndColor.b))));

	Out.vColor.rgb = vColor;
	Out.vColor.a = 1.f;
	if (Out.vColor.a <= 0.0f)
		discard;

	if (g_fUVTimeAcc <= vDiffuseUV.y)
		discard;
	return Out;
}

technique11			DefaultTechnique
{
	pass Alpha_Test_0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHATEST();
	}

	pass Alpha_Map_1
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHAMAP();
	}

	pass Alpha_AlphaAni_2
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHAANI();
	}

	pass Alpha_DiffuseAni_3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIFFUSEANI();
	}

	pass ChopEffect_4
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CHOP_EFFECT();
	}

	pass LeapAttackEffect_5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEAPATTACK_EFFECT();
	}

	pass DizziStar_6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DIZZISTAR();
	}

	pass ChargeSingleWave_7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CHARGESINGLEWAVE();
	}

	pass ChargeWave_8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CHARGEWAVE();
	}

	pass ImpactWave_9
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_IMPACTWAVE();
	}

	pass PSIBlast_10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PSIBLAST();
	}

	pass DonutRing_11
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DONUTRING();
	}
	
	pass PurseRing_12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PURSERING();
	}

	pass WaterWave_13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_WATERWAVE();
	}

	pass BrainStorm_14
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLAINSTORM();
	}

	pass SlidingMesh_15
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SLIDINGMESH();
	}

	pass BrainStorm_16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BALLOON();
	}
	pass Water_17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_WATER();
	}	
	pass BlendWave_18
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLENDWAVE();
	}
}