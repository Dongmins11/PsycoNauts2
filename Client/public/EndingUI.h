#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEndingUI final : public CGameObject
{
public:
	enum UISTATE { UISTATE_START, UISTATE_IDLE, UISTATE_SELECTED, UISTATE_TAB, UISTATE_END };
private:
	explicit CEndingUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEndingUI(const CEndingUI& rhs);
	virtual ~CEndingUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
	_float fRotX = 0.f;
	_float fRotY = 0.f;
	_float fRotZ = 0.f;
public:
	static CEndingUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END