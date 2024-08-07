
#include "Shader_Defines.hpp"
#include "Shader_BRDF.hlsl"
#include "Shader_Calculation.hlsl"
cbuffer Matrices
{
	matrix         g_WorldMatrix;
	matrix         g_ViewMatrix;
	matrix         g_ProjMatrix;
};

cbuffer InverseMatrices
{
	matrix         g_ViewMatrixInv;
	matrix         g_ProjMatrixInv;
};

cbuffer LightDesc
{
	float4      g_vLightDir;
	float4      g_vLightPos;
	float      g_fRange = 0.f;
	float4      g_vLightDiffuse;
	float4      g_vLightAmbient;
	float4      g_vLightSpecular;
	float      g_fOuterConeAngle;
	float      g_fInnerConeAngle;
	float4      g_vMtrlDiffuse = float4(1.f, 1.f, 1.f, 1.f);
	float4      g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
	float4      g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
	float      g_fPower = 3.5f;
	float	   g_fShadeAmbient = 0.4f;
	float		g_fOffset = 1.f;
	uint		g_iAttType = 0;
};

cbuffer CameraDesc
{
	vector         g_vCamPosition;
};

cbuffer OutlineDesc
{
	float g_fWinCX = 1280.f;
	float g_fWinCY = 720.f;

	float g_fCoord[5] = { -2.f,-1.f,0.f,1.f,2.f };
	float g_fLaplacianMask[25] = {
		0.f,0.f,-1.f,0.f,0.f,
		0.f,-1.f,-2.f,-1.f,0.f,
		-1.f,-2.f,16.f,-2.f,-1.f,
		0.f,-1.f,-2.f,-1.f,0.f,
		0.f,0.f,-1.f,0.f,0.f
	};
};

cbuffer RTCubeMatrix
{
	matrix         g_CubeViewMatrix[6];
	matrix         g_CubeProjMatrix;
};
cbuffer DepthBlur
{
	float4			g_vDofParam = 0;
};
cbuffer RenderOption
{
	bool g_bShadow = false;
	bool g_bEmissive = false;
};
cbuffer TimeAcc
{
	float		g_fTimeAcc = 0.f;
};

cbuffer ToneMapping
{
	uint			g_iToneOption = 0;

};
cbuffer BlurOption
{
	uint			g_iBlurOption = 0;

};
cbuffer BloomWeight
{
	float			g_fDiffuseWeight = 1.f;
	float			g_fBloomColorWeight = 1.f;
	float			g_fBlurColor2Weight = 1.f;
	float			g_fBlurColor4Weight = 1.f;

};
cbuffer AmbinetOcculusion
{
	bool			g_bAO = true;

};

cbuffer UVPercent
{
	float		g_fUVPercent = 0.f;

};
struct tagSSAO_In
{
	float2 vUV;
	float fDepth;
	float fViewZ;
	float3 vNormal;
};
cbuffer SSAO
{
	float g_fSSAORadious = 0.001f;
	float g_fBias = 0.0003f;

};
cbuffer Divide
{
	float2			g_vRatio;
	float2			g_vRatioMax;

};
cbuffer Alpha
{
	float			g_fAlpha = 1.f;
	float4			g_vColor;
};
cbuffer HPHeal
{
	int			g_iHpIndex = 0;
};
cbuffer Shadow
{
	float			g_fShadowBias = 0.000125f;
	float			g_fPercentLimitMax = 0.5f;
};

cbuffer ProgressBar
{
	float g_fRatio = 1.f;
};
cbuffer MSAA
{
	float			g_fMsaaOffset = 0.125f;
	float			g_fEdge_Threshold_Min = 0.0312f;
	float			g_fEdge_Threshold_Max = 0.125f;
	float2			g_vInverseScreenSize;
	uint			g_iMaxRepeat = 12;
	float			g_fSubPixel_Quality = 0.75f;
	uint			g_iSampler = 0;
	bool			g_iLuma = false;
	bool			g_bTest = false;

};
cbuffer Fog
{
	float4 g_fFogcolor = (float4) 0;
	float g_fFogDenstiy = (float)0;
	float g_fFogDist = (float)0;
	float g_fFogstart = (float)0;
	float g_fFogend = (float)0;
	float g_fFogfalloff = (float)0;
	bool g_fFogType = (bool)0;
};

cbuffer Radial
{
	int				g_iRadialCnt = 14;
	float			g_iRadialBlurPower = 0.03f;
	float2			g_vRadialDir = float2(0.5f, 0.5f);
};
cbuffer Dissolve
{
	float g_fDissolveTime = 0.f;
	bool  g_bDissolve = false;
	float g_fDissolveStart = 0.1f;
};
texture2D         g_DiffuseTexture;
textureCUBE         g_DiffuseTexture3D;
texture2D         g_ShadeTexture;
texture2D         g_NormalTexture;
texture2D         g_DepthTexture;
texture2D         g_SpecularTexture;
texture2D         g_OutlineTexture;
texture2D         g_RimlightTexture;
texture2D         g_RomTexture;
texture2D         g_DiffuseBRDFTexture;
texture2D         g_RoughnessTexture;
texture2D         g_MetalnessTexture;
texture2D         g_NonIBLTexture;
texture2D         g_HDRTexture2D;
textureCUBE         g_HDRTexture;
textureCUBE         g_IrradianceTexture;
textureCUBE         g_PreFilterTexture;
texture2D         g_BRDF_LUTTexture;
texture2D         g_DepthShadowTexture;
texture2D         g_ShadowMapTexture;
texture2D         g_TestTexture;
texture2D         g_HorizontalTexture;
texture2D         g_TotalBlurTexture;
texture2D         g_VelocityTexture;
texture2D         g_MotionBlurTexture;
texture2D         g_BrightnessTexture;
texture2D         g_BloomTexture;
texture2D		  g_ToneMappingTexture;
texture2D		  g_EmissiveTexture;
texture2D		  g_DepthBlurTexture;
texture2D		  g_DOFTexture;
texture2D			g_NoiseTexture;
texture2D			g_SpiralTexture;
texture2D			g_MixTexture;
texture2D			g_BloomColorTexture;
texture2D			g_BlurColor2Texture;
texture2D			g_BlurColor4Texture;
texture2D			g_SSAOTexture;
texture2D			g_EffectTexture;

texture2D			g_RadiusLoadingTexture;
texture2D			g_LutTexture;
texture2D			g_NonLightDefferedTexture;

texture2D			g_DissolveTexture;
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
sampler LinearSamplerClamp = sampler_state
{
	filter = min_mag_mip_linear;

	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerComparisonState CascadedPcfShadowMapSampler
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	BorderColor = float4(1.f, 1.f, 1.f, 1.f);
	ComparisonFunc = LESS_EQUAL;
};
sampler AnisotropicSamplerLinear = sampler_state
{
	filter = anisotropic;
	MaxAnisotropy = 16;

	AddressU = wrap;
	AddressV = wrap;
};
sampler AnisotropicSamplerLinearClamp = sampler_state
{
	filter = anisotropic;
	MaxAnisotropy = 16;

	AddressU = clamp;
	AddressV = clamp;
};
sampler AnisotropicSamplerMirror = sampler_state
{
	filter = anisotropic;
	MaxAnisotropy = 16;

	AddressU = mirror;
	AddressV = mirror;
};
struct VS_IN
{
	float3      vPosition : POSITION;
	float2      vTexUV : TEXCOORD0;
};
struct VS_IN_CUBE
{
	float3      vPosition : POSITION;
};
struct VS_OUT
{
	float4      vPosition : SV_POSITION;
	float4      vLocalPosition:POSITION;
	float2      vTexUV : TEXCOORD0;
};
struct VS_OUT_CUBE
{
	float4      vPosition : SV_POSITION;
	float4      vLocalPosition:POSITION;

};
struct VS_OUT_BLUR
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
	float2      vTexUV1 : TEXCOORD1;
	float2      vTexUV2 : TEXCOORD2;
	float2      vTexUV3 : TEXCOORD3;
	float2      vTexUV4 : TEXCOORD4;
	float2      vTexUV5 : TEXCOORD5;
	float2      vTexUV6 : TEXCOORD6;
	float2      vTexUV7 : TEXCOORD7;
	float2      vTexUV8 : TEXCOORD8;
	float2      vTexUV9 : TEXCOORD9;
};
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	matrix      matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vLocalPosition = vector(In.vPosition, 1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}
VS_OUT_CUBE VS_MAIN_CUBE(VS_IN_CUBE In)
{
	VS_OUT_CUBE      Out = (VS_OUT_CUBE)0;


	matrix      matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vLocalPosition = vector(In.vPosition, 1.f);


	return Out;
}
VS_OUT_BLUR VS_MAIN_HORISON(VS_IN In)
{
	VS_OUT_BLUR      Out = (VS_OUT_BLUR)0;

	matrix      matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	float   TexelSize;

	TexelSize = 1.f / 1280.f;

	Out.vTexUV1 = In.vTexUV + float2(TexelSize * -4.f, 0.f);
	Out.vTexUV2 = In.vTexUV + float2(TexelSize * -3.f, 0.f);
	Out.vTexUV3 = In.vTexUV + float2(TexelSize * -2.f, 0.f);
	Out.vTexUV4 = In.vTexUV + float2(TexelSize * -1.f, 0.f);
	Out.vTexUV5 = In.vTexUV + float2(TexelSize * -0.f, 0.f);
	Out.vTexUV6 = In.vTexUV + float2(TexelSize * -1.f, 0.f);
	Out.vTexUV7 = In.vTexUV + float2(TexelSize * -2.f, 0.f);
	Out.vTexUV8 = In.vTexUV + float2(TexelSize * -3.f, 0.f);
	Out.vTexUV9 = In.vTexUV + float2(TexelSize * -4.f, 0.f);

	return Out;
}
VS_OUT_BLUR VS_MAIN_VERTICAL(VS_IN In)
{
	VS_OUT_BLUR      Out = (VS_OUT_BLUR)0;

	matrix      matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float   TexelSize;

	TexelSize = 1.f / 720.f;

	Out.vTexUV1 = In.vTexUV + float2(0.f, TexelSize * -4.f);
	Out.vTexUV2 = In.vTexUV + float2(0.f, TexelSize * -3.f);
	Out.vTexUV3 = In.vTexUV + float2(0.f, TexelSize * -2.f);
	Out.vTexUV4 = In.vTexUV + float2(0.f, TexelSize * -1.f);
	Out.vTexUV5 = In.vTexUV + float2(0.f, TexelSize * -0.f);
	Out.vTexUV6 = In.vTexUV + float2(0.f, TexelSize * -1.f);
	Out.vTexUV7 = In.vTexUV + float2(0.f, TexelSize * -2.f);
	Out.vTexUV8 = In.vTexUV + float2(0.f, TexelSize * -3.f);
	Out.vTexUV9 = In.vTexUV + float2(0.f, TexelSize * -4.f);

	return Out;
}
//struct GS_IN_CUBE
//{
//   float4      vPosition : SV_POSITION;
//   float4      vLocalPosition:POSITION;
//
//};
//struct GS_OUT_CUBE
//{
//   float4      vPosition : SV_POSITION;
//   float4      vLocalPosition:POSITION;
//   float2      vTexUV : TEXCOORD0;
//   uint      RTIndex :SV_RenderTargetArrayIndex;
//};
//[maxvertexcount(18)]
//void GS_MAIN_CUBE(triangle VS_OUT_CUBE In[3], inout TriangleStream<GS_OUT_CUBE> CubeMapStream)
//{
//
//   for (uint i = 0; i < 6; ++i)
//   {
//
//      GS_OUT_CUBE      Out = (GS_OUT_CUBE)0;
//      Out.RTIndex = i;
//
//      for (uint j = 0; j < 3; ++j)
//      {
//
//         Out.vPosition = mul(In[j].vPosition, g_CubeViewMatrix[i]);
//         Out.vPosition = mul(Out.vPosition, g_CubeProjMatrix);
//         Out.vLocalPosition = In[j].vLocalPosition;
//
//         CubeMapStream.Append(Out);
//      }
//      CubeMapStream.RestartStrip();
//   }
//}

