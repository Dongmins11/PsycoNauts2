#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};
cbuffer OldMatrices
{
	float4x4 g_OldWorldMatrix;
	float4x4 g_OldViewMatrix;
};

struct tagBoneArray
{
	matrix		BoneMatrices[300];
};

cbuffer BoneMatrices
{
	tagBoneArray			g_BoneMatrices;
	tagBoneArray			g_OldBoneMatrices;
};
cbuffer Rimlight
{
	float4			g_vWorldPos;
	float4			g_vCamPosition;
	float			g_fPower = 30.f;
	float4			g_vRGBA;
	float2			g_vRimRatio;
};
cbuffer Light
{
	float4		g_vLightDir;
	float4		g_vLightPos;
	float		g_fLightRange;
	float4x4   g_LightViewMatrix;
};
cbuffer PBR
{
	float g_fPI = 3.141592f;
	float g_fEpsilon = 0.00001f;
	float3 g_vFdielectric = float3(0.04f, 0.04f, 0.04f);
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

cbuffer RenderOption
{
	bool g_bRimlight = false;
	bool g_bOutline = false;
	bool g_bMotionBlur = false;
	bool g_bShadow = false;
	bool g_bEmissive = true;

};
cbuffer NormalMapping
{
	bool g_bIsNormalMapping = true;
};

cbuffer Effect
{
	float	g_fTimeAcc = 0.f;
	float3	g_vColor = float3(1.f, 0.7f, 0.f);
	float3	g_vEndColor = float3(1.f, 0.2f, 0.f);
};

cbuffer Alpha
{
	float	g_fAlpha = 0.f;
};
cbuffer OffsetRGB
{
	float3 g_vOffsetRGBA;
};

cbuffer Dissovle
{
	float g_fDissolveTime = 0.f;
	bool  g_bDissolve = false;
	float g_fDissolveStart = 0.1f;
};

cbuffer BlendingTimer
{
	float g_fBlendingTime = 0.f;
};

texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_RimlightTexture;
texture2D			g_RomTexture;
texture2D			g_ShadowTexture;
texture2D			g_EmissiveTexture;
texture2D			g_AlphaMapTexture;
texture2D			g_DissolveTexture; /* 성준 */

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;

	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_point;

	AddressU = wrap;
	AddressV = wrap;
};
sampler LinearSamplerClamp = sampler_state
{
	filter = min_mag_mip_linear;

	AddressU = Clamp;
	AddressV = Clamp;
};
sampler AnisotropicSamplerLinear = sampler_state
{
	filter = anisotropic;
	MaxAnisotropy = 16;

	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};
struct VS_IN_SHADOWMAP
{
	float3      vPosition : POSITION;

};
struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vWorldPosition :POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float4      vClipPosition: TEXCOORD2;
	float4      vDir: TEXCOORD3;

};

