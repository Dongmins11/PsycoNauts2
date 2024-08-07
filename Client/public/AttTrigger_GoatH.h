#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CSphereCollider;
class CTransform;
END

BEGIN(Client)
class CAttTrigger_GoatH final :public CGameObject
{
private:
	CAttTrigger_GoatH(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAttTrigger_GoatH(const CAttTrigger_GoatH& rhs);
	virtual ~CAttTrigger_GoatH() = default;

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
	CSphereCollider*		m_pAttColliderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:
	const _float4x4*		m_pParentMatrix = nullptr;

private:
	HRESULT SetUp_Component();

public:
	static CAttTrigger_GoatH* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END