struct PS_IN
{
	float4      vPosition : SV_POSITION;
	float4      vLocalPosition:POSITION;
	float2      vTexUV : TEXCOORD0;
};
struct PS_IN_CUBE
{
	float4      vPosition : SV_POSITION;
	float4      vLocalPosition:POSITION;
};
struct PS_IN_BLUR
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
	float2      vTexUV1 : TEXCOORD1;
	float2      vTexUV2 : TEXCOORD2;
	float2      vTexUV3 : TEXCOORD3;
	float2      vTexUV4 : TEXCOORD4;
	float2      vTexUV5 : TEXCOORD5;
	float2      vTexUV6 : TEXCOORD6;
	float2      vTexUV7 : TEXCOORD7;
	float2      vTexUV8 : TEXCOORD8;
	float2      vTexUV9 : TEXCOORD9;
};
struct PS_OUT
{
	float4      vColor : SV_TARGET0;

};
struct PS_OUT_CUBE
{
	float4      vIrradiance : SV_TARGET0;

};
struct PS_OUT_LIGHT
{
	float4      vShade : SV_TARGET0;
	float4      vSpecular : SV_TARGET1;
};
struct PS_OUT_LIGHT_ROM
{
	float4      vNonIBL : SV_TARGET0;
	float4      vSpecular : SV_TARGET1;
};
struct PS_OUT_IBL
{
	float4      vIrradiance : SV_TARGET0;
	float4      vPreFilter : SV_TARGET1;
	float4      vBRDF_LUT : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	return Out;
}

PS_OUT PS_MAIN_3D(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	//float3 E = normalize(In.vPosition - g_vCamPosition);
	//float3 N = normalize(In.Normal);
	//float3 R = reflect(E, N);

	//Out.vColor = g_DiffuseTexture3D.Sample(LinearSampler, R);


	return Out;
}
PS_OUT_CUBE PS_MAIN_CUBE(PS_IN_CUBE In)
{
	PS_OUT_CUBE      Out = (PS_OUT_CUBE)0;

	float2 uv = SampleSphericalMap(normalize(In.vLocalPosition.xyz));
	float3 envColor = g_HDRTexture2D.Sample(LinearSampler, uv).xyz;

	//  HDR -> LDR(Tone map)
	envColor = envColor / (envColor + float3(1.0f, 1.0f, 1.0f));
	//  Gamma Correction
	envColor = pow(envColor, 1.0f / 2.2f);
	Out.vIrradiance = float4(envColor, 1.0f);
	//Out.vIrradiance = float4(0.f, 0.f, 0.f, 1.f);
	return Out;
}


PS_OUT PS_MAIN_BLUR_X(PS_IN_BLUR In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (0 == g_iBlurOption)
	{
		float4 color = 0;
		for (int i = 0; i < 13; ++i)
		{
			color += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, float2(
				In.vTexUV.x + (fPixelKernel[i] * (0.5f / g_fWinCX)) * 2 + 0.5f / g_fWinCX,
				In.vTexUV.y + (0.5f / g_fWinCY)))
				* fBlurWeights[i];
		}

		Out.vColor = color;
		Out.vColor.a = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV).a;
	}
	else if (1 == g_iBlurOption)
	{
		float fWeight0, fWeight1, fWeight2, fWeight3, fWeight4;
		float fNormalization;

		fWeight0 = 1.0f;
		fWeight1 = 0.9f;
		fWeight2 = 0.55f;
		fWeight3 = 0.18f;
		fWeight4 = 0.1f;

		fNormalization = (fWeight0 + 2.f * (fWeight1 + fWeight2 + fWeight3 + fWeight4));

		fWeight0 = fWeight0 / fNormalization;
		fWeight1 = fWeight1 / fNormalization;
		fWeight2 = fWeight2 / fNormalization;
		fWeight3 = fWeight3 / fNormalization;
		fWeight4 = fWeight4 / fNormalization;

		float4 vColor = float4(0.f, 0.f, 0.f, 0.f);


		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV1) * fWeight4;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV2) * fWeight3;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV3) * fWeight2;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV4) * fWeight1;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV5) * fWeight0;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV6) * fWeight1;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV7) * fWeight2;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV8) * fWeight3;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV9) * fWeight4;

		//float4 vFilter = g_FilterTexture.Sample(LinearWrapSampler, In.vTexUV4);


		//vColor.a = 1.f;
		Out.vColor = vColor;
		//Out.vColor = vColor + (vFilter * (1.f - vColor.a));
		Out.vColor.a = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV1).a;
	}
	else if (2 == g_iBlurOption)
	{
		float4 vOut = 0.f;

		float2 vTexUV = In.vTexUV;
		float2 vUV = 0.f;

		float fTu = 1.f / g_fWinCX;

		for (int i = -6; i < 6; ++i)
		{
			vUV = vTexUV + float2(fTu*i, 0.f);
			vOut += fBlurWeights_Test[6 + i] * saturate(g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, vUV));
		}

		vOut /= fTotalWeight_Test;

		Out.vColor = vOut;
	}
	//float4 vColor1 = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//vColor1 = pow(vColor1, 32);

	//float4 vColor2 = -0.84f;
	//float2 rcpres = { 0.0025, 0.0033333333333333333333333333333333 };


	//for (int i = 0; i < 13; ++i)
	//{
	//	vColor2 += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + (float2(fPixelKernel[i], 0.f)*rcpres))*fBlurWeights[i];
	//	vColor2 += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + (float2(0.f, fPixelKernel[i])*rcpres))*fBlurWeights[i];

	//}
	//vColor2 *= 0.48f;


	//Out.vColor = vColor1 + vColor2;
	return Out;
}
PS_OUT PS_MAIN_BLUR_Y(PS_IN_BLUR In)
{
	PS_OUT         Out = (PS_OUT)0;

	if (0 == g_iBlurOption)
	{
		float4 color = 0;
		for (int i = 0; i < 13; ++i)
		{
			color += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, float2(
				In.vTexUV.x + 0.5f / g_fWinCX,
				In.vTexUV.y + (fPixelKernel[i] * (0.5f / g_fWinCY) * 2) + (0.5f / g_fWinCY)))
				* fBlurWeights[i];
		}

		Out.vColor = color;
		Out.vColor.a = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV).a;
	}
	else if (1 == g_iBlurOption)
	{
		float fWeight0, fWeight1, fWeight2, fWeight3, fWeight4;
		float fNormalization;

		fWeight0 = 1.0f;
		fWeight1 = 0.9f;
		fWeight2 = 0.55f;
		fWeight3 = 0.18f;
		fWeight4 = 0.1f;

		fNormalization = (fWeight0 + 2.f * (fWeight1 + fWeight2 + fWeight3 + fWeight4));

		fWeight0 = fWeight0 / fNormalization;
		fWeight1 = fWeight1 / fNormalization;
		fWeight2 = fWeight2 / fNormalization;
		fWeight3 = fWeight3 / fNormalization;
		fWeight4 = fWeight4 / fNormalization;

		float4 vColor = float4(0.f, 0.f, 0.f, 0.f);


		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV1) * fWeight4;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV2) * fWeight3;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV3) * fWeight2;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV4) * fWeight1;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV5) * fWeight0;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV6) * fWeight1;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV7) * fWeight2;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV8) * fWeight3;
		vColor += g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV9) * fWeight4;

		//float4 vFilter = g_FilterTexture.Sample(LinearWrapSampler, In.vTexUV4);


		//vColor.a = 1.f;
		Out.vColor = vColor;
		//Out.vColor = vColor + (vFilter * (1.f - vColor.a));
		Out.vColor.a = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV1).a;
	}
	else if (2 == g_iBlurOption)
	{
		float4 vOut = 0.f;

		float2 vTexUV = In.vTexUV;
		float2 vUV = 0.f;

		float fTu = 1.f / g_fWinCY;

		for (int i = -6; i < 6; ++i)
		{
			vUV = vTexUV + float2(0.f, fTu*i);
			vOut += fBlurWeights_Test[6 + i] * saturate(g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, vUV));
		}

		vOut /= fTotalWeight_Test;

		Out.vColor = vOut;
	}
	//float4 vColor1 = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//vColor1 = pow(vColor1, 32);

	//float4 vColor2 = -0.84f;
	//float2 rcpres = { 0.0025, 0.0033333333333333333333333333333333 };


	//for (int i = 0; i < 13; ++i)
	//{
	//	vColor2 += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + (float2(fPixelKernel[i], 0.f)*rcpres))*fBlurWeights[i];
	//	vColor2 += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + (float2(0.f, fPixelKernel[i])*rcpres))*fBlurWeights[i];

	//}
	//vColor2 *= 0.48f;


	//Out.vColor = vColor1 + vColor2;
	return Out;
}

PS_OUT PS_MAIN_MOTIONBLUR(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	int iNumBlurSample = 100;

	float4 vVelocity = g_VelocityTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);
	float4 vDepth = g_DepthTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);

	vVelocity.xy /= (float)iNumBlurSample;

	int iCnt = 1;

	float4 BlurColor = float4(0.f, 0.f, 0.f, 0.f);

	float4 vOut = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);

	float fAlpha = vOut.a;
	for (int i = iCnt; i < iNumBlurSample; ++i)
	{
		BlurColor = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV + vVelocity.xy*(float)i);


		if (vVelocity.z < BlurColor.a + 0.04f)
		{
			iCnt++;
			vOut += BlurColor;
		}

	}
	vOut /= (float)iCnt;

	Out.vColor = vOut;
	Out.vColor.a = fAlpha;
	return Out;
}