struct VS_OUT_SHADOWMAP
{
	float4		vPosition : SV_POSITION;
	float4		vClipPosition: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;


	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector	vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
	vector	vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	Out.vPosition = mul(vPosition, matWVP);
	Out.vWorldPosition = mul(vPosition, g_WorldMatrix);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	//if (true == g_bIsNormalMapping)
	//	Out.vNormal.xyz *= -1.f;

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	//if (true == g_bIsNormalMapping)
	//	Out.vTangent.xyz *= -1.f;
	Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);


	//Shadow
	if (true == g_bShadow)
	{
		float4 WorldPosition = mul(vPosition, g_WorldMatrix);
		Out.vClipPosition = mul(WorldPosition, g_LightViewMatrix);
		Out.vClipPosition = mul(Out.vClipPosition, g_ProjMatrix);
	}
	else
	{
		Out.vClipPosition = vector(0.f, 0.f, 0.f, 0.f);
	}

	//Velocity
	if (true == g_bMotionBlur)
	{
		matrix OldBoneMatrix = mul(g_OldBoneMatrices.BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x) +
			mul(g_OldBoneMatrices.BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y) +
			mul(g_OldBoneMatrices.BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z) +
			mul(g_OldBoneMatrices.BoneMatrices[In.vBlendIndex.w], In.vBlendWeight.w);

		//Velocity
		float4 vNewPos = Out.vPosition;
		float4 vOldPos = mul(vector(In.vPosition, 1.f), OldBoneMatrix);

		vOldPos = mul(vOldPos, g_OldWorldMatrix);
		float4 vWorldOldPos = vOldPos;

		vOldPos = mul(vOldPos, g_OldViewMatrix);
		vOldPos = mul(vOldPos, g_ProjMatrix);

		//float3 vDir = vNewPos.xyz - vOldPos.xyz;
		float3 vDir = Out.vWorldPosition.xyz - vWorldOldPos.xyz;

		//float4 WorldNewPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
		//float4 WorldOldPos = mul(vector(In.vPosition, 1.f), g_OldWorldMatrix);
		float4 VelocityNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
		VelocityNormal = mul(VelocityNormal, g_WorldMatrix);
		//float3 vWorldDir = WorldNewPos.xyz - WorldOldPos.xyz;

		//matrix NormalMatrix = matWVP;
		//NormalMatrix._11 = 1.0f; NormalMatrix._22 = 1.0f; NormalMatrix._33 = 1.0f;
		//NormalMatrix._41 = 0.0f; NormalMatrix._42 = 0.0f; NormalMatrix._43 = 0.0f;

		//VelocityNormal = mul(VelocityNormal, g_ViewMatrix);
		//VelocityNormal = mul(VelocityNormal, g_ProjMatrix);

		float fDot = dot(normalize(vDir), normalize(VelocityNormal.xyz));
		if (fDot < 0.f)
			Out.vPosition = vOldPos;
		else
			Out.vPosition = vNewPos;


		float2 vVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
		Out.vDir.xy = vVelocity*0.5f;

		Out.vDir.y *= -1.f;
		Out.vDir.z = Out.vPosition.z;
		Out.vDir.w = Out.vPosition.w;

		//if (length(g_vCamPosition - vWorldPos)<5.f)
		//	Out.vDir.xy = 0.f;
	}
	else
	{
		Out.vDir = vector(0.f, 0.f, 0.f, 0.f);
	}
	return Out;
}

VS_OUT_SHADOWMAP VS_MAIN_SHADOWMAP(VS_IN In)
{
	VS_OUT_SHADOWMAP		Out = (VS_OUT_SHADOWMAP)0;

	matrix		matWV, matWVP;
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector	vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	matWV = mul(g_WorldMatrix, g_LightViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);

	float4 vWolrdPos = mul(vPosition, g_WorldMatrix);
	Out.vClipPosition = mul(vWolrdPos, g_LightViewMatrix);
	Out.vClipPosition = mul(Out.vClipPosition, g_ProjMatrix);
	return Out;
}

struct VS_OUT_ANIMATION
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT_ANIMATION VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT_ANIMATION		Out = (VS_OUT_ANIMATION)0;
	
	matrix		matWV, matWVP;
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	Out.vPosition = mul(vPosition, matWVP);

	vector	vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vWorldPosition :POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float4      vClipPosition: TEXCOORD2;
	float4      vDir: TEXCOORD3;

};

