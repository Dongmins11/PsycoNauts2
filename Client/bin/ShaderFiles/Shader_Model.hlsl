#include "Shader_Defines.hpp"
#include "Shader_BRDF.hlsl"
cbuffer Matrices
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
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
	float4x4	g_LightViewMatrix;
	float4x4	g_LightProjectionMatrix;
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

cbuffer OldMatrices
{
	float4x4 g_OldWorldMatrix;
	float4x4 g_OldViewMatrix;
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
cbuffer Dissovle
{
	float g_fDissolveTime = 0.f;
	bool  g_bDissolve = false;
	float g_fDissolveStart = 0.1f;
};

cbuffer DiffuseUV
{
	float g_fUVTimeAcc = 0.f;
	float g_fBlue = 0.3f;
};
cbuffer Offset
{
	float3 g_vOffsetRGBA;
};

cbuffer BlendingTimer
{
	float g_fBlendingTime = 0.f;
};
cbuffer Emissive
{
	float g_fEmissiveRatio = 1.f;
};
cbuffer Reflect

{
	float4x4 g_ReflectMatrix;
	float	 g_fReflectRatio = 0.1f;
};
texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_ColorLUT;
texture2D			g_RomTexture;
texture2D			g_ShadowTexture;
texture2D			g_TestTexture;
texture2D			g_EmissiveTexture;
texture2D			g_DissolveTexture;

texture2D			g_LightImgTexture;
texture2D			g_ReflectTexture;
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
struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};
struct VS_IN_SHADOWMAP
{
	float3		vPosition : POSITION;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vWorldPosition :POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPosition :TEXCOORD2;
	float4		vClipPosition: TEXCOORD3;
	float4		vDir: TEXCOORD4;


};
struct VS_OUT_REFLECT
{
	float4		vPosition : SV_POSITION;
	float4		vWorldPosition :POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPosition :TEXCOORD2;
	float4		vClipPosition: TEXCOORD3;
	float4		vDir: TEXCOORD4;
	float4		vReflectionPosition: TEXCOORD5;


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

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vLocalPosition = vector(In.vPosition, 1.f);
	Out.vWorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

	//if (true == g_bIsNormalMapping)
	//	Out.vNormal.xyz *= -1.f;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	//if (true == g_bIsNormalMapping)
	//	Out.vTangent.xyz *= -1.f;
	Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

	//Shadow
	if (true == g_bShadow)
	{
		Out.vClipPosition = mul(Out.vWorldPosition, g_LightViewMatrix);
		Out.vClipPosition = mul(Out.vClipPosition, g_ProjMatrix);
	}
	else
	{
		Out.vClipPosition = vector(0.f, 0.f, 0.f, 0.f);
	}

