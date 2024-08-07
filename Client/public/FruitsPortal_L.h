#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBoxCollider;
class CTransform;
END

BEGIN(Client)
class CFruitsPortal_L final :public CGameObject
{
public:
	typedef struct tagFruitPortalInfo
	{
		CGameObject*	pParent;
		_float4x4		Worldmatrix;
	}FRUITPORTALINFO;

private:
	CFruitsPortal_L(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFruitsPortal_L(const CFruitsPortal_L& rhs);
	virtual ~CFruitsPortal_L() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

private:
	CBoxCollider*		m_pBoxCollider = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	_bool				m_IsCheck = false;
	CGameObject*		m_pParent = nullptr;

private:
	HRESULT SetUp_Component();

public:
	static CFruitsPortal_L* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END