PS_OUT PS_MAIN_BRIGHTNESS(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	//float4 vBrightColor = float4(0.f, 0.f, 0.f, 0.f);
	//float4 vFragColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//float fBrightness = dot(vFragColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	//if (fBrightness > 0.99f)
	//	vBrightColor = float4(vFragColor.rgb, 1.f);

	//if (fBrightness > 1.5f && fBrightness < 6.f)
	//	vBrightColor = float4(vFragColor.rgb, 1.f);
	//else
	//	vBrightColor = float4(0.f, 0.f, 0.f, 1.f);

	float4 vBrightColor = float4(0.f, 0.f, 0.f, 0.f);
	float4 vFragColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vBrightColor.xyz = pow(vFragColor.xyz, 4);
	vBrightColor.xyz *= 30;


	Out.vColor.xyz = vBrightColor.xyz;
	Out.vColor.a = vFragColor.a;
	return Out;
}

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	//float4 vHDRColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//float4 vBloomColor = g_TotalBlurTexture.Sample(LinearSampler, In.vTexUV);
	//float4 vBloomOriTex = g_BrightnessTexture.Sample(LinearSampler, In.vTexUV);

	//float4 vBloom = vBloomColor + vBloomOriTex;

	////float4 vBloom =vBloomColor+vBloomOriTex;
	//Out.vColor = vHDRColor + vBloom;
	//Out.vColor.a = vHDRColor.a;


	//float4 vHDRColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//float4 vBloomColor = g_TotalBlurTexture.Sample(LinearSampler, In.vTexUV);
	//float4 vBloomOriTex = g_BrightnessTexture.Sample(LinearSampler, In.vTexUV);

	//float4 vBloom = pow(pow(abs(vBloomColor), 2.2f) + pow(abs(vBloomOriTex), 2.2f), 1.f / 2.2f);

	//vHDRColor = pow(abs(vHDRColor), 2.2f);
	//vBloom = pow(abs(vBloom), 2.2f);

	////float4 vBloom =vBloomColor+vBloomOriTex;
	//Out.vColor = vHDRColor + vBloom;
	//Out.vColor = pow(abs(Out.vColor), 1.f / 2.2f);

	float4 vHDRColor = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);
	float4 vBloomColor = g_BloomColorTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);
	float4 vBlurColor2 = g_BlurColor2Texture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);
	float4 vBlurColor4 = g_BlurColor4Texture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);



	Out.vColor = vHDRColor*g_fDiffuseWeight + vBloomColor*g_fBloomColorWeight + vBlurColor2*g_fBlurColor2Weight + vBlurColor4*g_fBlurColor4Weight;
	Out.vColor *= 1.5f;
	Out.vColor.w = vHDRColor.a;


	return Out;
}

PS_OUT PS_MAIN_TONEMAPPING(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	float4 vHDRColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = vHDRColor.a;
	bool		g_bAcesOld = false;
	bool		g_bHableOld = false;
	bool		g_bAcesNew = false;
	bool		g_bHableNew = false;
	bool		g_bReinhard_extended_luminance = false;

	if (0 == g_iToneOption)
		Out.vColor.xyz = Aces_fitted(vHDRColor.xyz);

	else if (3 == g_iToneOption)
		Out.vColor.xyz = HableToneMapping(vHDRColor.xyz);

	else if (2 == g_iToneOption)
		Out.vColor.xyz = ACESFilmicToneMapping(vHDRColor.xyz);

	else if (1 == g_iToneOption)
		Out.vColor.xyz = Uncharted2_filmic(vHDRColor.xyz);

	else if (4 == g_iToneOption)
		Out.vColor.xyz = Reinhard_Extended_Luminance(vHDRColor.xyz, 1.f);

	Out.vColor = pow(Out.vColor, 1 / 2.2f);

	return Out;

}

PS_OUT PS_MAIN_DEPTHBLUR(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	float fDepthBlur = 0.f;
	float fDepth = g_DepthTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV).x;
	if (fDepth < g_vDofParam.y)
	{
		fDepthBlur = (fDepth - g_vDofParam.y) / (g_vDofParam.y - g_vDofParam.x);
	}
	else
	{
		fDepthBlur = (fDepth - g_vDofParam.y) / (g_vDofParam.z - g_vDofParam.y);
		fDepthBlur = clamp(fDepthBlur, 0.f, g_vDofParam.w);
	}

	fDepthBlur = fDepthBlur*0.5f + 0.5f;

	Out.vColor = float4(fDepthBlur.xxx, 1.f);
	return Out;

}

PS_OUT PS_MAIN_DOP(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);
	vector vBlur = g_TotalBlurTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV);
	float fDepth = g_DepthTexture.Sample(AnisotropicSamplerLinearClamp, In.vTexUV).x;
	float fAlpha = vDiffuse.a;

	Out.vColor = vDiffuse;
	Out.vColor = DOF(Out.vColor, g_TotalBlurTexture, AnisotropicSamplerLinearClamp, In.vTexUV, fDepth, g_vDofParam);
	Out.vColor.a = fAlpha;
	return Out;

}

PS_OUT PS_MAIN_HDR(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vRimlight = g_RimlightTexture.Sample(LinearSampler, In.vTexUV);

	float fGamma = 2.2f;
	Out.vColor = float4(vRimlight.rgb + vDiffuse.rgb, vDiffuse.a);

	Out.vColor.xyz = pow(abs(Out.vColor.xyz), fGamma);

	return Out;

}

PS_OUT PS_MAIN_TOTALDIFFUSE(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;


	vector vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexUV);
	//if (vEffect.r==0.f && vEffect.b == 0.f &&vEffect.g == 0.f )
	//	discard;
	Out.vColor = vEffect;


	return Out;

}

