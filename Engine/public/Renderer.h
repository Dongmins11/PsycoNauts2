#pragma once

/* 화면에 그려져야할 객체들을 모아서 보관한다. */
/* 보관할때 그려져야하는 순서대로 출력할수있도록 나눠서 보관하낟. */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER { RENDER_PRIORITY,RENDER_SHADOW,RENDER_NONALPHA,RENDER_REFLECT,RENDER_REFLECT_DEST,RENDER_PLAYER,RENDER_DISTORTION,RENDER_NONSHADOW_LAST,RENDER_TEST, RENDER_IBL, RENDER_NONLIGHT, RENDER_ALPHA, RENDER_NONBLOOM_EFFECT , RENDER_MODEL_EFFECT,RENDER_MODEL_EFFECT_AFTER, RENDER_RECT_EFFECT, RENDER_PARTICLE, RENDER_UI,RENDER_TOOL, RENDER_END };
	enum RENDEROPTION { RENDEROPTION_RIMLIGHT, RENDEROPTION_OUTLINE, RENDEROPTION_SHADOW, RENDEROPTION_HDR, RENDEROPTION_MOTIONBLUR, RENDEROPTION_BLOOM,RENDEROPTION_BLOOMEFFECT, RENDEROPTION_DOP,RENDEROPTION_EMISSIVE,
		RENDEROPTION_FOG, RENDEROPTION_DISTORTION_POST, RENDEROPTION_RADIAL, RENDEROPTION_SSAO,RENDEROPTION_LUT,RENDEROPTION_END };
	//enum TONEOPTION { TONEOPTION_ACES_OLD, TONEOPTION_HABLE_OLD, TONEOPTION_ACES_NEW, TONEOPTION_HABLE_NEW, TONEOPTION_REIN, TONEOPTION_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;

public:/*For.Set*/
	void Set_IsRim(_bool bCheck) { m_bRim = bCheck; }
	void Set_RimLightColor(_float4 vColor) { m_vRimLightColor = vColor; }
	void Set_RimLightColorPower(_float fPower) { m_fRimLightPower = fPower; }
	void Set_RimWidth(_float fRimWidth) { m_fRimWidth = fRimWidth; }
	void Set_RenderOption(RENDEROPTION eRenderOption, _bool bCheck) { m_bRenderOption[eRenderOption] = bCheck; }
	void Set_RimRatio(_float2 vRimRatio) { m_vRImRatio = vRimRatio; }
	void Set_ToneOption(_uint iToneOption) { m_iToneOption = iToneOption; }
	void Set_BlurOption(_uint iBlurOption) { m_iBlurOption = iBlurOption; }
	void Set_BloomWeight(_float* ArrWeight) { memcpy(m_BloomWeight, ArrWeight, sizeof(_float) * 4); }
	void Set_BloomEffectWeight(_float* ArrWeight) { memcpy(m_BloomEffectWeight, ArrWeight, sizeof(_float) * 4); }
	void Set_DOFWeight(_float* ArrWeight) { memcpy(m_DOFWeight, ArrWeight,sizeof(_float)*4); }
	void Set_SSAOValue(_float fRad, _float fBias) { m_fSSAORadious = fRad, m_fBias = fBias; }
	void Set_LUT_Index(_uint iIndex) { m_iLUT_Index = iIndex; }
	void Set_Fog(_float* ArrFogValues, _float4 vFogColor, _bool bHeight) { memcpy(m_fFog_Values, ArrFogValues, sizeof(_float) * 3), m_vFog_Color = vFogColor, m_bFog_Height = bHeight; }
	void Set_MSAA(_float fOffset, _uint iSampleCount) { m_fMSAA_Offset = fOffset, m_iMassSampler = iSampleCount; }
	void Set_Radial(_int iCnt, _float fBlurPower, _float2 vDir) { m_iRadialCnt = iCnt, m_fRadialBlurPower = fBlurPower, m_vRadialDir = vDir; }
	void Set_IsSSAA(_bool bCheck) { m_bIsSSAA = bCheck; }
public:/*For.Get*/
	bool Get_IsRim() { return m_bRim; }
	_float4 Get_RimLightColorFloat4() { return m_vRimLightColor; }
	_vector Get_RimLightColorVector() { return XMLoadFloat4(&m_vRimLightColor); }
	_float   Get_RimLightPower() { return m_fRimLightPower; }
	_float   Get_RimWidth() { return m_fRimWidth; }
	_float2   Get_RimRatio() { return m_vRImRatio; }
	_bool	Get_RenderOption(RENDEROPTION eRenderOption) { return m_bRenderOption[eRenderOption]; }