struct PS_IN_SHADOWMAP
{
	float4		vPosition : SV_POSITION;
	//float4		vNormal : NORMAL;
	float4		vClipPosition: TEXCOORD0;

};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vRimlight : SV_TARGET3;
	float4		vRom : SV_TARGET4;
	float4		vVelocity : SV_TARGET5;
	float4		vEmissive : SV_TARGET6;
	float4		vDepthShadow : SV_TARGET7;
};
struct PS_OUT_SHADOWMAP
{
	float4      vShadowMap : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(AnisotropicSamplerLinear, In.vTexUV);
	//if (Out.vDiffuse.a < 0.3f)
	//	discard;
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);


	vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz*2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal =mul(vNormal, WorldMatrix);

	//vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	//if (0.5f > vNormalDesc.z)
	//{
	//	vNormalDesc.z = sqrt(1 - ((vNormalDesc.x * vNormalDesc.x) + (vNormalDesc.y * vNormalDesc.y)))*2.f - 1.f;
	//	vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
	//	vNormalDesc.y = -vNormalDesc.y*2.f + 1.f;
	//	//vNormalDesc.z = 1.f;

	//	//vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
	//	//vNormalDesc.y = vNormalDesc.y*2.f - 1.f;
	//	//vNormalDesc.z = vNormalDesc.z*2.f - 1.f;

	//}

	//else
	//{
	//	vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
	//	vNormalDesc.y = vNormalDesc.y*2.f - 1.f;
	//	vNormalDesc.z = vNormalDesc.z*2.f - 1.f;
	//}

	//float3	vNormal = normalize(vNormalDesc.xyz);

	//float3x3		WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
	//WorldMatrix = transpose(WorldMatrix);

	//vNormal = mul(WorldMatrix, vNormal);
	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	if (true == g_bEmissive)
		Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	else
	{
		Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	}
	if (true == g_bRimlight)
	{
		vector vResult;

		vector		vLook = g_vCamPosition - In.vWorldPosition;
		float		fFresnel = pow(1.f - max(0.f, dot(normalize(float4(vNormal, 0.f)), normalize(vLook))), g_fPower);
		vector		vReflect = reflect(normalize(g_vLightDir),float4(vNormal, 0.f));
		vector		vT1 = vector(0.f, 1.f, 0.f, 0.f);
		vector		vT = vector(0.f, 0.25f, 0.f, 0.f);
		float		fL = max(0.f, dot(normalize(vT1), normalize(float4(vNormal, 0.f))));
		float		fM = max(0.f, (1.f - max(0.f, dot(normalize(vReflect), vT))) - pow(fL, 0.8));

		vResult = g_vRGBA* fFresnel* fM;
		Out.vRimlight = vResult / g_vRimRatio.x;
		Out.vDiffuse = float4(Out.vDiffuse.rgb / g_vRimRatio.y, Out.vDiffuse.a);
	}
	else
	{
		Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}
	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = In.vDir.z / In.vDir.w;
		Out.vVelocity.w = In.vDir.w / 300.f;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse.rgb += g_vOffsetRGBA;
	return Out;
}
////Outline
//PS_OUT PS_MAIN_OUTLINE(PS_IN In)
//{
//	PS_OUT			Out = (PS_OUT)0;
//
//	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
//	if (Out.vDiffuse.a < 0.3f)
//		discard;
//	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);
//
//	vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
//	float3 vNormal = vNormalDesc.xyz*2.f - 1.f;
//
//	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
//	vNormal = mul(vNormal, WorldMatrix);
//	Out.vOutline = float4(vNormal, 0.f);
//
//	for (uint i = 0; i < 25; ++i)
//	{
//		vector vBlendColor = g_fLaplacianMask[i] * g_NormalTexture.Sample(LinearSampler, In.vTexUV + float2(g_fCoord[i % 5] / g_fWinCX, g_fCoord[i / 5] / g_fWinCY));
//		Out.vOutline += vBlendColor;
//	}
//
//	float fBlack = 1 - (Out.vOutline.r*0.3f + Out.vOutline.g*0.59f + Out.vOutline.b*0.11f);
//	Out.vOutline = vector(fBlack, fBlack, fBlack, 1.f);
//	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
//	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
//	Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
//
//	return Out;
//}

