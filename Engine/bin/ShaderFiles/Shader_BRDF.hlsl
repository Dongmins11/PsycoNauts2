#define EPSILON 0.000000000001f
#define PI	3.1452f
#define SAMPLE_COUNT 1024
#define MAX_REFLECTION_LOD 7
//<<Non-IBL>>

///////////////////////////////////////////
//<DiffuseBRDF>
////////////////////////////////////////////

//Fresnel Reflectance
float3 Fresnel_Reflectance(float3 vFdielectric, float3 vDiffuse, float fMetalness)
{
	return lerp(vFdielectric, vDiffuse, fMetalness);
}

//Fresnel
float3 Fresnel(float3 vNormal, float3 vView, float3 vFresnel_Reflectance)
{
	return vFresnel_Reflectance + (float3(1.f, 1.f, 1.f) - vFresnel_Reflectance)*pow(1.f - max(0.f, dot(vNormal, vView)), 5.f);
}

//Fresnel LanbertDiffuse
float3 Fresnel_LambertDiffuse(float3 fKd)
{
	return fKd / PI;
}
//////////////////////////////////////////
//<SpecularBRDF>
///////////////////////////////////////////

//Trowbridge-Reitz GGX Distribution : D
float NormalDistributionGGX(float3 vNormal, float3 vHalfVector, float fRoughness)
{
	float fAlpha = fRoughness * fRoughness;
	float fAlphaSq = fAlpha * fAlpha;

	float fNDotH2 = pow(max(dot(vNormal, vHalfVector), 0.f), 2.f);

	float fDenom = (PI*pow((fNDotH2*(fAlphaSq - 1.f) + 1.f), 2.f));


	if (fDenom < EPSILON)
		return 1.f;
	else
		return fAlphaSq / fDenom;

}
//Smith's Schlick-GGX for Direct Lighting
float Geometry_Smiths_SchlickGGX(float3 vNormal, float3 vVector, float fRoughness)
{
	float fK = pow((fRoughness + 1.f), 2.f) / 8.f;
	float fNV = max(0.f, dot(vNormal, vVector));
	float fDenom = (fNV*(1.f - fK) + fK) + 0.0001f;

	return fNV / fDenom;
}

//Geometry Mul : G 
float Geometry(float3 vNormal, float3 vViewVector, float3 vLightDir, float fRoughness)
{
	return Geometry_Smiths_SchlickGGX(vNormal, vViewVector, fRoughness)*Geometry_Smiths_SchlickGGX(vNormal, vLightDir, fRoughness);
}

//////////////////////////////////////////////////
//<BRDF>
//////////////////////////////////////////////////

float3 BRDF(float3 vWi, float3 vDiffuse, float3 vSpecularColor, float fRoughness, float fMetalness, float3 vNormal, float3 vView, float3 vWorldPos)
{
	float3 vWo = normalize(vView);
	float3 vHalfVector = normalize(vWo + vWi);

	float3 vF0 = Fresnel_Reflectance(float3(0.04f, 0.04f, 0.04f), vDiffuse, fMetalness);

	//Cook-Torrence BRDF

	float3 F = Fresnel(vHalfVector, vView, vF0);
	float G = Geometry(vNormal, vWo, vWi, fRoughness);
	float D = NormalDistributionGGX(vNormal, vHalfVector, fRoughness);
	float fDenom = (4.0f * max(0.0f, dot(vWo, vNormal)) * max(0.0f, dot(vWi, vNormal))) + 0.0001f;
	float3 vSpecular = F*G*D / fDenom;
	float3 vSpecularBRDF = vSpecular*  vSpecularColor;

	float3 vKS = F;
	float3 vKD = (float3(1, 1, 1) - vKS) * (1.0f - fMetalness) * vDiffuse;
	const float3 vDiffuseBRDF = Fresnel_LambertDiffuse(vKD);

	return (vDiffuseBRDF + vSpecularBRDF);
}

//<<IBL>>

//////////////////
//<Diffuse>
////////////////

//Fresnel For Irradiance
float3 FresnelWithRoughness(float fCosTheta, float3 vFresnel_Reflectance, float fRoughness)
{
	return vFresnel_Reflectance + (max((1.f - fRoughness).xxx, vFresnel_Reflectance) - vFresnel_Reflectance)*pow(1.f - fCosTheta, 5.f);
}

