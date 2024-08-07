#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CTuto_MessageUI final : public CGameObject
{
private:
	explicit CTuto_MessageUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTuto_MessageUI(const CTuto_MessageUI& rhs);
	virtual ~CTuto_MessageUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pButtonTextureCom = nullptr;

	CVIBuffer_Rect*		m_pBackground_BufferCom = nullptr;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

	_double				m_lfTimeAcc = 0.0;
	_double				m_lfTimeMax = 2.0;
private:
	
public:
	static CTuto_MessageUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END