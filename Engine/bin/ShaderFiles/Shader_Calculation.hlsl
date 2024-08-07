/*For. ToneMapping*/
static const float gamma = 2.2;

float3 ACESFilmicToneMapping(float3 color)
{
	color *= 0.6f;
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);
}

float3 HableToneMapping(float3 color)
{
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	float W = 11.2f;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	return color;
}

/*For.Reinhard_Extended_Luminance */
float Luminance(float3 vColor)
{
	return dot(vColor, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 Change_Luminance(float3 vColor_In, float fLuminance_Out)
{
	float fLuminance_In = Luminance(vColor_In);
	return vColor_In*(fLuminance_Out / fLuminance_In);
}

float3 Reinhard_Extended_Luminance(float3 vColor, float fMax_White_Luminance)
{
	float fLuminance_Old = Luminance(vColor);
	float fNumerator = fLuminance_Old*(1.f + (fLuminance_Old / (fMax_White_Luminance*fMax_White_Luminance)));
	float fLuminance_New = fNumerator / (1.f + fLuminance_Old);
	
	return Change_Luminance(vColor, fLuminance_New);
}

/*For. ACES */

static const float3x3 Aces_input_Matrix =
{
	float3(0.59719f,0.35458f,0.04823f),
	float3(0.07600f,0.90834f,0.01566f),
	float3(0.02840f,0.13383f,0.83777f)
};

static const float3x3 Aces_output_Matrix =
{
	float3(1.60475f,-0.53108f,-0.07367f),
	float3(-0.10208f,1.10813f,-0.00605f),
	float3(-0.00327f,-0.07276f,1.07602f)
};

float3 Mul(float3x3 M, float3 V)
{
	float fX = M[0][0] * V[0] + M[0][1] * V[1] + M[0][2] * V[2];
	float fY = M[1][0] * V[1] + M[1][1] * V[1] + M[1][2] * V[2];
	float fZ = M[2][0] * V[1] + M[2][1] * V[1] + M[2][2] * V[2];

	return float3(fX, fY, fZ);
}

float3 Rtt_and_odt_fit(float3 vVector)
{
	float3 a = vVector * (vVector + 0.0245786f) - 0.000090537f;
	float3 b = vVector * (0.983729f * vVector + 0.4329510f) + 0.238081f;
	return a / b;
}

float3 Aces_fitted(float3 vVector)
{
	vVector = Mul(Aces_input_Matrix, vVector);
	vVector = Rtt_and_odt_fit(vVector);

	return saturate(Mul(Aces_output_Matrix, vVector));
}

/*For. Hable */

float3 Uncharted2_tonemap_partial(float3 vVector)
{
	float fA = 0.15f;
	float fB = 0.50f;
	float fC = 0.10f;
	float fD = 0.20f;
	float fE = 0.02f;
	float fF = 0.30f;

	return ((vVector*(fA*vVector + fC*fB) + fD*fE) / (vVector*(fA*vVector + fB) + fD*fF)) - fE / fF;
}

float3 Uncharted2_filmic(float3 vVector)
{
	float fExposure_bias = 2.f;
	float3 fCurr = Uncharted2_tonemap_partial(vVector*fExposure_bias);

	float3 vW = float3(11.2f, 11.2f, 11.2f);
	float3 fWhite_scale = float3(1.f,1.f,1.f) / Uncharted2_tonemap_partial(vW);

	return fCurr* fWhite_scale;
}

/*For.SSAO*/

half3 RayRandom[16] =
{
	half3(0.2024537f, 0.841204f, -0.9060141f)
	, half3(-0.2200423f, 0.6282339f, -0.8275437f)
	, half3(0.3677659f, 0.1086345f, -0.4466777f)
	, half3(0.8775856f, 0.4617546f, -0.6427765f)
	, half3(0.7867433f, -0.141479f, -0.1567597f)
	, half3(0.4839356f, -0.8253108f, -0.1563844f)
	, half3(0.4401554f, -0.4228428f, -0.3300118f)
	, half3(0.0019193f, -0.8048455f, 0.0726584f)
	, half3(-0.7578573f, -0.5583301f, 0.2347527f)
	, half3(-0.4540417f, -0.252664f, 0.0694318f)
	, half3(-0.0483353f, -0.2527294f, 0.5924745f)
	, half3(-0.4192392f, 0.2084218f, -0.3672943f)
	, half3(-0.8433938f, 0.1451271f, 0.2202872f)
	, half3(-0.4037157f, -0.8263387f, 0.4698132f)
	, half3(-0.6657394f, 0.6298575f, 0.6342437f)
	, half3(-0.001783f, 0.2834622f, 0.8343929f)
};


float3 RandomNormal(float2 vUV)
{
	float fNoiseX = (frac(sin(dot(vUV, float2(15.8989f, 76.132f)*1.f))*46336.23745f));
	float fNoiseY = (frac(sin(dot(vUV, float2(11.9899f, 62.223f)*2.f))*34748.34744f));
	float fNoiseZ = (frac(sin(dot(vUV, float2(13.3238f,63.122f)*3.f))*59998.47362f));

	return normalize(float3(fNoiseX, fNoiseY, fNoiseZ));


}

float4 Get_SSAO(float2 vUV, float fDepth, float fViewZ, float3 vNormal,texture2D DepthTex,SamplerState St,float fRadious, float fBias, int iCount)
{
	float3 vRay;
	float3 vReflect;
	float2 vRandomUV;
	float fOccNorm;


	float fFar = 300.f;
	float iColor = 0;
	float4 vOut = float4(0.f, 0.f, 0.f, 1.f);
	for (int i = 0; i < iCount; ++i)
	{
		vRay = reflect(RandomNormal(vUV), RayRandom[i]);
		vReflect = normalize(reflect(vRay, vNormal))*fRadious;
		vReflect.x *= -1.f;
		vRandomUV = vUV + vReflect.xy;
		fOccNorm = DepthTex.Sample(St, vRandomUV).r *fViewZ;

		if (fOccNorm <=fDepth + fBias)
			++iColor;
	}
	vOut.xyz = iColor / iCount;
	return vOut;



}

float3 GetLutColor(float3 vColor, texture2D LutTex, SamplerState St)
{
	float2 vLutSize = float2(0.00390625f, 0.0625f);
	float4 vLutUV;
	vColor = saturate(vColor)*15.f;
	vLutUV.w = floor(vColor.b);
	vLutUV.xy = (vColor.rg + 0.5f)*vLutSize;
	vLutUV.x += vLutUV.w* vLutSize.y;
	vLutUV.z = vLutUV.x + vLutSize.y;

	float3 vLutColor = lerp(LutTex.SampleLevel(St, vLutUV.xy, vLutUV.z).rgb, LutTex.SampleLevel(St, vLutUV.zy, vLutUV.z).rgb, vColor.b - vLutUV.w);
	//float3 vLutColor = lerp(tex2Dlod(St, vLutUV.xyzz).rgb, tex2Dlod(St, vLutUV.zyzz).rgb, vColor.b - vLutUV.w);
	return vLutColor;
}

float ExponentialFog(float4 vPos, float4 vCamPos, float fFogStart, float fFogDensity)
{
	float3 vView = (vCamPos - vPos).xyz;

	float fDistance = length(vView);

	float fFog_Distance = max(fDistance - fFogStart, 0.0);

	float fFog = exp(-fFog_Distance * fFogDensity);
	return 1 - fFog;
}

float ExponentialHeightFog(float4 pos_ws, float4 campos, float fogstart, float fogdensity, float fogfalloff)
{
	float3 camera_to_world = (campos - pos_ws).xyz;

	float distance = length(camera_to_world);

	// 픽셀 거리에 대한 안개 응시 거리 찾기
	float fogDist = max(distance - fogstart, 0.0);

	//// 거리 기반 안개 강도
	//float fogHeightDensityAtViewer = exp(-fogfalloff * campos.y);
	//float fogDistInt = fogDist * fogHeightDensityAtViewer;

	//// 높이 기반 안개 강도
	//float eyeToPixelY = camera_to_world.y * (fogDist / distance);
	//float t = fogfalloff * eyeToPixelY;
	//const float thresholdT = 0.01;
	//float fogHeightInt = abs(t) > thresholdT ? (1.0 - exp(-t)) / t : 1.0;

	//// 두 요소를 결합하여 최종 요소를 얻습니다.
	//float fog = exp(-fogdensity * fogDistInt * fogHeightInt);

	//return 1 - fog;

	float vHeight = pos_ws.y;

	if (0.f <= vHeight)
		vHeight = 0.f;
	else
	{
		vHeight *= -1.f;
	}
	float fDistance = vHeight + fogfalloff;
	float fFog_Distance = 0.f;

	fFog_Distance = max(fDistance/* - fogstart*/, 0.0);

	float fFog = exp(-fFog_Distance * fogdensity*fogDist);
	return 1 - fFog;

}
float rgb2luma(float3 rgb)
{
	float fLuma = 0.f;
	fLuma = dot(rgb, float3(0.2126f, 0.7152f, 0.0722f));
	return fLuma;
}

half3 Radialblur(Texture2D mainTex, SamplerState sample, half2 UV, int RadialCnt, float fBlurPower, float2 vDir)
{
	const half blurpower = fBlurPower;

	half2 dir = UV - vDir;

	half3 color = half3(0, 0, 0);
	half f = 1.0 / (half) RadialCnt;
	for (int i = 0; i < RadialCnt; ++i)
	{
		color += mainTex.Sample(sample, UV - blurpower * dir * half(i)).rgb * f;
	}

	return color;
}