PS_OUT PS_MAIN_MSAA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	SamplerState MySample;
	if (0 == g_iSampler)
		MySample = LinearSamplerClamp;
	else if (1 == g_iSampler)
		MySample = LinearSampler;
	else if (2 == g_iSampler)
		MySample = AnisotropicSamplerLinear;
	else if (3 == g_iSampler)
		MySample = AnisotropicSamplerLinearClamp;
	else if (4 == g_iSampler)
		MySample = AnisotropicSamplerMirror;


	//float4 vColorCenter = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV);
	////<가장자리 찾기>

	///*현재 픽셀*/
	//float fLumaCenter = Luma(vColorCenter.rgb,g_iLuma);
	//
	//float fLumaDown = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(0, -1)).rgb,g_iLuma);
	//float fLumaUp = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(0, 1)).rgb, g_iLuma);
	//float fLumaLeft = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, 0)).rgb, g_iLuma);
	//float fLumaRight = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, 0)).rgb, g_iLuma);

	//float fLumaMin = min(fLumaCenter, min(min(fLumaDown, fLumaUp), min(fLumaLeft, fLumaRight)));
	//float fLumaMax = max(fLumaCenter, max(max(fLumaDown, fLumaUp), max(fLumaLeft, fLumaRight)));

	//float fLumaRange = fLumaMax - fLumaMin;

	//if (fLumaRange < max(g_fEdge_Threshold_Min, fLumaMax*g_fEdge_Threshold_Max))//픽셀의 대비를 비교하여 대비가 절대임계값, 최대 Luma에 비례하는 임계값보다 작으면 안함.
	//{
	//	Out.vColor = vColorCenter;
	//	//Out.vColor.rgb = float3(0.f, 1.f, 0.f);
	//	//Out.vColor = vColorCenter;
	//	return Out;
	//}

	////<가장자리가 수평인지 수직인지 판단하기>
	//float fLumaDownLeft = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, -1)).rgb, g_iLuma);
	//float fLumaUpRight = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, 1)).rgb, g_iLuma);
	//float fLumaUpLeft = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, 1)).rgb, g_iLuma);
	//float fLumaDownRight = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, -1)).rgb, g_iLuma);

	//float fLumaDownUp = fLumaDown + fLumaUp;
	//float fLumaLeftRight = fLumaLeft + fLumaRight;

	//float fLumaLeftCorners = fLumaDownLeft + fLumaUpLeft;
	//float fLumaDownCorners = fLumaDownLeft + fLumaDownRight;
	//float fLumaRightCorners = fLumaDownRight + fLumaUpRight;
	//float fLumaUpCorners = fLumaUpRight + fLumaUpLeft;

	//float fEdgeHorizontal = abs(-2.f * fLumaLeft + fLumaLeftCorners) + 2.f*(abs(-2.f * fLumaCenter + fLumaDownUp)) + abs(-2.f*fLumaRight + fLumaRightCorners);
	//float fEdgeVertical = abs(-2.f * fLumaUp + fLumaUpCorners) + 2.f*(abs(-2.f * fLumaCenter + fLumaLeftRight)) +abs(-2.f*fLumaDown + fLumaDownCorners);

	//bool IsHorizontal = (fEdgeHorizontal > fEdgeVertical);

	////<실제 가장자리의 방향이 어디인지 판정>
	//float fLuma1 = IsHorizontal ? fLumaDown : fLumaLeft;
	//float fLuma2 = IsHorizontal ? fLumaUp: fLumaRight;

	//float fGradient1 = fLuma1 - fLumaCenter;
	//float fGradient2 = fLuma2 - fLumaCenter;

	//bool IsSteepest = abs(fGradient1) >= abs(fGradient2);

	//float fGradientScaled = 0.25f*max(abs(fGradient1), abs(fGradient2));

	////<반픽셀 이동한 후, 점의 평균 Luma구하기>

	//float fStepLength = IsHorizontal ? g_vInverseScreenSize.y : g_vInverseScreenSize.x;

	//float fLumaLocalAverage = 0.f;

	//if (true == IsSteepest)
	//{
	//	fStepLength = -fStepLength;
	//	fLumaLocalAverage = (fLuma1 + fLumaCenter)*0.5f;
	//}
	//else
	//{
	//	fLumaLocalAverage = (fLuma2 + fLumaCenter)*0.5f;
	//}

	//float2 vCurrentUV = In.vTexUV;
	//if (true == IsHorizontal)
	//	vCurrentUV.y += fStepLength*0.5f;
	//else
	//{
	//	vCurrentUV.x += fStepLength*0.5f;
	//}

	////<가장자리를 따라서 어디가 끝인지 탐색함>

	//float2 vOffset = IsHorizontal ? float2(g_vInverseScreenSize.x, 0.f) : float2(0.f, g_vInverseScreenSize.y);

	//float2 vUV1 = vCurrentUV - vOffset;
	//float2 vUV2 = vCurrentUV + vOffset;

	//float fLumaEnd1 = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, vUV1).rgb, g_iLuma);
	//float fLumaEnd2 = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, vUV2).rgb, g_iLuma);

	//fLumaEnd1 -= fLumaLocalAverage;
	//fLumaEnd2 -= fLumaLocalAverage;


	//bool Reached1 = abs(fLumaEnd1) >= fGradientScaled;//현재 끝점에서 Luma변화값이 로컬변화도 보다 크면 가장자리에 다다른것
	//bool Reached2 = abs(fLumaEnd2) >= fGradientScaled;
	//bool ReachedBoth = Reached1 && Reached2;

	//if (!Reached1) //못 도달했으면 계속 uv 옮겨주면서 탐색
	//	vUV1 -= vOffset;
	//if (!Reached2)
	//	vUV2 += vOffset;

	////<양쪽 끝에 도달할 때까지 계속 탐색>

	//float fQuality[12] = {1.f,1.f,1.f,1.f,1.f,1.5f,2.f,2.f,2.f,2.f,4.f,8.f };//for문 5번 이후에 , 점점 큰 폭으로 uv를 옮겨서 탐색하겠다.
	//
	//if (false == ReachedBoth)
	//{
	//	for (uint i = 2; i < 12; ++i)
	//	{
	//		if (!Reached1)
	//		{
	//			fLumaEnd1 = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, vUV1).rgb, g_iLuma);
	//			fLumaEnd1 -= fLumaLocalAverage;
	//		}
	//		if (!Reached2)
	//		{
	//			fLumaEnd2 = Luma(g_DiffuseTexture.Sample(LinearSamplerClamp, vUV2).rgb, g_iLuma);
	//			fLumaEnd2 -= fLumaLocalAverage;
	//		}


	//		Reached1 = abs(fLumaEnd1) >= fGradientScaled;
	//		Reached2 = abs(fLumaEnd2) >= fGradientScaled;
	//		ReachedBoth = Reached1 && Reached2;

	//		if (!Reached1)
	//		{
	//			vUV1 -= vOffset*fQuality[i];
	//		}
	//		if (!Reached2)
	//		{
	//			vUV2 += vOffset*fQuality[i];
	//		}

	//		if (true == ReachedBoth)
	//			break;
	//	}
	//}

	////픽셀의 offset 구함. 현재픽셀에서 가장자리의 두 끝점중 가까운 곳을 찾고 픽셀이 끝점에 더 가까울수록 offset은 커진다.

	//float fDistance1 = IsHorizontal ? (In.vTexUV.x - vUV1.x) : (In.vTexUV.y - vUV1.y);
	//float fDistance2 = IsHorizontal ? (vUV2.x - In.vTexUV.x) : (vUV2.y - In.vTexUV.y);

	//bool IsDirection = fDistance1 < fDistance2;
	//float fDistanceFinal = min(fDistance1, fDistance2);

	//float fEdgeThickness = fDistance1 + fDistance2;

	//float fPixelOffset = -fDistanceFinal / fEdgeThickness + 0.5f;

	////현재 픽셀의 luma와 끝점의 luma의 차이가 일관된지 검사. 안하면 너무 멀리 이동했을 수도있다. 
	//bool IsLumaCenterSmaller = fLumaCenter < fLumaLocalAverage;

	//bool CorrectVariation = ((IsDirection ? fLumaEnd1 : fLumaEnd2) < 0.f) != IsLumaCenterSmaller;
	//
	//float fFinalOffset = CorrectVariation ? fPixelOffset : 0.f;

	////서브픽셀 offset 구함
	//float fLumaAverage = (1.f / 12.f) * (2.f * (fLumaDownUp + fLumaLeftRight) + fLumaLeftCorners + fLumaRightCorners);

	//float fSubPixelOffset1 = clamp(abs(fLumaAverage - fLumaCenter) / fLumaRange, 0.f, 1.f);
	//float fSubPixelOffset2 = (-2.f*fSubPixelOffset1 + 3.f)*fSubPixelOffset1*fSubPixelOffset1;

	//float fSubPixelOffsetFinal = fSubPixelOffset2*fSubPixelOffset2* g_fSubPixel_Quality;

	//fFinalOffset = max(fFinalOffset, fSubPixelOffsetFinal);

	////최종 UV좌표 계산

	//float2 vFinalUV = In.vTexUV;
	//if (IsHorizontal)
	//{
	//	vFinalUV.y += fFinalOffset*fStepLength;
	//}
	//else
	//{
	//	vFinalUV.x += fFinalOffset*fStepLength;

	//}

	//float4 vFinalColor = g_DiffuseTexture.Sample(LinearSamplerClamp, vFinalUV);
	//Out.vColor = vFinalColor;
	////Out.vColor.r += 0.5f;
	if (0 == g_iSampler)
	{
		float4 colorCenter = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV);
		float lumaCenter = rgb2luma(colorCenter.rgb);


		// Luma at the four direct neighbours of the current fragment.
		float lumaDown = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(0, -1)).rgb);
		float lumaUp = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(0, 1)).rgb);
		float lumaLeft = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, 0)).rgb);
		float lumaRight = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, 0)).rgb);


		// Find the maximum and minimum luma around the current fragment.
		float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
		float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

		// delta 계산
		float lumaRange = lumaMax - lumaMin;

		// 만약 luma 차이가 임계값보다 작다면(또는 우리가 정말 어두운 영역내에 있다면), 
		// 우리는 가장자리에 있지 않는 것이므로, AA를 수행하지 않는다.
		if (lumaRange < max(g_fEdge_Threshold_Min, lumaMax*g_fEdge_Threshold_Max))//픽셀의 대비를 비교하여 대비가 절대임계값, 최대 Luma에 비례하는 임계값보다 작으면 안함.
		{
			Out.vColor = colorCenter;
			Out.vColor.rgb = 0.f;
			//return Out;
		}
		else
		{
			const float dx = (1.f / 1280.f) * g_fMsaaOffset;
			const float dy = (1.f / 720.f)* g_fMsaaOffset;

			const float2 offsets[9] =
			{
				float2(-dx,-dy),float2(0.f,-dy),float2(dx,-dy),
				float2(-dx,0.f),float2(0.f,0.f),float2(dx,0.f),
				float2(-dx,dy),float2(0.f,dy),float2(dx,dy),
			};
			//float2 offsets[25] =
			//{
			//	float2(-2 * dx, -2 * dx), float2(-1 * dx, -2 * dx), float2(0 * dx, -2 * dx), float2(1 * dx, -2 * dx), float2(2 * dx, -2 * dx),
			//	float2(-2 * dx, -1 * dx), float2(-1 * dx, -1 * dx), float2(0 * dx, -1 * dx), float2(1 * dx, -1 * dx), float2(2 * dx, -1 * dx),
			//	float2(-2 * dx, 0 * dx), float2(-1 * dx, 0 * dx), float2(0 * dx, 0 * dx), float2(1 * dx, 0 * dx), float2(2 * dx, 0 * dx),
			//	float2(-2 * dx, 1 * dx), float2(-1 * dx, 1 * dx), float2(0 * dx, 1 * dx), float2(1 * dx, 1 * dx), float2(2 * dx, 1 * dx),
			//	float2(-2 * dx, 2 * dx), float2(-1 * dx, 2 * dx), float2(0 * dx, 2 * dx), float2(1 * dx, 2 * dx), float2(2 * dx, 2 * dx),
			//};
			float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
			for (int i = 0; i < 9; ++i)
			{
				vColor.rgb += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offsets[i]).rgb;
			}

			vColor /= 9.f;
			vColor.a = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV).a;
			Out.vColor = vColor;
			//return Out;
		}
		// 남은 4개의 코너 lumas 를 얻음

		float lumaDownLeft = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, -1)).rgb);
		float lumaUpRight = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, 1)).rgb);
		float lumaUpLeft = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, 1)).rgb);
		float lumaDownRight = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, -1)).rgb);

		float lumaDownUp = lumaDown + lumaUp;
		float lumaLeftRight = lumaLeft + lumaRight;

		// 코너도 동일
		float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
		float lumaDownCorners = lumaDownLeft + lumaDownRight;
		float lumaRightCorners = lumaDownRight + lumaUpRight;
		float lumaUpCorners = lumaUpRight + lumaUpLeft;

		// 수평과 수직 축을 따르는 변화도의 추정치 계산
		float edgeHorizontal = abs(-2.0 * lumaLeft + lumaLeftCorners) + abs(-2.0 * lumaCenter + lumaDownUp) * 2.0 + abs(-2.0 * lumaRight + lumaRightCorners);
		float edgeVertical = abs(-2.0 * lumaUp + lumaUpCorners) + abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0 + abs(-2.0 * lumaDown + lumaDownCorners);

		// 로컬 가장자리가 수평인가? 수직인가?
		bool isHorizontal = (edgeHorizontal >= edgeVertical);

		// 로컬 가장자리의 반대방향으로 2개의 이웃 텍셀을 선택함
		float luma1 = isHorizontal ? lumaDown : lumaLeft;
		float luma2 = isHorizontal ? lumaUp : lumaRight;

		// 이 방향으로 변화도를 계산
		float gradient1 = luma1 - lumaCenter;
		float gradient2 = luma2 - lumaCenter;

		// 어느 방향이 변화가 더 가파른가?
		bool is1Steepest = abs(gradient1) >= abs(gradient2);

		// 해당 방향의 변화도, 정규화
		float gradientScaled = 0.25*max(abs(gradient1), abs(gradient2));

		float stepLength = isHorizontal ? g_vInverseScreenSize.y : g_vInverseScreenSize.x;

		//float			g_fMsaaOffset = 0.125f;
		//float			g_fEdge_Threshold_Min = 0.0312f;
		//float			g_fEdge_Threshold_Max = 0.125f;
		//float2			g_vInverseScreenSize;
		//uint			g_iMaxRepeat = 12;
		//float			g_fSubPixel_Quality = 0.75f;
		//uint			g_iSampler = 0;
		//bool			g_iLuma = false;
		// 올바른 방향의 평균 luma
		float lumaLocalAverage = 0.0;

		if (is1Steepest) {
			// 방향을 바꿈
			stepLength = -stepLength;
			lumaLocalAverage = 0.5*(luma1 + lumaCenter);
		}
		else {
			lumaLocalAverage = 0.5*(luma2 + lumaCenter);
		}

		// UV 를 올바른 방향으로 반픽셀 이동시킴
		float2 currentUv = In.vTexUV;
		if (isHorizontal) {
			currentUv.y += stepLength * 0.5;
		}
		else {
			currentUv.x += stepLength * 0.5;
		}

		// 오른쪽 방향으로 offset (각 반복 스탭에 대해) 을 계산.
		float2 offset = isHorizontal ? float2(g_vInverseScreenSize.x, 0.0) : float2(0.0, g_vInverseScreenSize.y);

		// 가장자리의 각 측면을 직교하여 탐색하기 위해서 UV를 계산. QUALITY 는 스탭을 더 빠르게 함.
		float2 uv1 = currentUv - offset;
		float2 uv2 = currentUv + offset;

		// 탐색하는 세그먼트의 양쪽 끝에서 lumas를 읽고, delta 를 계산하고 로컬 평균 luma에 기록

		float lumaEnd1 = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, uv1).rgb);
		float lumaEnd2 = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, uv2).rgb);

		lumaEnd1 -= lumaLocalAverage;
		lumaEnd2 -= lumaLocalAverage;

		// 현재 끝점에서 luma delta가 로컬 변화도 보다 크면, 우리는 가장자리의 측면에 도달한 것임
		bool reached1 = abs(lumaEnd1) >= gradientScaled;
		bool reached2 = abs(lumaEnd2) >= gradientScaled;
		bool reachedBoth = reached1 && reached2;

		// 측면에 도달하지 못했다면, 우리는 계속해서 이 방향으로 탐색함.
		if (!reached1) {
			uv1 -= offset;
		}
		if (!reached2) {
			uv2 += offset;
		}

		// 만약 양 방향 모두 측면에 도달하지 않았다면, 계속해서 탐색
		float fQuality[12] = { 1.f,1.f,1.f,1.f,1.f,1.5f,2.f,2.f,2.f,2.f,4.f,8.f };//for문 5번 이후에 , 점점 큰 폭으로 uv를 옮겨서 탐색하겠다.

		if (!reachedBoth) {
			for (int i = 2; i < 12; i++) {
				// 필요하다면, 첫번재 방향의 luma를 읽음, delta 계산
				if (!reached1) {
					lumaEnd1 = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, uv1).rgb);
					lumaEnd1 = lumaEnd1 - lumaLocalAverage;
				}
				// 필요하다면, 반대 방향의 luma를 읽음, delta 계산
				if (!reached2) {
					lumaEnd2 = rgb2luma(g_DiffuseTexture.Sample(LinearSamplerClamp, uv2).rgb);
					lumaEnd2 = lumaEnd2 - lumaLocalAverage;
				}
				// 만약 현재 끝점의 luma delta가 로컬 변화도 보다 크다면, 우리는 가장자리 측면에 도달한 것임
				reached1 = abs(lumaEnd1) >= gradientScaled;
				reached2 = abs(lumaEnd2) >= gradientScaled;
				reachedBoth = reached1 && reached2;

				// 만약 측면에 도달하지 않았다면, 우리는 이 방향으로 계속 탐색, 
				// 가변 품질로 진행 (역주 : 스탭을 반복에 따라 조정하겠다는 의미)
				if (!reached1) {
					uv1 -= offset * fQuality[i];
				}
				if (!reached2) {
					uv2 += offset * fQuality[i];
				}

				// 두 측면에 도착했다면, 탐색을 중단
				if (reachedBoth) { break; }
			}
		}

		// 양 끝 가장자리까지의 거리를 계산
		float distance1 = isHorizontal ? (In.vTexUV.x - uv1.x) : (In.vTexUV.y - uv1.y);
		float distance2 = isHorizontal ? (uv2.x - In.vTexUV.x) : (uv2.y - In.vTexUV.y);

		// 어떤 방향의 가장자리의 끝이 더 가깝나?
		bool isDirection1 = distance1 < distance2;
		float distanceFinal = min(distance1, distance2);

		// 가장자리의 길이
		float edgeThickness = (distance1 + distance2);

		// UV offset: 가장자리의 측면까지 가장 가까운 방향으로 읽음
		float pixelOffset = -distanceFinal / edgeThickness + 0.5;

		// 중앙 luma가 로별 평균보다 더 작나?
		bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

		// 만약 중앙 luma가 그것의 이웃보다 더 작다면, 양 끝의 delta luma가 양수여야 합니다. (같은 변형)
		// (가장자리의 측면과 더 가까운 방향으로)
		bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

		// 만약 luma 차이가 잘못된 경우, offset 을 적용하지 않음
		float finalOffset = correctVariation ? pixelOffset : 0.0;

		// Sub-pixel shifting
		// 3x3 이웃에 대한 luma의 전체 가중 평균
		float lumaAverage = (1.0 / 12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

		// 3x3 이웃의 루마 범위에 대한, 전역 평균과 중앙 luma 간의 delta의 비율
		float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0, 1.0);
		float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;

		// 이 delta에 기반한 sub-pixel의 offset 계산
		float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * g_fSubPixel_Quality;

		// 두 offset 중 더 큰것을 고름
		finalOffset = max(finalOffset, subPixelOffsetFinal);

		// 최종 UV 좌표 계산



		float2 finalUv = In.vTexUV;
		if (isHorizontal)
		{
			finalUv.y += finalOffset * stepLength;
		}
		else
		{
			finalUv.x += finalOffset * stepLength;
		}

		//새로운 UV 좌표에서 컬러를 읽고 사용함

		float4 finalColor = g_DiffuseTexture.Sample(LinearSamplerClamp, finalUv);
		Out.vColor = finalColor;


		//float4 finalColor = float4(0.f, 0.f, 0.f, 0.f);

		//float3 vUp = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(0, 1)).rgb;
		//float3 vDown = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(0, -1)).rgb;
		//float3 vLeft = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(-1, 0)).rgb;
		//float3 vRight = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexUV, int2(1, 0)).rgb;
		//float3 vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV).rgb;

		//float3 vUp = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + float2(0.f, g_vInverseScreenSize.y)).rgb;
		//float3 vDown = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + float2(0.f, -g_vInverseScreenSize.y)).rgb;
		//float3 vLeft = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + float2(-g_vInverseScreenSize.x, 0.f)).rgb;
		//float3 vRight = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + float2(g_vInverseScreenSize.x, 0.f)).rgb;
		//float2 finalUv = In.vTexUV;

		//float fDecline = 0.f;
		//float fConstant = 0.f;
		//float fX = 0.f;
		//float fRatio = 0.f;
		//if (isHorizontal)
		//{
		//	if (finalOffset > 0)
		//	{
		//		fDecline = g_vInverseScreenSize.y;
		//		fConstant = -1.f*fDecline*In.vTexUV.y;
		//		fX = In.vTexUV.y + finalOffset * stepLength;
		//		fRatio = fDecline*fX + fConstant;
		//		finalColor.rgb = lerp(vColor, vUp, finalOffset);
		//		//finalColor.rgb = (vUp + vDown + vRight + vLeft);

		//		//if(vUp.r == 0.f)
		//		//	finalColor.r = 1.f;

		//	}
		//	else
		//	{
		//		fDecline = -g_vInverseScreenSize.y;
		//		fConstant = -1.f*fDecline*In.vTexUV.y;
		//		fX = In.vTexUV.y + finalOffset * stepLength;
		//		fRatio = fDecline*fX + fConstant;

		//		finalColor.rgb = lerp(vColor, vDown, -finalOffset);
		//		//finalColor.g = 1.f;

		//	}
		//	finalUv.y += finalOffset * stepLength;
		//}
		//else
		//{
		//	if (finalOffset > 0)
		//	{
		//		fDecline = g_vInverseScreenSize.x;
		//		fConstant = -1.f*fDecline*In.vTexUV.x;
		//		fX = In.vTexUV.x + finalOffset * stepLength;
		//		fRatio = fDecline*fX + fConstant;
		//		finalColor.rgb = lerp(vColor, vRight, finalOffset);
		//		//finalColor.rgb = (vUp + vDown + vRight + vLeft);
		//		//if(vRight.r ==0.f)
		//		//finalColor.b = 1.f;
		//	}
		//	else
		//	{
		//		fDecline = -g_vInverseScreenSize.x;
		//		fConstant = -1.f*fDecline*In.vTexUV.x;
		//		fX = In.vTexUV.x + finalOffset * stepLength;
		//		fRatio = fDecline*fX + fConstant;
		//		finalColor.rgb = lerp(vColor, vLeft, -finalOffset);
		//		//finalColor.rg = 1.f;

		//	}
		//	finalUv.x += finalOffset * stepLength;
		//}

		////if (vRight.r <= 0.f || (vLeft.r <= 0.f) || (vUp.r <= 0.f) || (vDown.r <= 0.f))
		////	Out.vColor.r = 1.f;
		//Out.vColor.a = g_DiffuseTexture.Sample(LinearSamplerClamp, finalUv).a;
		//Out.vColor.rgb = finalColor.rgb;

		//if ((vRight.r <= 0.1f &&vRight.g <= 0.1f &&vRight.b <= 0.1f)||
		//	(vUp.r <= 0.1f &&vUp.g <= 0.1f &&vUp.b <= 0.1f) ||
		//	(vDown.r <= 0.1f &&vDown.g <= 0.1f &&vDown.b <= 0.1f) ||
		//	(vLeft.r <= 0.1f &&vLeft.g <= 0.1f &&vLeft.b <= 0.1f))
		//	Out.vColor.r = 1.f;
		//Out.vColor.a = 1.f;

	}
	else if (1 == g_iSampler)
	{
		Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	}
	else if (2 == g_iSampler)
	{
		const float dx = (1.f / 1280.f) * g_fMsaaOffset;
		const float dy = (1.f / 720.f)* g_fMsaaOffset;

		const float2 offsets[9] =
		{
			float2(-dx,-dy),float2(0.f,-dy),float2(dx,-dy),
			float2(-dx,0.f),float2(0.f,0.f),float2(dx,0.f),
			float2(-dx,dy),float2(0.f,dy),float2(dx,dy),
		};
		//float2 offsets[25] =
		//{
		//	float2(-2 * dx, -2 * dx), float2(-1 * dx, -2 * dx), float2(0 * dx, -2 * dx), float2(1 * dx, -2 * dx), float2(2 * dx, -2 * dx),
		//	float2(-2 * dx, -1 * dx), float2(-1 * dx, -1 * dx), float2(0 * dx, -1 * dx), float2(1 * dx, -1 * dx), float2(2 * dx, -1 * dx),
		//	float2(-2 * dx, 0 * dx), float2(-1 * dx, 0 * dx), float2(0 * dx, 0 * dx), float2(1 * dx, 0 * dx), float2(2 * dx, 0 * dx),
		//	float2(-2 * dx, 1 * dx), float2(-1 * dx, 1 * dx), float2(0 * dx, 1 * dx), float2(1 * dx, 1 * dx), float2(2 * dx, 1 * dx),
		//	float2(-2 * dx, 2 * dx), float2(-1 * dx, 2 * dx), float2(0 * dx, 2 * dx), float2(1 * dx, 2 * dx), float2(2 * dx, 2 * dx),
		//};
		float4 vColor1 = float4(0.f, 0.f, 0.f, 0.f);
		for (uint i = 0; i < 9; ++i)
			vColor1 += g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offsets[i]);

		vColor1 /= 9.f;
		vColor1.a = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV).a;
		Out.vColor = vColor1;
	}

	return Out;
}
PS_OUT PS_MAIN_LUT(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;


	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vColor = float4(GetLutColor(vColor.rgb, g_LutTexture, LinearSampler), vColor.a);

	Out.vColor = vColor;
	return Out;

}
PS_OUT PS_MAIN_FOG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float      fViewZ = vDepthDesc.y * 300.f;

	vector      vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.r;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	float fFog = 0.0f;

	if (g_fFogType == 0)
	{
		fFog = ExponentialFog(vPosition, g_vCamPosition, g_fFogstart, g_fFogDenstiy);
	}
	else if (g_fFogType == 1)
	{
		fFog = ExponentialHeightFog(vPosition, g_vCamPosition, g_fFogstart, g_fFogDenstiy, g_fFogfalloff);
	}

	Out.vColor = vDiffuse;
	//Out.vColor.rgb += lerp(Out.vColor, g_fFogcolor, fFog).rgb;
	Out.vColor.rgb = lerp(Out.vColor, g_fFogcolor, fFog).rgb;

	//Out.vColor = 1.f - Out.vColor;
	return Out;
}
PS_OUT PS_MAIN_RADIAL(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;


	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//if (vEffect.r==0.f && vEffect.b == 0.f &&vEffect.g == 0.f )
	//	discard;

	Out.vColor.rgb = Radialblur(g_DiffuseTexture, LinearSampler, In.vTexUV, g_iRadialCnt, g_iRadialBlurPower, g_vRadialDir);
	Out.vColor.a = vColor.a;



	return Out;

}
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT   Out = (PS_OUT_LIGHT)0;

	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float      fViewZ = vDepthDesc.y * 300.f;


	Out.vShade = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 1.f;

	vector      vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.r;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	vector      vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector      vLook = vPosition - g_vCamPosition;


	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.0f);
	Out.vSpecular.a = 0.f;

	return Out;
}
PS_OUT_LIGHT_ROM PS_MAIN_DIRECTIONAL_ROM(PS_IN In)
{
	PS_OUT_LIGHT_ROM   Out = (PS_OUT_LIGHT_ROM)0;
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector vDiffuseBRDF = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float fRoughness = g_RomTexture.Sample(LinearSampler, In.vTexUV).x;
	//if (0.1f >= fRoughness)
	//	fRoughness = 0.1f;
	float fMetalness = g_RomTexture.Sample(LinearSampler, In.vTexUV).z;

	vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float      fViewZ = vDepthDesc.y * 300.f;


	vector      vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.r;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	//Outline
	vector      vLightDir = vPosition - g_vLightPos;
	float      fDistance = length(vLightDir);
	float      fDot = max(0, dot(normalize(vNormal), normalize(vLightDir)));
	vector      vOutLine = g_OutlineTexture.Sample(LinearSampler, In.vTexUV);

	if (vOutLine.r == 0.f)
	{
		fDot = 0.f;
		vOutLine.r = 1.f;
	}

	float4 vShade = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal))/**(fDot + vOutLine.r)*/ + g_fShadeAmbient /*(g_vLightAmbient * g_vMtrlAmbient)*/;
	vShade.a = 1.f;
	float3 vLo = normalize(g_vCamPosition.xyz - vPosition.xyz);
	float fCosLo = saturate(dot(normalize(vNormal.xyz), vLo));

	float3 vF0 = 0.04f;
	vF0 = Fresnel_Reflectance(vF0, vDiffuseBRDF.xyz, fMetalness);

	//Lightning
	float3 vLightInverse = -g_vLightDir.xyz;
	float3 vLh = normalize((normalize(vLightInverse) + normalize(vLo)));

	float fCosLi = saturate(dot(normalize(vNormal.xyz), normalize(vLightInverse)));
	float fCosLh = saturate(dot(normalize(vNormal.xyz), normalize(vLh)));
	//if (0.5f > fCosLi)
	//	fCosLi = 0.5f;


	//F : Relate To Mettal
	float3      vF = Fresnel(vLh, vLo, vF0);

	//D : Relate To Roughness
	float      fD = NormalDistributionGGX(normalize(vNormal.xyz), vLh, fRoughness);

	//G : Relate TO Roughness
	float      fG = Geometry(normalize(vNormal.xyz), vLo, vLightInverse, fRoughness);


	//DiffuseBRDF
	float3 vAmbient = float3(0.f, 0.f, 0.f);
	if (0.9f <= fMetalness)
	{
		vAmbient = float3(0.6f, 0.6f, 0.6f);
		fMetalness = 0.f;
	}

	float3 vKd = lerp(float3(1.f, 1.f, 1.f) - vF, float3(0.0f, 0.0f, 0.0f), fMetalness) + vAmbient;

	const float3 kS = vF;
	const float3 kD = (float3(1, 1, 1) - kS) * saturate(1.0f - fMetalness + vAmbient) * vDiffuseBRDF.xyz;
	const float3 Id = Fresnel_LambertDiffuse(kD);

	vDiffuseBRDF.xyz = kD;
	float4 vLightRadiance = float4(0.4f, 0.4f, 0.4f, 1.f);


	//SpecularBRDF
	Out.vSpecular.rgb = (vF*fD*fG) / max(0.001f, 4.f*fCosLi*fCosLo);
	Out.vSpecular.a = 0.f;

	//Shadow

	float percentLit = 0.f;

	if (true == g_bShadow)
	{
		vector vShadowDepth = g_DepthShadowTexture.Sample(LinearSampler, In.vTexUV);
		float3 shadow = float3(1.f, 1.f, 1.f);

		vShadowDepth.xyz /= vShadowDepth.w;
		//shadow = CalcShadowFactor(CascadedPcfShadowMapSampler, g_ShadowTexture, vShadowDepth, vPosition.z);
		float2 uv = vShadowDepth.xy/* / vShadowDepth.w*/;
		uv.y = -uv.y;
		uv = uv*0.5f + 0.5f;
		//vShadowDepth = g_ShadowTexture.Sample(LinearSamplerClamp, uv);


		float depth = vShadowDepth.z /*/ vShadowDepth.w*/;

		const float dx = 1.f / 1280.f;
		const float dy = 1.f / 720.f;

		const float2 offsets[9] =
		{
			float2(-dx,-dy),float2(0.f,-dy),float2(dx,-dy),
			float2(-dx,0.f),float2(0.f,0.f),float2(dx,0.f),
			float2(-dx,dy),float2(0.f,dy),float2(dx,+dy),
		};
		float fBias = 0.000125f;

		for (uint i = 0; i < 9; ++i)
		{
			float fOffset = g_ShadowMapTexture.Sample(LinearSamplerClamp, uv + offsets[i]).r;
			float fResult = 0.f;

			if (fOffset > depth - g_fShadowBias)
				fResult = 1.f;
			else
			{
				float3 vShadeTest = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal));
				//fResult =(1- ((vShadeTest.x + vShadeTest.y + vShadeTest.z) / 3.f))*(1 - ((vShadeTest.x + vShadeTest.y + vShadeTest.z) / 3.f));
				fResult = 0.f;

			}
			percentLit += fResult;
		}

		percentLit /= (9.f);

		//float range = length(float3(0.f, 0.f, 0.f) - vPosition.z);
		//if (range > 100)
		//   percentLit = 1.f;

		if (g_fPercentLimitMax > percentLit)
			percentLit = g_fPercentLimitMax;
		//if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
		//	vDiffuseBRDF.xyz *= percentLit;
		if (!(saturate(uv.x) == uv.x && saturate(uv.y) == uv.y))
			percentLit = 1.f;
	}
	else
	{
		percentLit = 1.f;
	}
	float fCosLi2 = saturate(dot(vNormal.xyz, normalize(vLightInverse)));
	//if (0.3f > fCosLi2)
	//	fCosLi2 = 0.3f;
	float fAO = g_RomTexture.Sample(LinearSampler, In.vTexUV).y;
	float fSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV).y;
	if (false == g_bAO)
		fAO = 1.f;
	if (false == g_bAO)
		fSSAO = 1.f;


	Out.vNonIBL.xyz = (vDiffuseBRDF.xyz*vShade.xyz + Out.vSpecular.xyz)*vLightRadiance.xyz*fSSAO*percentLit;
	Out.vNonIBL.w = vDiffuseBRDF.w;

	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	float fEmissive = smoothstep(1.f - vEmissive.a, 1.f, 1.f - fCosLi);

	vEmissive *= fEmissive;
	if (true == g_bEmissive)
		Out.vNonIBL.xyz += vEmissive.xyz;

	Out.vSpecular = Out.vNonIBL;
	//Out.vNonIBL = float4(fAO, fAO, fAO, 1.f);

	//float3 F = FresnelWithRoughness(fCosLo, vF0, fRoughness);



	//float3 irradiance = g_IrradianceTexture.Sample(LinearSampler, vNormal.xyz).rgb;
	//float3 diffuse = irradiance * vDiffuseBRDF.xyz;

	//float3 R = reflect(-vLo, vNormal.xyz);

	//float  Max_Reflection_Lod = 4.f;
	//float3 prefilteredColor = g_PreFilterTexture.SampleLevel(LinearSampler, R, fRoughness * MAX_REFLECTION_LOD).rgb;
	//float2 envBRDF = g_BRDF_LUTTexture.Sample(LinearSampler, float2(max(dot(vNormal.xyz, vLo), 0.0), fRoughness)).rg;
	//float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	//float3 KD = float3(1.f, 1.f, 1.f) - F;
	//float3 ambient = (vKd * diffuse + vAmbient /*+ specular*/);
	//Out.vShade = float4(ambient, 1.f);

	//float3 color = ambient/* + Out.vNonIBL.xyz*/;

	//color = color / (color + float3(1.0, 1.0, 1.0));
	//color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	//Out.vNonIBL += float4(color, 1.f);
	return Out;
}
PS_OUT_LIGHT_ROM PS_MAIN_SPOT_ROM(PS_IN In)
{
	PS_OUT_LIGHT_ROM   Out = (PS_OUT_LIGHT_ROM)0;
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector vDiffuseBRDF = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float fRoughness = g_RomTexture.Sample(LinearSampler, In.vTexUV).x;
	//if (0.1f >= fRoughness)
	//	fRoughness = 0.1f;
	//float fMetalness = g_RomTexture.Sample(LinearSampler, In.vTexUV).z;
	float fMetalness = 0.f;
	vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float      fViewZ = vDepthDesc.y * 300.f;


	vector      vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.r;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	//Outline
	//vector      vLightDir = vPosition - g_vLightPos;
	//float      fDistance = length(vLightDir);
	//float      fDot = max(0, dot(normalize(vNormal), normalize(vLightDir)));
	//vector      vOutLine = g_OutlineTexture.Sample(LinearSampler, In.vTexUV);

	//if (vOutLine.r == 0.f)
	//{
	//   fDot = 0.f;
	//   vOutLine.r = 1.f;
	//}
	vector      vLightDir = vPosition - g_vLightPos;


	float4 vShade;
	vShade = g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal))/**(fDot + vOutLine.r)*/ + g_fShadeAmbient /*(g_vLightAmbient * g_vMtrlAmbient)*/;
	vShade.a = 1.f;
	float3 vLo = normalize(g_vCamPosition.xyz - vPosition.xyz);
	float fCosLo = saturate(dot(normalize(vNormal.xyz), vLo));

	float3 vF0 = 0.04f;
	vF0 = Fresnel_Reflectance(vF0, vDiffuseBRDF.xyz, fMetalness);

	//Lightning
	float      fDistance = length(vLightDir);

	float      fAtt = saturate((g_fRange - fDistance) / g_fRange);

	float3 vLightInverse = -vLightDir.xyz;
	float3 vLh = normalize((normalize(vLightInverse) + normalize(vLo)));

	float fCosLi = saturate(dot(normalize(vNormal.xyz), normalize(vLightInverse)));
	float fCosLh = saturate(dot(normalize(vNormal.xyz), normalize(vLh)));

	//F : Relate To Mettal
	float3      vF = Fresnel(vLh, vLo, vF0);

	//D : Relate To Roughness
	float      fD = NormalDistributionGGX(normalize(vNormal.xyz), vLh, fRoughness);

	//G : Relate TO Roughness
	float      fG = Geometry(normalize(vNormal.xyz), vLo, vLightInverse, fRoughness);

	//DiffuseBRDF
	float3 vAmbient = float3(0.f, 0.f, 0.f);
	if (0.9f <= fMetalness)
		vAmbient = float3(0.25f, 0.25f, 0.25f);

	float3 vKd = lerp(float3(1.f, 1.f, 1.f) - vF, float3(0.0f, 0.0f, 0.0f), fMetalness) + vAmbient;
	const float3 kS = vF;
	const float3 kD = (float3(1, 1, 1) - kS) * saturate(1.0f - fMetalness + vAmbient) * vDiffuseBRDF.xyz;

	vDiffuseBRDF.xyz = kD;

	float4 vLightRadiance = float4(1.f, 1.f, 1.f, 1.f);
	//Shadow
	if (true == g_bShadow)
	{
		vector vShadowDepth = g_DepthShadowTexture.Sample(LinearSampler, In.vTexUV);

		float2 uv = vShadowDepth.xy / vShadowDepth.w;
		uv.y = -uv.y;
		uv = uv*0.5f + 0.5f;

		float fCurrentDepth = (vShadowDepth.z / vShadowDepth.w);
		float3 vRgb = saturate(vDiffuseBRDF.xyz);
		float fBias = 0.0000125f;
		float fShadowDepth = g_ShadowMapTexture.Sample(LinearSamplerClamp, uv).r;

		if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
		{
			if (fShadowDepth < fCurrentDepth - fBias)
				vRgb *= 0.3f;
		}

		vDiffuseBRDF.xyz = vRgb;
	}
	float vRadiance = SpotlightIntensity(g_vLightPos.xyz, g_vLightDir.xyz, g_fOuterConeAngle, g_fInnerConeAngle, vPosition.xyz);
	//SpecularBRDF
	Out.vSpecular.rgb = (vF*fD*fG) / max(0.001f, 4.f*fCosLi*fCosLo);
	Out.vSpecular.a = 0.f;

	float fAO = g_RomTexture.Sample(LinearSampler, In.vTexUV).y;
	float fSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV).y;
	if (false == g_bAO)
		fAO = 1.f;
	if (false == g_bAO)
		fSSAO = 1.f;

	Out.vNonIBL = (vDiffuseBRDF* vShade + Out.vSpecular)*vRadiance*g_vLightDiffuse*fSSAO/**fCosLi*//**0.001f*/;
	Out.vNonIBL.w = vDiffuseBRDF.w;

	Out.vSpecular = Out.vNonIBL;
	//float3 F = FresnelWithRoughness(fCosLo, vF0, fRoughness);



	//float3 irradiance = g_IrradianceTexture.Sample(LinearSampler, vNormal.xyz).rgb;
	//float3 diffuse = irradiance * vDiffuseBRDF.xyz;

	////float3 R = reflect(-vLo, vNormal.xyz);

	////float  Max_Reflection_Lod = 4.f;
	////float3 prefilteredColor = g_PreFilterTexture.SampleLevel(LinearSampler, R, fRoughness * MAX_REFLECTION_LOD).rgb;
	////float2 envBRDF = g_BRDF_LUTTexture.Sample(LinearSampler, float2(max(dot(vNormal.xyz, vLo), 0.0), fRoughness)).rg;
	////float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	//float3 ambient = (vKd * diffuse /*+ specular*/);

	//float3 color = ambient;

	//color = color / (color + float3(1.0, 1.0, 1.0));
	//color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	//Out.vNonIBL += float4(color, 1.f);
	return Out;
}
PS_OUT_LIGHT_ROM PS_MAIN_POINT_ROM(PS_IN In)
{
	PS_OUT_LIGHT_ROM   Out = (PS_OUT_LIGHT_ROM)0;
	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector vDiffuseBRDF = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float fRoughness = g_RomTexture.Sample(LinearSampler, In.vTexUV).x;
	//if (0.1f >= fRoughness)
	//	fRoughness = 0.1f;
	float fMetalness = g_RomTexture.Sample(LinearSampler, In.vTexUV).z;

	vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float      fViewZ = vDepthDesc.y * 300.f;


	vector      vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.r;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	//Outline
	//vector      vLightDir = vPosition - g_vLightPos;
	//float      fDistance = length(vLightDir);
	//float      fDot = max(0, dot(normalize(vNormal), normalize(vLightDir)));
	//vector      vOutLine = g_OutlineTexture.Sample(LinearSampler, In.vTexUV);

	//if (vOutLine.r == 0.f)
	//{
	//   fDot = 0.f;
	//   vOutLine.r = 1.f;
	//}
	vector      vLightDir = vPosition - g_vLightPos;

	float4 vShade;
	vShade = g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal))/**(fDot + vOutLine.r)*/ + g_fShadeAmbient /*(g_vLightAmbient * g_vMtrlAmbient)*/;
	vShade.a = 1.f;
	float3 vLo = normalize(g_vCamPosition.xyz - vPosition.xyz);
	float fCosLo = saturate(dot(normalize(vNormal.xyz), vLo));

	float3 vF0 = 0.04f;
	vF0 = Fresnel_Reflectance(vF0, vDiffuseBRDF.xyz, fMetalness);

	//Lightning
	float      fDistance = length(vLightDir);
	float      fAtt = 0.f;
	float4	   vLightDiffuse = g_vLightDiffuse;

	if (1 == (uint)vLightDiffuse.w)
	{
		fAtt = saturate((g_fRange - fDistance) / (g_fRange*g_fOffset));
	}
	else
	{
		fAtt = saturate((g_fRange - fDistance) / (fDistance*fDistance + 0.000001f));
		vLightDiffuse.w = 1.f;
	}

	float3 vLightInverse = -vLightDir.xyz;
	float3 vLh = normalize((normalize(vLightInverse) + normalize(vLo)));

	float fCosLi = saturate(dot(normalize(vNormal.xyz), normalize(vLightInverse)));
	float fCosLh = saturate(dot(normalize(vNormal.xyz), normalize(vLh)));

	//F : Relate To Mettal
	float3      vF = Fresnel(vLh, vLo, vF0);

	//D : Relate To Roughness
	float      fD = NormalDistributionGGX(normalize(vNormal.xyz), vLh, fRoughness);

	//G : Relate TO Roughness
	float      fG = Geometry(normalize(vNormal.xyz), vLo, vLightInverse, fRoughness);

	//DiffuseBRDF
	float3 vAmbient = float3(0.f, 0.f, 0.f);
	if (0.9f <= fMetalness)
	{
		vAmbient = float3(0.6f, 0.6f, 0.6f);
		fMetalness = 0.f;
	}

	float3 vKd = lerp(float3(1.f, 1.f, 1.f) - vF, float3(0.0f, 0.0f, 0.0f), fMetalness) + vAmbient;
	const float3 kS = vF;
	const float3 kD = (float3(1, 1, 1) - kS) * saturate(1.0f - fMetalness + vAmbient) * vDiffuseBRDF.xyz;

	vDiffuseBRDF.xyz = kD;

	float4 vLightRadiance = float4(1.f, 1.f, 1.f, 1.f);

	//SpecularBRDF

	//fAtt = saturate((g_fRange - fDistance) / g_fRange);
	//vector      vReflect = reflect(normalize(vLightDir), vNormal);
	//vector      vLook = vPosition - g_vCamPosition;

	//Out.vSpecular = ((g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.0f)) * fAtt;
	//Out.vSpecular.a = 0.f;

	Out.vSpecular.rgb = (vF*fD*fG) / max(0.001f, 4.f*fCosLi*fCosLo);
	Out.vSpecular.a = 0.f;

	//if (fDistance < g_fRange)
	//{
	float fAO = g_RomTexture.Sample(LinearSampler, In.vTexUV).y;
	float fSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV).y;
	if (false == g_bAO)
		fAO = 1.f;
	if (false == g_bAO)
		fSSAO = 1.f;


	Out.vNonIBL = (vDiffuseBRDF*vShade + Out.vSpecular)*vLightDiffuse*fAtt/*(1.f / (fDistance*fDistance))*/*g_fPower*fSSAO;
	Out.vNonIBL.w = vDiffuseBRDF.w;

	Out.vSpecular = Out.vNonIBL;
	Out.vNonIBL = float4(fAO, fAO, fAO, 1.f);

	//}
	//else
	//{
	//	Out.vNonIBL = vector(0.f,0.f,0.f,0.f);
	//	Out.vNonIBL.w = vDiffuseBRDF.w;

	//	Out.vSpecular = Out.vNonIBL;
	//}

	//float3 F = FresnelWithRoughness(fCosLo, vF0, fRoughness);



	//float3 irradiance = g_IrradianceTexture.Sample(LinearSampler, vNormal.xyz).rgb;
	//float3 diffuse = irradiance * vDiffuseBRDF.xyz;

	////float3 R = reflect(-vLo, vNormal.xyz);

	////float  Max_Reflection_Lod = 4.f;
	////float3 prefilteredColor = g_PreFilterTexture.SampleLevel(LinearSampler, R, fRoughness * MAX_REFLECTION_LOD).rgb;
	////float2 envBRDF = g_BRDF_LUTTexture.Sample(LinearSampler, float2(max(dot(vNormal.xyz, vLo), 0.0), fRoughness)).rg;
	////float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	//float3 ambient = (vKd * diffuse /*+ specular*/);

	//float3 color = ambient;

	//color = color / (color + float3(1.0, 1.0, 1.0));
	//color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	//Out.vNonIBL += float4(color, 1.f);
	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT   Out = (PS_OUT_LIGHT)0;

	//vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	//vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	//vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	//float      fViewZ = vDepthDesc.y * 300.f;

	//vector      vPosition;

	//vPosition.x = In.vTexUV.x * 2.f - 1.f;
	//vPosition.y = In.vTexUV.y * -2.f + 1.f;
	//vPosition.z = vDepthDesc.r;
	//vPosition.w = 1.f;

	//vPosition *= fViewZ;

	//vPosition = mul(vPosition, g_ProjMatrixInv);
	//vPosition = mul(vPosition, g_ViewMatrixInv);


	//vector      vLightDir = vPosition - g_vLightPos;
	//float      fDistance = length(vLightDir);

	//float      fAtt = saturate((g_fRange - fDistance) / g_fRange);

	//Out.vShade =  (g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	//Out.vShade.a = 1.f;

	//
	//vector      vReflect = reflect(normalize(vLightDir), vNormal);
	//vector      vLook = vPosition - g_vCamPosition;

	//Out.vSpecular = ((g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.0f)) * fAtt;
	//Out.vSpecular.a = 0.f;

	return Out;
}
PS_OUT_IBL PS_MAIN_IBL(PS_IN In)
{
	PS_OUT_IBL      Out = (PS_OUT_IBL)0;
	float fRoughness = g_RomTexture.Sample(LinearSampler, In.vTexUV).x;
	Out.vIrradiance = CreateIrradiance(In.vLocalPosition.xyz, g_HDRTexture, LinearSampler);
	//Out.vIrradiance = vector(0.f, 0.f, 0.f, 0.f);
	Out.vPreFilter = CreatePreFilter(In.vLocalPosition.xyz, g_HDRTexture, LinearSampler, fRoughness);
	//Out.vPreFilter = vector(0.f, 0.f, 0.f, 0.f);

	vector      vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
	vector      vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float      fViewZ = vDepthDesc.y * 300.f;
	vector      vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.r;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	float3 vLo = normalize(g_vCamPosition.xyz - vPosition.xyz);
	float fCosLo = saturate(dot(vNormal.xyz, vLo));
	Out.vBRDF_LUT = float4(IntegrateBRDF(fCosLo, fRoughness), 0.f, 1.f);


	//Out.vBRDF_LUT = vector(0.f, 0.f, 0.f, 0.f);



	return Out;
}
PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector      vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector      vRimlight = g_RimlightTexture.Sample(LinearSampler, In.vTexUV);
	vector      vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);

	float4 vResult;

	//if (vRimlight.r == 0.f)
	//   vResult = vDiffuse;
	//else
	vResult = float4(vRimlight.rgb / 2.f + vDiffuse.rgb / 2.f, vDiffuse.a);

	Out.vColor = vResult * vShade /*+ vRom*/;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLEND_ROM(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_LOGO_TEXT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vDiffuse.a *= g_fTimeAcc;

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_MAIN_LOGO_TITLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
		discard;

	Out.vColor.a = 0.5f;

	return Out;
}

