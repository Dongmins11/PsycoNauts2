#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CEffect_Option;
END

BEGIN(Client)
class CDonutRing final :public CGameObject
{
private:
	explicit CDonutRing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDonutRing(const CDonutRing& rhs);
	virtual ~CDonutRing() = default;

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
	CEffect_Option*			m_pOptionCom = nullptr;

	_float					m_fTimeAcc = 0.0f;
	_bool					m_isFadeOn = false;
	_float					m_fFadeSet = 1.f;

	EFFECTMESHINFO*			m_pMeshInfo = nullptr;

	/* »ó¼Ó */
	_bool	  m_isOnce = false;
	_float4x4 m_ParentMat;

private:
	HRESULT Call_Inheritance();

	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CDonutRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
END