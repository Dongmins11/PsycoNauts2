#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Trail;
class CEffect_Option;
END

BEGIN(Client)
class CTrail_Effect final :public CGameObject
{
protected:
	explicit CTrail_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrail_Effect(const CTrail_Effect& rhs);
	virtual ~CTrail_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
	
private:
	CTexture*				m_pTexture = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Trail*		m_pVIBuffer = nullptr;
	CEffect_Option*			m_pOptionCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CTrail_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END