PS_OUT PS_MAIN_LOADING_BG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4 vMix = g_MixTexture.Sample(LinearSampler, In.vTexUV);
	float4 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
	float4 vSpiral = g_SpiralTexture.Sample(LinearSampler, In.vTexUV);

	vNoise = float4(0.f, 150.f / 255.f, 100.f / 255.f, 1.f);		// Noise Color
	vSpiral = float4(0.f, 214.f / 255.f, 120.f / 255.f, 1.f);	// Spiral Color

	Out.vColor = (vSpiral * vMix.r) + (vNoise * (1.f - vDiffuse.r));
	Out.vColor.a = 0.5;

	return Out;
}

PS_OUT PS_MAIN_LOADING_FIFO(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4 vMix = g_MixTexture.Sample(LinearSampler, In.vTexUV);

	// 초록 -> 흰색
	if (vDiffuse.r > 0.5f)
		vDiffuse = vDiffuse * vMix;
	else
		vDiffuse = vMix;

	vDiffuse.a = vDiffuse.r;

	Out.vColor = vDiffuse * g_fTimeAcc;

	//if (vDiffuse.r < 0.5f)
	//	vDiffuse = vMix * vDiffuse;
	//else
	//	vDiffuse = vMix;

	//vDiffuse.a = vDiffuse.r;

	//Out.vColor = vDiffuse * g_fTimeAcc;

	return Out;
}

