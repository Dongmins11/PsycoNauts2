#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CSphereCollider;
class CTransform;
END

BEGIN(Client)
class CCensorB_AttTriggerL final :public CGameObject
{
private:
	CCensorB_AttTriggerL(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCensorB_AttTriggerL(const CCensorB_AttTriggerL& rhs);
	virtual ~CCensorB_AttTriggerL() = default;

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

public:
	void Attack(_bool _IsAttack);

private:
	CSphereCollider*		m_pAttColliderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:
	const _float4x4*		m_pParentMatrix = nullptr;
	_bool					m_IsReadyDead = false;
	_bool					m_IsAttack = false;

private:
	HRESULT SetUp_Component();

public:
	static CCensorB_AttTriggerL* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END