PS_OUT PS_PREVIEW(PS_IN In)
{
	PS_OUT        Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);


	vector    vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3    vNormal = vNormalDesc.xyz * 2.f - 1.f;

	Out.vDiffuse.a = 0.4;
	Out.vDiffuse.rb = 0.f;

	float3x3        WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	return Out;
}
PS_OUT_SHADOWMAP PS_MAIN_SHADOWMAP(PS_IN_SHADOWMAP In)
{
	PS_OUT_SHADOWMAP        Out = (PS_OUT_SHADOWMAP)0;

	float fDepth = In.vClipPosition.z / In.vClipPosition.w;

	Out.vShadowMap = float4(fDepth, fDepth, fDepth, 1.f);

	return Out;
}
PS_OUT_SHADOWMAP PS_MAIN_TEST(PS_IN In)
{
	PS_OUT_SHADOWMAP        Out = (PS_OUT_SHADOWMAP)0;

	Out.vShadowMap = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	return Out;
}
PS_OUT PS_MAIN_BLENDING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//if (Out.vDiffuse.a < 0.3f)
	//	discard;
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);


	vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz*2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	if (true == g_bEmissive)
		Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	else
	{
		Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	}
	//if (true == g_bRimlight)
	//{
	//	vector vResult;

	//	vector		vLook = g_vCamPosition - In.vWorldPosition;
	//	float		fFresnel = pow(1.f - max(0.f, dot(normalize(float4(vNormal, 0.f)), normalize(vLook))), g_fPower);
	//	vector		vReflect = reflect(normalize(g_vLightDir), float4(vNormal, 0.f));
	//	vector		vT1 = vector(0.f, 1.f, 0.f, 0.f);
	//	vector		vT = vector(0.f, 0.25f, 0.f, 0.f);
	//	float		fL = max(0.f, dot(normalize(vT1), normalize(float4(vNormal, 0.f))));
	//	float		fM = max(0.f, (1.f - max(0.f, dot(normalize(vReflect), vT))) - pow(fL, 0.8));

	//	vResult = g_vRGBA* fFresnel* fM;
	//	Out.vRimlight = vResult;
	//	vResult = float4(vResult.rgb / g_vRimRatio.x + Out.vDiffuse.rgb / g_vRimRatio.y, Out.vDiffuse.a);

	//	Out.vDiffuse = vResult;
	//}
	//else
	//{
		Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	//}
	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = In.vDir.z / In.vDir.w;
		Out.vVelocity.w = In.vDir.w / 300.f;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse.a -= g_fAlpha;
	if (0.1f > Out.vDiffuse.a)
		Out.vDiffuse.a = 0.1f;
	//Out.vDiffuse.a = 0.4f;

	return Out;
}
struct PS_IN_ANIMATION
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT_ANIMATION
{
	float4		vDiffuse : SV_TARGET0;
};

PS_OUT_ANIMATION PS_MAIN_EFFECT(PS_IN_ANIMATION In)
{
	PS_OUT_ANIMATION		Out = (PS_OUT_ANIMATION)0;

	vector vDiffuse = g_AlphaMapTexture.Sample(LinearSampler, In.vTexUV); //AlphaMap

	float3 vColor = g_vColor;
	vColor.r = saturate(vColor.r - ((1.f - vDiffuse.a) * saturate((vColor.r - g_vEndColor.r))));
	vColor.g = saturate(vColor.g - ((1.f - vDiffuse.a) * saturate((vColor.g - g_vEndColor.g))));
	vColor.b = saturate(vColor.b - ((1.f - vDiffuse.a) * saturate((vColor.b - g_vEndColor.b))));

	Out.vDiffuse.rgb = vColor;

	Out.vDiffuse.a = vDiffuse.a * g_fTimeAcc;
	if (Out.vDiffuse.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DISCARD(PS_IN In)
{
	PS_OUT        Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	//Out.vDiffuse.arbg = 
	discard;

	return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);

	vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz*2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	if (true == g_bEmissive)
	{
		Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
		if (true == g_bDissolve)
		{
			vector vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

			if (vDissolve.r - g_fDissolveTime < 0.0f)
			{
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);
				discard;
			}
			else if (vDissolve.r - g_fDissolveTime < g_fDissolveStart)
			{
				Out.vEmissive = Out.vDiffuse;
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);

			}
		}
	}
	else
	{
		if (true == g_bDissolve)
		{
			vector vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

			if (vDissolve.r - g_fDissolveTime < 0.0f)
			{
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);
				discard;
			}
		}

		Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	}
	if (true == g_bRimlight)
	{
		vector vResult;

		vector		vLook = g_vCamPosition - In.vWorldPosition;
		float		fFresnel = pow(1.f - max(0.f, dot(normalize(float4(vNormal, 0.f)), normalize(vLook))), g_fPower);
		vector		vReflect = reflect(normalize(g_vLightDir), float4(vNormal, 0.f));
		vector		vT1 = vector(0.f, 1.f, 0.f, 0.f);
		vector		vT = vector(0.f, 0.25f, 0.f, 0.f);
		float		fL = max(0.f, dot(normalize(vT1), normalize(float4(vNormal, 0.f))));
		float		fM = max(0.f, (1.f - max(0.f, dot(normalize(vReflect), vT))) - pow(fL, 0.8));

		vResult = g_vRGBA* fFresnel* fM;
		Out.vRimlight = vResult / g_vRimRatio.x;
		Out.vDiffuse = float4(Out.vDiffuse.rgb / g_vRimRatio.y, Out.vDiffuse.a);
	}
	else
	{
		Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}
	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = In.vDir.z / In.vDir.w;
		Out.vVelocity.w = In.vDir.w / 300.f;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse.rgb += g_vOffsetRGBA;
	return Out;
}