////////////////
//<Specular>
////////////////

inline float Geometry_Smiths_SchlickGGX_EnvironmentMap(float3 N, float3 V, float roughness)
{ // describes self shadowing of geometry
  //
  // G_ShclickGGX(N, V, k) = ( dot(N,V) ) / ( dot(N,V)*(1-k) + k )
  //
  // k		 :	remapping of roughness based on wheter we're using geometry function 
  //				for direct lighting or IBL
  // k_direct	 = (roughness + 1)^2 / 8
  // k_IBL	 = roughness^2 / 2
  //
	const float k = pow(roughness, 2) / 2.0f;
	const float NV = max(0.0f, dot(N, V));
	const float denom = (NV * (1.0f - k) + k) + 0.0001f;
	//if (denom < EPSILON) return 1.0f;
	return NV / denom;
}

//IBL : G 

float GeometryEnvironmentMap(float3 N, float3 V, float3 L, float k)
{ // essentially a multiplier [0, 1] measuring microfacet shadowing
	float geomNV = Geometry_Smiths_SchlickGGX_EnvironmentMap(N, V, k);
	float geomNL = Geometry_Smiths_SchlickGGX_EnvironmentMap(N, L, k);
	return geomNV * geomNL;
}


//////////////////////////////////////////////////
//<BRDF-IBL>
//////////////////////////////////////////////////
float3 EnvironmentBRDF(float3 vDiffuse, float fRoughness, float fMetalness, float3 vNormal, float3 V, float ao, float3 irradience, float3 envSpecular, float2 F0ScaleBias)
{
	const float3 F0 = lerp(0.04f.xxx, vDiffuse, fMetalness);

	const float3 Ks = FresnelWithRoughness(max(dot(vNormal, V), 0.0), F0, fRoughness);
	const float3 Kd = (1.0f - Ks) * (1.0f - fMetalness);

	const float3 diffuse = irradience * vDiffuse;
	const float3 specular = envSpecular * (Ks * F0ScaleBias.x + F0ScaleBias.y);

	return (Kd * diffuse + specular) * ao;
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
	const float a = roughness * roughness;

	const float phi = 2.0f * PI * Xi.x;
	const float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
	const float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	// from sphreical coords to cartesian coords
	float3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space to world space
	const float3 up = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	const float3 tangent = normalize(cross(up, N));
	const float3 bitangent = cross(N, tangent);

	const float3 Sample = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(Sample);
}

float RadicalInverse_VdC(uint bits) //  Van Der Corpus
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(int i, int count)
{
	return float2(float(i) / float(count), RadicalInverse_VdC(uint(i)));
}

//SpecularBRDF-IBL(BRDF_LUT)
float2 IntegrateBRDF(float NdotV, float roughness)
{
	float3 V;
	V.x = sqrt(1.0f - NdotV * NdotV);
	V.y = 0;
	V.z = NdotV;

	float F0Scale = 0;	// Integral1
	float F0Bias = 0;	// Integral2

	const float3 N = float3(0.f, 0.f, 1.f);
	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		const float2 Xi = Hammersley(i, SAMPLE_COUNT);
		const float3 H = ImportanceSampleGGX(Xi, N, roughness);
		const float3 L = normalize(reflect(-V, H));

		const float NdotL = max(L.z, 0.f);
		const float NdotH = max(H.z, 0.f);
		const float VdotH = max(dot(V, H), 0.f);

		if (NdotL > 0.0f)
		{
			const float G = GeometryEnvironmentMap(N, V, L, roughness);
			const float G_Vis = (G * VdotH) / ((NdotH * NdotV) + 0.0001);
			const float Fc = pow(1.0f - VdotH, 5.0f);

			F0Scale += (1.0f - Fc) * G_Vis;
			F0Bias += Fc * G_Vis;
		}
	}
	return float2(F0Scale, F0Bias) / SAMPLE_COUNT;
}

