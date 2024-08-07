#pragma once

#include "Base.h"

/* 빛의 정보를 보관한다. */

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLight() = default;


public:
	HRESULT NativeConstruct(const LIGHTDESC& LightDesc);
	HRESULT Render(class CVIBuffer_Rect* pVIBuffer);

public:/*For.Set*/
	void Set_IsOutline(_bool bCheck) { m_bGaraOutline = bCheck; }
	void Set_LightDesc(LIGHTDESC& tLightDesc) { m_LightDesc = tLightDesc; }
	void Set_CurIndex(_uint iCurIndex) { m_CurIndex = iCurIndex; }
	void Set_LightRender(_bool bCheck) { m_bIsRender = bCheck; }
public:
	_uint Get_CurIndex() { return m_CurIndex; }
public:/*For.Get*/

public:
	LIGHTDESC* Get_LightDesc(){
		return &m_LightDesc;
	}
	bool Get_IsOutline() { return m_bGaraOutline; }

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	LIGHTDESC			m_LightDesc;
	_bool				m_bGaraOutline = false;
	_uint				m_CurIndex = 0;
	_bool				m_bIsRender = true;
public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END