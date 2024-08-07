#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	HRESULT Render();

public:/*For.Set*/
	void Set_DiffuseTag(const _tchar* pDiffuseTag) { lstrcpy(m_szDiffuse, pDiffuseTag); }
	void Set_IsEmissive(_bool bcheck) { m_bEmissive = bcheck; }
	void Set_IsShadow(_bool bcheck) { m_bShadow = bcheck; }
	void Set_ShadeAmbient(_float fAmbient) { m_fShadeAmbient = fAmbient; }
	void Set_LightDesc(_uint iIndex, LIGHTDESC& tLightDesc);
	void Set_IsAO(_bool bcheck) { m_bAO = bcheck; }
	void Set_LightRender(_bool bCheck, _uint iIndex);
	void Set_LightVector(_float3 vLook, _float3 vPos) { m_vLightLook = vLook, m_vLightPos = vPos; }
	void Set_ShadowValue(_float fShadowBias, _float fPercentLimitMax) { m_fShadowBias = fShadowBias, m_fPercentLimitMax = fPercentLimitMax; }
	void Set_LightOffset(_float fLightOffset) { m_fLightOffset = fLightOffset; }
	void Set_IsCuriling(_bool bCheck) { m_bIsCurling = bCheck; }
	void Set_IsSSAA(_bool bCheck) { m_bIsSSAA = bCheck; }
public:/*For.Get*/
	_float3 Get_ShadowLightLook() { return m_vLightLook; }
	_float3 Get_ShadowLightPos() { return m_vLightPos; }

public: /* Curling */
	void Light_Curling(_uint curindex) { m_iLightCurling = curindex; }
	void Set_CurIndex(_uint LayerIndex,_uint curindex);
	_uint Get_Lights_Size();

public:
	void Lights_Clear();

private:
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	_float4x4					m_TransformMatrix;
	_float4x4					m_ProjMatrix;

	/*For.Renderer*/
	_tchar						m_szDiffuse[MAX_PATH];
	_bool						m_bEmissive = true;
	_bool						m_bShadow = true;
	_bool						m_bAO = false;
	_float						m_fShadeAmbient = 0.2f;
	_float						m_fLightOffset = 1.f;
	_bool						m_bIsSSAA = true;
private:
	list<class CLight*>			m_Lights;
	typedef list<class CLight*>	LIGHTS;

	_uint						m_iLightCurling = 0;

private:/*For.Shadow*/
	_float3					m_vLightLook;
	_float3					m_vLightPos;
	_float					m_fShadowBias = 0.0000175f;
	_float					m_fPercentLimitMax = 0.175f;

private:
	_bool					m_bIsCurling = true;
public:
	virtual void Free() override;
};

END