float2 invAtan = float2(0.1591f, 0.3183f);
float2 SampleSphericalMap(float3 v)
{
	float2 uv = float2(atan2(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5f;
	return uv;
}
float4 CreateIrradiance(float3 vLocalPos, textureCUBE HDRTexture, sampler Sampler)
{
	float3 normal = normalize(vLocalPos);
	float3 irradiance = float3(0.0f, 0.0f, 0.0f);

	float3 up = float3(0.0, 1.0, 0.0);
	float3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
			//float2 uv = SampleSphericalMap(sampleVec);
			irradiance += HDRTexture.Sample(Sampler, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	return float4(irradiance, 1.0);
}

////Create PreFilter
//float4  CreatePreFilter(float3 vLocalPos, textureCUBE HDRTexture, sampler Sampler, float fRoughness)
//{
////float roughness = fRoughness;
////float3 N = normalize(vLocalPos);
////float3 R = N;
////float3 V = R;
////
////uint SAMPLECOUNT = 1024;
////float totalWeight = 0.0;
////float3 prefilteredColor = float3(0.0, 0.0, 0.0);
////for (uint i = 0u; i < SAMPLECOUNT; ++i)
////{
////	float2 Xi = Hammersley(i, SAMPLECOUNT);
////	float3 H = ImportanceSampleGGX(Xi, N, roughness);
////	float3 L = normalize(2.0 * dot(V, H) * H - V);
////
////	float NdotL = max(dot(N, L), 0.0);
////	if (NdotL > 0.0)
////	{
////		prefilteredColor += HDRTexture.Sample(Sampler, L).rgb * NdotL;
////		totalWeight += NdotL;
////	}
////}
////prefilteredColor = prefilteredColor / totalWeight;
////
////return float4(prefilteredColor, 1.0);
//
//	float3 normalVec = -normalize(vLocalPos);
//	float3 R = normalVec;
//	float3 viewDir = R;
//
//	float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
//	float totalWeight = 0.0f;
//
//	const uint NumSamples = 1024;
//	for (uint i = 0; i < NumSamples; i++)
//	{
//		float2 Xi = Hammersley(i, NumSamples);
//		float3 halfwayVec = ImportanceSampleGGX(Xi, fRoughness, normalVec);
//		float3 lightDir = 2 * dot(viewDir, halfwayVec) * halfwayVec - viewDir;
//		float NdotL = saturate(dot(normalVec, lightDir));
//		if (NdotL > 0)
//		{
//			// sample from the environment's mip level based on roughness/pdf
//			float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness);
//			float NdotH = max(dot(normalVec, halfwayVec), 0.0f);
//			float HdotV = max(dot(halfwayVec, viewDir), 0.0f);
//			float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;
//
//			float resolution = 512.0f; // resolution of source cubemap (per face)
//			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
//			float saSample = 1.0f / (float(NumSamples) * pdf + 0.0001f);
//
//			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
//
//			PrefilteredColor += HDRTexture.SampleLevel(Sampler, lightDir, mipLevel).rgb * NdotL;
//			totalWeight += NdotL;
//		}
//	}
//	PrefilteredColor /= totalWeight;
//
//	return float4(PrefilteredColor, 1.0f);
//}

//Create PreFilter
float4  CreatePreFilter(float3 vLocalPos, textureCUBE HDRTexture, sampler Sampler, float fRoughness)
{


	float3 normalVec = -normalize(vLocalPos);
	float3 R = normalVec;
	float3 viewDir = R;

	float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
	float totalWeight = 0.0f;

	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 halfwayVec = normalize(ImportanceSampleGGX(Xi, normalVec, max(0.04f, fRoughness)));
		float3 lightDir = normalize(2 * dot(viewDir, halfwayVec) * halfwayVec - viewDir);
		float NdotL = max(dot(normalVec, lightDir), 0.5f);
		float NdotH = max(dot(normalVec, halfwayVec), 0.5f);
		float HdotV = max(dot(halfwayVec, viewDir), 0.5f);
		if (NdotL > 0)
		{
			// sample from the environment's mip level based on roughness/pdf
			float D = NormalDistributionGGX(normalVec, halfwayVec, fRoughness);


			float pdf = D * NdotH / (4.0f * HdotV + 0.00001f) + 0.0001f;

			float resolution = 1280.0f; // resolution of source cubemap (per face)
			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
			float saSample = 1.0f / (float(NumSamples) * pdf + 0.000001f);

			float mipLevel = fRoughness == 0.0 ? 0.0 : log2(saSample / saTexel);

			PrefilteredColor += HDRTexture.SampleLevel(Sampler, lightDir, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	PrefilteredColor /= max(totalWeight, 0.0001f);

	return float4(PrefilteredColor, 1.0f);
}

float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosH, float3 wPosition)
{
	shadowPosH.xyz /= shadowPosH.w;

	float depth = shadowPosH.z;

	const float dx = 1.f / 1280.f;

	float percentLit = 0.f;
	const float2 offsets[9] =
	{
		float2(-dx,-dx),float2(0.f,-dx),float2(dx,-dx),
		float2(-dx,0.f),float2(0.f,0.f),float2(dx,0.f),
		float2(-dx,dx),float2(0.f,dx),float2(dx,+dx),
	};

	for (uint i = 0; i < 9; ++i)
	{
		percentLit = shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy + offsets[i], depth).r;

	}

	percentLit /= 9.f;

	float range = length(float3(0.f, 0.f, 0.f) - wPosition);
	if (range > 100)
		percentLit = 1.f;

	return percentLit;
}

float SpotlightIntensity(float3 vLightPos, float3 vLightDir, float fOuterConeAngle, float fInnerConeAngle, float3 worldPos)
{
	float3 pixelDirectionInWorldSpace = normalize(worldPos - vLightPos);
	float3 spotDir = normalize(vLightDir);
#if 1
	float theta = 0.f;
	theta = acos(dot(pixelDirectionInWorldSpace, spotDir));

	if (theta >  fOuterConeAngle)	return 0.0f;
	if (theta <= fInnerConeAngle)	return 1.0f;
	return 1.0f - (theta - fInnerConeAngle) / (fOuterConeAngle - fInnerConeAngle);
#else
	if (dot(spotDir, pixelDirectionInWorldSpace) < cos(fOuterConeAngle))
		return 0.0f;
	else
		return 1.0f;
#endif
}

half ConvertZToLinearDepth(half depth)
{
	float near = 0.1f;
	float far = 300.f;

	return (near * far) / (far - depth * (far - near));

}

float BlurFactor(in float depth, float4 g_vDofParam)
{
	float f0 = 1.0f - saturate((depth - g_vDofParam.x) / max(g_vDofParam.y - g_vDofParam.x, 0.01f));
	float f1 = saturate((depth - g_vDofParam.z) / max(g_vDofParam.w - g_vDofParam.z, 0.01f));
	float blur = saturate(f0 + f1);

	return blur;
}

float3 DistanceDOF(float3 colorFocus, float3 colorBlurred, float depth, float4 g_vDofParam)
{
	half blurFactor = BlurFactor(depth, g_vDofParam);

	return lerp(colorFocus, colorBlurred, blurFactor);
}

float4 DOF(float4 vOutcolor, Texture2D BlurTexture, SamplerState st, float2 vTexUV, float fDepth, float4 g_vDofParam)
{
	float4 vColor = vOutcolor;

	// 흐리게 축소된 HDR 텍스처에서 흐릿한 색상을 가져옵니다.
	float3 vSceneBlurred = BlurTexture.Sample(st, vTexUV).xyz;

	// 전체 해상도 깊이를 선형 깊이로 변환
	fDepth = ConvertZToLinearDepth(fDepth);

	// 거리 DOF 색상 계산
	vColor = float4(DistanceDOF(vColor.xyz, vSceneBlurred, fDepth, g_vDofParam), vColor.a);

	return vColor;
}

float fPixelKernel[13] =
{
	-6,
	-5,
	-4,
	-3,
	-2,
	-1,
	0,
	1,
	2,
	3,
	4,
	5,
	6,
};

float fBlurWeights[13] =
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

float fBlurWeights_Test[13] =
{
	0.0561f,
	0.1353f,
	0.278f,
	0.4868f,
	0.7261f,
	0.9231f,
	1.f,
	0.9231f,
	0.7261f,
	0.4868f,
	0.278f,
	0.1353f,
	0.0561f
};

float fTotalWeight_Test = 6.2108f;