	//Velocity
	if (true == g_bMotionBlur)
	{
		float4 vNewPos = Out.vPosition;
		float4 vOldPos = mul(vector(In.vPosition, 1.f), g_OldWorldMatrix);
		vOldPos = mul(vOldPos, g_OldViewMatrix);
		vOldPos = mul(vOldPos, g_ProjMatrix);

		float3 vDir = vNewPos.xyz - vOldPos.xyz;

		float4 WorldNewPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
		float4 WorldOldPos = mul(vector(In.vPosition, 1.f), g_OldWorldMatrix);
		float4 WorldNormal = Out.vNormal;

		float3 vWorldDir = WorldNewPos.xyz - WorldOldPos.xyz;

		matrix NormalMatrix = matWVP;
		NormalMatrix._11 = 1.0f; NormalMatrix._22 = 1.0f; NormalMatrix._33 = 1.0f;
		NormalMatrix._41 = 0.0f; NormalMatrix._42 = 0.0f; NormalMatrix._43 = 0.0f;


		float4 vNormal = mul(WorldNormal, g_ViewMatrix);

		float fDot = dot(normalize(vWorldDir), normalize(WorldNormal.xyz));
		if (fDot < 0.f)
			Out.vPosition = vOldPos;
		else
			Out.vPosition = vNewPos;


		float2 vVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
		Out.vDir.xy = vVelocity*0.5f;

		Out.vDir.y *= -1.f;
		Out.vDir.z = Out.vPosition.z;
		Out.vDir.w = Out.vPosition.w;

		if (length(g_vCamPosition - Out.vWorldPosition) < 3.f)
			Out.vDir.xy = 0.f;
	}
	else
	{
		Out.vDir = vector(0.f, 0.f, 0.f, 0.f);
	}
	return Out;
}
VS_OUT_REFLECT VS_MAIN_REFLECT(VS_IN In)
{
	VS_OUT_REFLECT		Out = (VS_OUT_REFLECT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vLocalPosition = vector(In.vPosition, 1.f);
	Out.vWorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	//if (true == g_bIsNormalMapping)
	//	Out.vNormal.xyz *= -1.f;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	//if (true == g_bIsNormalMapping)
	//	Out.vTangent.xyz *= -1.f;
	Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

	//Shadow
	if (true == g_bShadow)
	{
		Out.vClipPosition = mul(Out.vWorldPosition, g_LightViewMatrix);
		Out.vClipPosition = mul(Out.vClipPosition, g_ProjMatrix);
	}
	else
	{
		Out.vClipPosition = vector(0.f, 0.f, 0.f, 0.f);
	}

	//Velocity
	if (true == g_bMotionBlur)
	{
		float4 vNewPos = Out.vPosition;
		float4 vOldPos = mul(vector(In.vPosition, 1.f), g_OldWorldMatrix);
		vOldPos = mul(vOldPos, g_OldViewMatrix);
		vOldPos = mul(vOldPos, g_ProjMatrix);

		float3 vDir = vNewPos.xyz - vOldPos.xyz;

		float4 WorldNewPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
		float4 WorldOldPos = mul(vector(In.vPosition, 1.f), g_OldWorldMatrix);
		float4 WorldNormal = Out.vNormal;

		float3 vWorldDir = WorldNewPos.xyz - WorldOldPos.xyz;

		matrix NormalMatrix = matWVP;
		NormalMatrix._11 = 1.0f; NormalMatrix._22 = 1.0f; NormalMatrix._33 = 1.0f;
		NormalMatrix._41 = 0.0f; NormalMatrix._42 = 0.0f; NormalMatrix._43 = 0.0f;


		float4 vNormal = mul(WorldNormal, g_ViewMatrix);

		float fDot = dot(normalize(vWorldDir), normalize(WorldNormal.xyz));
		if (fDot < 0.f)
			Out.vPosition = vOldPos;
		else
			Out.vPosition = vNewPos;


		float2 vVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
		Out.vDir.xy = vVelocity*0.5f;

		Out.vDir.y *= -1.f;
		Out.vDir.z = Out.vPosition.z;
		Out.vDir.w = Out.vPosition.w;

		if (length(g_vCamPosition - Out.vWorldPosition) < 3.f)
			Out.vDir.xy = 0.f;
	}
	else
	{
		Out.vDir = vector(0.f, 0.f, 0.f, 0.f);
	}
	float4x4 reflectmatrix = mul(g_ReflectMatrix, g_ProjMatrix);
	reflectmatrix = mul(g_WorldMatrix, reflectmatrix);
	Out.vReflectionPosition = mul(vector(In.vPosition, 1.f), reflectmatrix);
	return Out;
}
VS_OUT_SHADOWMAP VS_MAIN_SHADOWMAP(VS_IN_SHADOWMAP In)
{
	VS_OUT_SHADOWMAP		Out = (VS_OUT_SHADOWMAP)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_LightViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	float4 vWolrdPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vClipPosition = mul(vWolrdPos, g_LightViewMatrix);
	Out.vClipPosition = mul(Out.vClipPosition, g_ProjMatrix);
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vWorldPosition :POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPosition :TEXCOORD2;
	float4		vClipPosition: TEXCOORD3;
	float4		vDir: TEXCOORD4;

};
struct PS_IN_REFLECT
{
	float4		vPosition : SV_POSITION;
	float4		vWorldPosition :POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPosition :TEXCOORD2;
	float4		vClipPosition: TEXCOORD3;
	float4		vDir: TEXCOORD4;
	float4		vReflectionPosition: TEXCOORD5;
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

struct PS_SKYOUT
{
	float4		vColor : SV_TARGET0;
};
struct PS_OUT_EFFECT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vEmissive : SV_TARGET1;
};
struct PS_OUT_SHADOWMAP
{
	float4		vShadowMap : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//if (Out.vDiffuse.a < 0.3f)
	//	discard;

	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	if (true == g_bEmissive)
	{
		Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
		Out.vEmissive.rgb *= g_fEmissiveRatio;


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
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);

	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);


	if (0.5f > vNormalDesc.z)
	{
		vNormalDesc.z = sqrt(1 - ((vNormalDesc.x * vNormalDesc.x) + (vNormalDesc.y * vNormalDesc.y)))*2.f - 1.f;
		vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
		vNormalDesc.y = -vNormalDesc.y*2.f + 1.f;
		//vNormalDesc.z = 1.f;
	}
	else
	{
		vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
		vNormalDesc.y = vNormalDesc.y*2.f - 1.f;
		vNormalDesc.z = vNormalDesc.z*2.f - 1.f;
	}

	float3	vNormal = normalize(vNormalDesc.xyz);

	float3x3		WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
	WorldMatrix = transpose(WorldMatrix);

	vNormal = mul(WorldMatrix, vNormal);

	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);


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

		if(false == g_bIsNormalMapping)
			Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}
	else
	{
		Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}

	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = 1.f;
		Out.vVelocity.w = In.vDir.z / In.vDir.w;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse.rgb += g_vOffsetRGBA;
	return Out;
}
PS_OUT PS_MAIN_REFLECT(PS_IN_REFLECT In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//if (Out.vDiffuse.a < 0.3f)
	//	discard;

	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	if (true == g_bEmissive)
	{
		Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV)*g_fEmissiveRatio;
		if (true == g_bDissolve)
		{
			vector vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

			if (vDissolve.r - g_fDissolveTime <= 0.0f)
			{
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);
				discard;
			}
			else if (vDissolve.r - g_fDissolveTime <= g_fDissolveStart)
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

			if (vDissolve.r - g_fDissolveTime <= 0.0f)
			{
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);
				discard;
			}
		}
		Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	}
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);

	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	if (0.f == vNormalDesc.z)
	{
		vNormalDesc.z = sqrt(1 - ((vNormalDesc.x * vNormalDesc.x) + (vNormalDesc.y * vNormalDesc.y)))*2.f - 1.f;
		vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
		vNormalDesc.y = -vNormalDesc.y*2.f + 1.f;
		//vNormalDesc.z = 1.f;

		//vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
		//vNormalDesc.y = vNormalDesc.y*2.f - 1.f;
		//vNormalDesc.z = vNormalDesc.z*2.f - 1.f;

	}

	else
	{
		vNormalDesc.x = vNormalDesc.x*2.f - 1.f;
		vNormalDesc.y = vNormalDesc.y*2.f - 1.f;
		vNormalDesc.z = vNormalDesc.z*2.f - 1.f;
	}

	float3	vNormal = normalize(vNormalDesc.xyz);

	float3x3		WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
	WorldMatrix = transpose(WorldMatrix);

	vNormal = mul(WorldMatrix, vNormal);

	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);


	if (true == g_bRimlight)
	{
		vector vResult;

		vector		vLook = g_vCamPosition - In.vWorldPosition;
		//vector		vLook = g_vLookCam;
		float		fFresnel = pow(1.f - max(0.f, dot(normalize(float4(vNormal, 0.f)), normalize(vLook))), g_fPower);
		vector		vReflect = reflect(normalize(g_vLightDir), float4(vNormal, 0.f));
		vector		vT1 = vector(0.f, 1.f, 0.f, 0.f);
		vector		vT = vector(0.f, 0.25f, 0.f, 0.f);
		float		fL = max(0.f, dot(normalize(vT1), normalize(float4(vNormal, 0.f))));
		float		fM = max(0.f, (1.f - max(0.f, dot(normalize(vReflect), vT))) - pow(fL, 0.8));

		vResult = g_vRGBA* fFresnel* fM;
		Out.vRimlight = vResult / g_vRimRatio.x;
		Out.vDiffuse = float4(Out.vDiffuse.rgb / g_vRimRatio.y, Out.vDiffuse.a);

		//vResult = float4(vResult.rgb / g_vRimRatio.x + Out.vDiffuse.rgb / g_vRimRatio.y, Out.vDiffuse.a);

		//float3 vLo = normalize(g_vCamPosition.xyz - In.vWorldPosition.xyz);
		//float3 vLightInverse = -g_vLightDir.xyz;
		//float3 vLh = normalize((normalize(vLightInverse) + normalize(vLo)));

		//float3      vF = Fresnel(normalize(float4(vNormal, 0.f)), vLo, 0.04f);
	}
	else
	{
		Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}

	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = 1.f;
		Out.vVelocity.w = In.vDir.z / In.vDir.w;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);

	float2 vReflectTexCoord = float2(0.f, 0.f);
	vReflectTexCoord.x = saturate((In.vReflectionPosition.x / In.vReflectionPosition.w) / 2.f + 0.5f);
	vReflectTexCoord.y = saturate(-(In.vReflectionPosition.y / In.vReflectionPosition.w) / 2.f + 0.5f);

	float4 vReflectColor = float4(0.f, 0.f, 0.f, 0.f);
	vReflectColor = g_ReflectTexture.Sample(LinearSamplerClamp, vReflectTexCoord);

	Out.vDiffuse = lerp(Out.vDiffuse, vReflectColor, g_fReflectRatio);
	return Out;
}
PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float2 ColorUV = float2(vDiffuse.b, 0.f);

	//	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//	float2 ColorUV = float2(Out.vDiffuse.r, 0.f);

	vector	vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	vector vColor = g_ColorLUT.Sample(LinearSampler, ColorUV);
	Out.vDiffuse = vColor;//g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);


	//Out.vColor.a = 0.8f;
	//Out.vColor.r = 1.f;
	//Out.vColor.gb = 0.f;

	//if (Out.vColor.r == 0.f && Out.vColor.g == 0.f && Out.vColor.b == 0.f)
	//	discard;


	if (Out.vDiffuse.a < 0.3f)
		discard;

	float3x3		WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN3(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	if (Out.vDiffuse.a < 0.3f)
		discard;
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse.bg = 0;

	float3x3		WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_SKYOUT PS_MAIN_MAP(PS_IN In)
{
	PS_SKYOUT        Out = (PS_SKYOUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector    vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	float3    vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3        WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vColor = vDiffuse;
	Out.vColor.a = 1.f;
	return Out;
}

PS_OUT PS_PREVIEW(PS_IN In)
{
	PS_OUT        Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);


	vector    vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
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

PS_SKYOUT PS_MAIN_TEST(PS_IN In)
{
	PS_SKYOUT        Out = (PS_SKYOUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_UVMOVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vDiffuseUV = In.vTexUV;
	vDiffuseUV.x += g_fUVTimeAcc;

	Out.vDiffuse = g_LightImgTexture.Sample(LinearSampler, vDiffuseUV);
	//if (Out.vDiffuse.a < 0.3f)
	//	discard;

	//ShadowMap
	if (true == g_bShadow)
		Out.vDepthShadow = In.vClipPosition;
	else
	{
		Out.vDepthShadow = vector(0.f, 0.f, 0.f, 0.f);
	}
	if (true == g_bEmissive)
	{
		float4 vEmissive = Out.vDiffuse;
		vEmissive.b *= g_fBlue;

		Out.vEmissive = vEmissive;
		if (true == g_bDissolve)
		{
			vector vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

			if (vDissolve.r - g_fDissolveTime <= 0.0f)
			{
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);
				discard;
			}
			else if (vDissolve.r - g_fDissolveTime <= g_fDissolveStart)
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

			if (vDissolve.r - g_fDissolveTime <= 0.0f)
			{
				Out.vDepthShadow = vector(1.f, 1.f, 1.f, 1.f);
				discard;
			}
		}
		Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	}
	Out.vRom = g_RomTexture.Sample(LinearSampler, In.vTexUV);

	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3		WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(normalize(vNormal), WorldMatrix);

	if (true == g_bIsNormalMapping)
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	else
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);


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

		if (false == g_bIsNormalMapping)
			Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}
	else
	{
		Out.vRimlight = vector(0.f, 0.f, 0.f, 0.f);
	}

	//Velocity
	if (true == g_bMotionBlur)
	{
		Out.vVelocity.xy = In.vDir.xy;
		Out.vVelocity.z = 1.f;
		Out.vVelocity.w = In.vDir.z / In.vDir.w;
	}
	else
		Out.vVelocity = vector(0.f, 0.f, 0.f, 0.f);


	return Out;
}

