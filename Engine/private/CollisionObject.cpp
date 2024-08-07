#include "..\public\CollisionObject.h"
#include "GameObject.h"


CCollisionObject::CCollisionObject()
{
}

CTransform * CCollisionObject::Get_ObjectTransform() const
{
	return (CTransform*)m_pCollisionGameObject->Get_Components(TEXT("Com_Transform"));
}

const LayerType CCollisionObject::Get_ObjectLayerType() const
{
	return m_pCollisionGameObject->Get_ObjectLayerType();
}