PS_OUT PS_MAIN_ALPHA_TEST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_SKILL_UI(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4 vMix = g_MixTexture.Sample(LinearSampler, In.vTexUV);
	float4 vTempColor;
	vTempColor.rgba = 0.f;
	float4 vColor;

	if (vMix.r > g_fTimeAcc)
		vColor = vDiffuse;
	else
		vColor = vTempColor;

	Out.vColor = vColor;

	if (0.3f > Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector      vNormalDesc = g_NormalTexture.Sample(LinearSamplerClamp, In.vTexUV);
	//vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector      vDepthDesc = g_DepthTexture.Sample(LinearSamplerClamp, In.vTexUV);

	float      fViewZ = vDepthDesc.y * 300.f;
	float	   fDepth = vDepthDesc.x*fViewZ;

	vNormalDesc = normalize(vNormalDesc*2.f - 1.f);
	Out.vColor = Get_SSAO(In.vTexUV, fDepth, fViewZ, vNormalDesc.xyz, g_DepthTexture, LinearSampler, g_fSSAORadious, g_fBias, 16);
	//Out.vColor = 1.f - Out.vColor;
	return Out;
}

PS_OUT PS_MAIN_PLAYERHPBAR_HEAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vDiffuse.a != 0.f)
		vDiffuse.rgb = float4(0.f, 1.0f, 0.3f, 1.f);

	vDiffuse.a *= g_fTimeAcc;

	Out.vColor = vDiffuse;
	return Out;
}