PS_OUT PS_MAIN_BLENDINGTIME(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//if (Out.vDiffuse.a < 0.3f)
	//	discard;
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);


	vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz*2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	if (true == g_bEmissive)
		Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	else
	{
		Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	}
	//if (true == g_bRimlight)
	//{
	//	vector vResult;

	//	vector		vLook = g_vCamPosition - In.vWorldPosition;
	//	float		fFresnel = pow(1.f - max(0.f, dot(normalize(float4(vNormal, 0.f)), normalize(vLook))), g_fPower);
	//	vector		vReflect = reflect(normalize(g_vLightDir), float4(vNormal, 0.f));
	//	vector		vT1 = vector(0.f, 1.f, 0.f, 0.f);
	//	vector		vT = vector(0.f, 0.25f, 0.f, 0.f);
	//	float		fL = max(0.f, dot(normalize(vT1), normalize(float4(vNormal, 0.f))));
	//	float		fM = max(0.f, (1.f - max(0.f, dot(normalize(vReflect), vT))) - pow(fL, 0.8));

	//	vResult = g_vRGBA* fFresnel* fM;
	//	Out.vRimlight = vResult;
	//	vResult = float4(vResult.rgb / g_vRimRatio.x + Out.vDiffuse.rgb / g_vRimRatio.y, Out.vDiffuse.a);

	//	Out.vDiffuse = vResult;
	//}
	//else
	//{
	Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	//}
	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = In.vDir.z / In.vDir.w;
		Out.vVelocity.w = In.vDir.w / 300.f;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse.a = g_fBlendingTime;
	//if (0.1f > Out.vDiffuse.a)
	//	Out.vDiffuse.a = 0.1f;
	//Out.vDiffuse.a = 0.4f;

	return Out;
}


technique11			DefaultTechnique
{
	pass Default_0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// Pass 1 : 그림자맵
	pass ShadowMap_1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOWMAP();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOWMAP();
	}
	// Pass 2 : 미정
	pass ShadowMap_2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOWMAP();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOWMAP();
	}

	// Pass 3 : 미정
	pass ShadowMap_3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOWMAP();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOWMAP();
	}

	//Pass 4 : Tool Preview-> Tool에서 미리 보기 기능 
	pass Preview_4
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PREVIEW();
	}
	// Pass 5 : 미정
	pass ShadowMap_5
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEST();
	}

	// Pass 6 : Effect_Animation
	pass Effect_6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	// Pass 7 : Alpha
	pass Alpha_7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLENDING();
	}

	pass Discard //8 : 성준 
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISCARD();
	}

	pass Dissolve //9 : 성준 
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass BlendingTime //10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLENDINGTIME();
	}
}