public:
	HRESULT	Add_RenderTarget();
	HRESULT Add_MRT();
	HRESULT Create_RenderTargetBuffer();
#ifdef _DEBUG
	HRESULT Ready_Debug_Buffer_Desc();
#endif // DEBUG

	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();
	HRESULT Draw_RenderGroup_Tool();



private:
	list<class CGameObject*>            m_RenderGroup[RENDER_END];
	typedef list<class CGameObject*>      RENDERGROUP;

private:
	class CTarget_Manager*               m_pTarget_Manager = nullptr;
	class CLight_Manager*               m_pLight_Manager = nullptr;

private:
	class CVIBuffer_Rect*      m_pVIBuffer = nullptr;
	class CVIBuffer_Rect*      m_pVIBuffer_Down_2 = nullptr;
	class CVIBuffer_Rect*      m_pVIBuffer_Down_4 = nullptr;
	class CVIBuffer_Rect*      m_pVIBuffer_Up_2 = nullptr;
	class CTexture*			   m_pTexture_LUT = nullptr;

	_float4x4               m_TransformMatrix;
	_float4x4               m_ProjMatrix;

	// For.RimLight
	_bool                  m_bRim = false;
	_float4                  m_vRimLightColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float                  m_fRimLightPower = 3.f;
	_float2					m_vRImRatio;
	_bool					m_bRenderOption[RENDEROPTION_END];
	_uint					m_iToneOption=2;
	_uint					m_iBlurOption = 2;
	_float					m_BloomWeight[4];
	_float					m_BloomEffectWeight[4];
	_float					m_DOFWeight[4];
	_float					m_fSSAORadious = 0.001f;
	_float					m_fBias = 0.0003f;
	_float					m_fRimWidth = 0.f;

	_float					m_fMulResolution = 2.f;
	_float					m_fMSAA_Offset = 0.5f;
	_uint					m_iMassSampler = 1;
	_uint					m_iLUT_Index = 0;

	_bool					m_bFog_Height = true;
	//_float4					m_vFog_Color = _float4(0.3019f, 0.4f, 0.5019f, 1.f);
	_float4					m_vFog_Color = _float4(0.f, 0.f, 0.f, 1.f);
	_float					m_fFog_Values[3];

	_int					m_iRadialCnt = 11;
	_float					m_fRadialBlurPower = 0.011f;
	_float2					m_vRadialDir = _float2(0.5f, 0.5f);

	_bool					m_bIsSSAA = true;
public:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_Reflect();
	HRESULT Render_NonAlpha();
	HRESULT Render_Distortion();
	HRESULT Render_MSAA();
	HRESULT Render_Rim_Blur();
	HRESULT Render_HDR();
	HRESULT Render_SSAO();
	HRESULT Render_SSAO_Blur();
	HRESULT Render_Lights();
	HRESULT Render_Effect();
	HRESULT Render_TotalDiffuse();
	HRESULT Render_Brightness();
	HRESULT Render_IBL();
	HRESULT Render_Blur();
	HRESULT Render_BrightnessBlur();
	HRESULT Render_BrightnessBlur_Two();
	HRESULT Render_DOPBlur();
	HRESULT Render_Bloom();

	HRESULT Render_EffectBlur();
	HRESULT Render_Brightness_Effect();
	HRESULT Render_BrightnessBlur_Effect();
	HRESULT Render_BrightnessBlur_Two_Effect();
	HRESULT Render_Bloom_Effect();

	HRESULT Render_ToneMapping();
	HRESULT Render_LUT();
	HRESULT Render_DOP();
	HRESULT Render_Fog();
	HRESULT Render_MotionBlur();
	HRESULT Render_Distortion_Post();
	HRESULT Render_Radial();
	HRESULT Render_Blend();
	HRESULT Render_Blend_Tool();
	HRESULT Render_NonLight();
	HRESULT Render_Alpha();
	HRESULT Render_NonBloomEffect();
	HRESULT Render_Model_Effect();
	HRESULT Render_Model_Effect_After();
	HRESULT Render_Rect_Effect();
	HRESULT Render_Particle();
	HRESULT Render_UI();
	HRESULT Render_Tool();
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END