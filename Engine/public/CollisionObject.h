#pragma once

#include "Base.h"
#include "GameInstance.h"
#include "GameObject.h"
BEGIN(Engine)
//class CGameObject;
class ENGINE_DLL CCollisionObject
{
public:
	CCollisionObject();
	virtual ~CCollisionObject() = default;

public:
	CTransform* Get_ObjectTransform() const;
	const LayerType Get_ObjectLayerType() const;
public:
	CGameObject* Get_Object() { return m_pCollisionGameObject; }
	_float3		 Get_HitPosFloat3() { return m_vHitPos; }
	_vector		Get_HitPos() { return XMLoadFloat3(&m_vHitPos); }
	void Set_GameObject(CGameObject* pGameObject) { m_pCollisionGameObject = pGameObject; }
	void Set_HitPosition(_float3 vPos) { m_vHitPos = vPos; }

public:
	void CollisionEnter(CCollisionObject& collision) const { if(m_pCollisionGameObject)m_pCollisionGameObject->OnCollisionEnter(collision); }
	void CollisionStay(CCollisionObject& collision) const { if (m_pCollisionGameObject)m_pCollisionGameObject->OnCollisionStay(collision); }
	void CollisionExit(CCollisionObject& collision) const { if (m_pCollisionGameObject)m_pCollisionGameObject->OnCollisionExit(collision); }

	void TriggerEnter(CCollisionObject& collision) const { if (m_pCollisionGameObject)m_pCollisionGameObject->OnTriggerEnter(collision); }
	void TriggerStay(CCollisionObject& collision) const { if (m_pCollisionGameObject)m_pCollisionGameObject->OnTriggerStay(collision); }
	void TriggerExit(CCollisionObject& collision) const { if (m_pCollisionGameObject)m_pCollisionGameObject->OnTriggerExit(collision); }

	void ControllerHit(CCollisionObject& collision) const { if (m_pCollisionGameObject)m_pCollisionGameObject->OnControllerCollisionHit(collision); }
private:
	CGameObject* m_pCollisionGameObject = nullptr;
	_float3		m_vHitPos = { 0.f,0.f,0.f };
};
END