PS_OUT PS_MAIN_PLAYERHPBAR_DAMAGED(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vDiffuse.a != 0.f)
		vDiffuse.rgb = float4(1.f, 1.0f, 0.f, 1.f);

	vDiffuse.a *= (1.f - g_fTimeAcc);

	Out.vColor = vDiffuse;
	return Out;
}
PS_OUT PS_MAIN_DIVIDE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vTextUV = In.vTexUV;
	vTextUV.x = vTextUV.x *(1.f / g_vRatioMax.x) + (1.f / g_vRatioMax.x)*(g_vRatio.x - 1.f);
	vTextUV.y = vTextUV.y *(1.f / g_vRatioMax.y) + (1.f / g_vRatioMax.y)*(g_vRatio.y - 1.f);
	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTextUV);
	if (0.3f > Out.vColor.a)
		discard;
	return Out;
}

PS_OUT PS_UV_PERCENTCULL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vNewTexUV = In.vTexUV;
	float fUVPer = 1 - (g_fUVPercent / 100.f);

	if (fUVPer > vNewTexUV.x)
		discard;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vNewTexUV);

	Out.vColor = vDiffuse;

	if (0.3f > Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_FADEINOUT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.a = g_fAlpha;
	Out.vColor = g_vColor;
	return Out;
}