PS_OUT_EFFECT PS_MAIN_ALPHABLENDING(PS_IN In)
{
	PS_OUT_EFFECT        Out = (PS_OUT_EFFECT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	if (true == g_bEmissive)
		Out.vEmissive = Out.vDiffuse;

	return Out;
}

PS_OUT PS_MAIN_DISCARD(PS_IN In)
{
	PS_OUT        Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	discard;

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
	// Pass 0 : 기본
	pass Default_0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

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

	// Pass 0 : 기본
	pass Test_2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEST();
	}

	// Pass 3 : 미정
	pass Default_3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// Pass 4 : Tool 
	pass Tool_Check_4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN3();
	}


	//Pass 5 : Tool Preview->Tool에서 미리 보기 기능
	pass PREVIEW_5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PREVIEW();
	}
	// Pass 6 : SkyBox 
	pass SkyBox_6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Priority, 0);
		SetRasterizerState(Rasterizer_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAP();
	}
	// Pass 7 : UV_Move
	pass Default_7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UVMOVE();
	}
	// Pass 8 : 기본
	pass Blending_8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHABLENDING();
	}
	pass Discard_9 //9 : 성준 
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISCARD();
	}

	pass BlendingTime_10 //10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLENDINGTIME();
	}

	pass Reflect_Dest_11 //11
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_REFLECT();
	}

	// Pass 0 : 기본
	pass Reflect_Sour_12
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DepthStencil_Default, 0);
		SetRasterizerState(Rasterizer_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}