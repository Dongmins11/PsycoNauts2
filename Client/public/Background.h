#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBackground final : public CGameObject
{
private:
	explicit CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBackground(const CBackground& rhs);
	virtual ~CBackground() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pTextureTitleCom = nullptr;
	CVIBuffer_Rect*		m_pModelCom = nullptr;
	CVIBuffer_Rect*		m_pModelTextCom = nullptr;
	CVIBuffer_Rect*		m_pModelTitleCom = nullptr;
	
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
	_float				m_fTimeAcc = 0.f;
	_float				m_fX, m_fY, m_fLogoSizeX, m_fLogoSizeY, m_fTextSizeX, m_fTextSizeY;
	_float4x4			m_ProjMatrix;

public:
	static CBackground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END