//PS_OUT PS_FADEINOUT_DISSOLVE(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	Out.vColor = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
//
//	if (vDissolve.r < g_fDissolveTime)
//	{
//		Out.vColor= vector(0.f, 0.f, 0.f, 1.f);
//	}
//	else
//	{
//		discard;
//	}
//	return Out;
//}

PS_OUT PS_MAIN_PLAYERHPBAR_HEAL_DEFAULT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (1 == g_iHpIndex)
	{
		if (In.vTexUV.x < 0.27f && In.vTexUV.x >= g_fTimeAcc * 0.25f)
			discard;
	}
	else if (2 == g_iHpIndex)
	{
		if ((In.vTexUV.x > 0.27f && In.vTexUV.x < 0.55f) && In.vTexUV.x >= g_fTimeAcc * 0.25f + 0.25f)
			discard;
	}
	else if (3 == g_iHpIndex)
	{
		if ((In.vTexUV.x > 0.55f && In.vTexUV.x < 0.75f) && In.vTexUV.x >= g_fTimeAcc * 0.25f + 0.5f)
			discard;
	}
	else if (4 == g_iHpIndex)
	{
		if (In.vTexUV.x > 0.75f && In.vTexUV.x >= g_fTimeAcc * 0.25f + 0.75f)
			discard;
	}

	Out.vColor = vDiffuse;
	return Out;
}

// 프로그래스바(round)
PS_OUT PS_RatioRoundLoading(PS_IN In)
{
	PS_OUT vOut = (PS_OUT)0;

	float gradient = g_RadiusLoadingTexture.Sample(LinearSampler, In.vTexUV).r;
	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNonColor;
	vNonColor.rgba = 0.f;

	vector col;

	if (g_fRatio > gradient)
		col = vColor;
	else
		col = vNonColor;

	vOut.vColor = col;
	return vOut;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	if (Out.vColor.a > 0.8f)
		Out.vColor.a = g_fAlpha;

	if (0.f > g_fAlpha)
		discard;

	return Out;
}

PS_OUT PS_UV_PERCENTCULL1(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vNewTexUV = In.vTexUV;
	float fUVPer = (g_fUVPercent / 100.f);

	if (fUVPer < vNewTexUV.x)
		discard;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vNewTexUV);

	Out.vColor = vDiffuse;

	if (0.3f > Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_2D_PLAYER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
		discard;

	if (true == g_bDissolve)
	{
		vector vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

		if (vDissolve.r - g_fDissolveTime < g_fDissolveStart)
		{
			discard;
		}
	}

	return Out;
}

technique11         DefaultTechnique
{
	pass Default_0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional_1
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}


	pass Light_Point_2
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	//Pass 3 : Light_Point_Rom
	pass Light_Point_Rom_3
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT_ROM();
	}

	//Pass 4 : Light_Direction_Rom
	pass Light_Directional_Rom_4
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL_ROM();
	}
	//Pass 5 : Light_Spot_Rom
	pass Light_Point_Rom_5
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPOT_ROM();
	}
	//Pass 6 : Blending_Rom
	pass Deferred_Blending_Rom_6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND_ROM();
	}

	//Pass 7 : Logo_Text
	pass Logo_Text_7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOGO_TEXT();
	}

	//Pass 8 : Logo_Title
	pass Logo_Title_8
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOGO_TITLE();
	}

	//Pass 9 : Loading_BG
	pass Loading_BG
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOADING_BG();
	}

	// Pass 10 : Loading_FIFO
	pass Loading_FIFO
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOADING_FIFO();
	}

	// Pass 11 : AlphaTest : UI
	pass AlphaTestUI
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_TEST();
	}

	// Pass 12 : SkillUI
	pass SkillUI
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKILL_UI();
	}
	//Pass 13 : IBL
	pass IBL_13
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_IBL();
	}
	//Pass 14 : CUBE
	pass CUBE_14
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_CUBE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CUBE();
	}
	//Pass 15 : BLUR_Horizon
	pass BLUR_X_15
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_HORISON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
	}

	pass BLUR_Y_16
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_VERTICAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
	}

	pass MOTIONBLUR_17
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
	}

	pass Brightness_18
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BRIGHTNESS();
	}

	pass Bloom_19
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
	}

	pass ToneMapping_20
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TONEMAPPING();
	}

	pass DepthBlur_21
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEPTHBLUR();
	}

	pass DOP_22
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOP();
	}

	pass HDR_23
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HDR();
	}

	pass SSAO_24
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SSAO();
	}

	pass HPBAR_DAMAGED_25
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PLAYERHPBAR_DAMAGED();
	}
	pass Effect_26
	{
		SetBlendState(Blending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TOTALDIFFUSE();
	}

	/*이미지 분할 재생*/
	pass DIVIDE_27
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIVIDE();
	}

	pass NONCULL_28
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_TEST();
	}

	pass UVCULL_29
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UV_PERCENTCULL();
	}

	pass FADE_30
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FADEINOUT();
	}

	pass HPBAR_HEAL_31
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PLAYERHPBAR_HEAL();
	}

	pass HPBAR_DEFAULT_32
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PLAYERHPBAR_HEAL_DEFAULT();
	}

	// 프로그래스바
	pass Render_RatioRound_33
	{
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RatioRoundLoading();
	}

	//몬스터 UI
	pass MONSTER_UI_34
	{
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	pass MSAA_35
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MSAA();
	}
	pass LUT_36
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LUT();
	}

	pass FOG_37
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FOG();
	}

	pass RADIAL_38
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RADIAL();
	}

	pass UVCULL_39
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UV_PERCENTCULL1();
	}

	pass PLAYER_2D_40
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_2D_PLAYER();
	}

}