#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTexture;
END

BEGIN(Client)
class CChopC final :public CGameObject
{
private:
	explicit CChopC(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CChopC(const CChopC& rhs);
	virtual ~CChopC() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pNoiseTexture = nullptr;
	CTexture*				m_pAlphaTexture = nullptr;

	_float					m_fTimeAcc = 0.0f;
	_bool					m_isCreate = false;
	_bool					m_isParticleCreate = false;

private:
	HRESULT	Lading_Effect(class CTransform* pTransform);
	HRESULT	Impact_Effect(class CTransform* pTransform);

	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CChopC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
END