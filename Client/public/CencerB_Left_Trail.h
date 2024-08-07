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
class CCencerB_Left_Trail final :public CGameObject
{
protected:
	explicit CCencerB_Left_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCencerB_Left_Trail(const CCencerB_Left_Trail& rhs);
	virtual ~CCencerB_Left_Trail() = default;

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
	static CCencerB_Left_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END