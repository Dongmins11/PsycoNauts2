#include "..\public\PhysXCollider.h"
#include "Transform.h"
#include "GameObject.h"
#include "GameInstance.h"

CPhysXCollider::CPhysXCollider(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CComponent(_pDevice, _pDeviceContext)
{

	XMStoreFloat4x4(&m_PivotMatrix, XMMatrixIdentity());
}

CPhysXCollider::CPhysXCollider(const CPhysXCollider & _rhs)
	:CComponent(_rhs)
	,m_PivotMatrix(_rhs.m_PivotMatrix)
{
}

HRESULT CPhysXCollider::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysXCollider::NativeConstruct(void * _pArg)
{
	if (nullptr != _pArg)
	{
		m_tDesc = *(DESC*)_pArg;
		//m_pTransform = m_tDesc.pTransform;
		m_pTransform = (CTransform*)m_tDesc.pGameObject->Get_Components(TEXT("Com_Transform"));
	}

	if (FAILED(__super::NativeConstruct(_pArg)))
		return E_FAIL;

	return S_OK;
}

const CPhysXCollider::DESC& CPhysXCollider::Get_Desc() const
{
	return m_tDesc;
}

HRESULT CPhysXCollider::Set_ShapeLayer(LayerType eLayerType)
{
	CPhysXScene_Manager* m_pPhysX = CPhysXScene_Manager::Get_Instance();
	if (!m_pPhysX || !m_pShape)
		return E_FAIL;

	m_pPhysX->Set_ShapeLayer(m_pShape, (_uint)eLayerType);
	return S_OK;
}

void CPhysXCollider::SetRigidActor(PxRigidActor * _pRigidbody)
{
	m_pRigidActor = _pRigidbody;
}

void CPhysXCollider::SetShape(PxShape * _pShape)
{
	m_pShape = _pShape;
}

void CPhysXCollider::SetMaterial(PxMaterial * _pMaterial)
{
	m_pMaterial = _pMaterial;
}

void CPhysXCollider::SetMass(const _float _fValue)
{
	if (0.f > _fValue ||
		CPhysXScene_Manager::TYPE_DYNAMIC != m_tDesc.eType)
		return;

	static_cast<PxRigidBody*>(m_pRigidActor)->setMass(_fValue);
	m_tDesc.fMass = _fValue;
}

void CPhysXCollider::SetPivotMatrix(_fmatrix _PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, _PivotMatrix);
}

void CPhysXCollider::SetGravity(const _bool _isGravity)
{
	m_pRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_isGravity);
	
	m_tDesc.isGravity = _isGravity;
}

void CPhysXCollider::SetKinematic(const _bool _isKinematic)
{
	static_cast<PxRigidBody*>(m_pRigidActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, _isKinematic);
	m_tDesc.isKinematic = _isKinematic;
}

void CPhysXCollider::SetTrigger(const _bool _isTrigger)
{
	switch (_isTrigger)
	{
	case true:
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		break;
	case false:
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		break;
	}
	m_tDesc.isTrigger = _isTrigger;
}


void CPhysXCollider::SetRestitution(const _float fValue)
{
	m_tDesc.fRestitution = fValue;
}

void CPhysXCollider::Remove_Actor()
{
	CPhysXScene_Manager* m_pPhysX = CPhysXScene_Manager::Get_Instance();
	m_pPhysX->Remove_Actor(m_pRigidActor);
}


void CPhysXCollider::Add_Actor()
{
	CPhysXScene_Manager* pPX = CPhysXScene_Manager::Get_Instance();

	pPX->Add_Actor(m_pRigidActor);
}

void CPhysXCollider::Sleep()
{
	if (CPhysXScene_Manager::TYPE_DYNAMIC == m_tDesc.eType)
		static_cast<PxRigidDynamic*>(m_pRigidActor)->putToSleep();
}

void CPhysXCollider::WakeUp()
{
	if (CPhysXScene_Manager::TYPE_DYNAMIC == m_tDesc.eType)
		static_cast<PxRigidDynamic*>(m_pRigidActor)->putToSleep();
}

void CPhysXCollider::Add_Force(_fvector vPow)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, vPow);
	PxVec3 vPower = ToPxVec3(vPos);
	if (m_tDesc.eType == CPhysXScene_Manager::TYPE_DYNAMIC)
	{
		static_cast<PxRigidDynamic*>(m_pRigidActor)->addForce(vPower, PxForceMode::eVELOCITY_CHANGE);
	}
}

void CPhysXCollider::Add_Force(_fvector vPow, PxForceMode::Enum eMode)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, vPow);
	PxVec3 vPower = ToPxVec3(vPos);
	if (m_tDesc.eType == CPhysXScene_Manager::TYPE_DYNAMIC)
	{
		static_cast<PxRigidDynamic*>(m_pRigidActor)->addForce(vPower, eMode);
	}
}

void CPhysXCollider::Add_Torque(_fvector vAxis, PxForceMode::Enum eMode)
{
	if (m_tDesc.eType == CPhysXScene_Manager::TYPE_DYNAMIC)
	{
		PxRigidDynamic* tmpRigidActor = static_cast<PxRigidDynamic*>(m_pRigidActor);
		tmpRigidActor->setMaxAngularVelocity(360.f);
		_float3 fAxis;
		XMStoreFloat3(&fAxis, vAxis);
		PxVec3 pxAxis = ToPxVec3(fAxis);
		tmpRigidActor->addTorque(pxAxis, eMode);
	}
}

void CPhysXCollider::Reset_Power()
{
	if (m_tDesc.eType == CPhysXScene_Manager::TYPE_DYNAMIC)
	{
		PxRigidDynamic* tmpRigidActor = static_cast<PxRigidDynamic*>(m_pRigidActor);
		PxVec3 vecZero = PxVec3(PxZero);
		tmpRigidActor->setAngularVelocity(vecZero);
		tmpRigidActor->setLinearVelocity(vecZero);
	}
}

PxShape* CPhysXCollider::Get_Shape()
{
	if (m_pShape)
		return m_pShape;
	else
		return nullptr;
}

void CPhysXCollider::Update_Transform()
{
	PxTransform pxTransform = m_pRigidActor->getGlobalPose();
	_float3 vPosition = FromPxVec3(pxTransform.p);
	_float4 vQuat = FromPxQuat(pxTransform.q);
	_matrix vQuatMat = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuat));
	_vector vErrorFix = XMVector3TransformCoord(XMLoadFloat3(&(_float3)m_PivotMatrix.m[3]), vQuatMat);
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPosition) - vErrorFix, 1.f));
	//m_pTransform->Set_State(CTransform::STATE_POSITION,XMLoadFloat3(&vPosition));
	m_pTransform->RotationQuaternion(XMLoadFloat4(&vQuat));
}

void CPhysXCollider::Update_Transform(_float4x4* _pWorldMatrix)
{
	PxTransform pxTransform = m_pRigidActor->getGlobalPose();
	_float3 vPosition = FromPxVec3(pxTransform.p);
	_float4 vQuat = FromPxQuat(pxTransform.q);
	_matrix vQuatMat = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuat));
	_vector vErrorFix = XMVector3TransformCoord(XMLoadFloat3(&(_float3)m_PivotMatrix.m[3]), vQuatMat);
	
	XMStoreFloat4((_float4*)_pWorldMatrix->m[3], XMVectorSetW(XMLoadFloat3(&vPosition) - vErrorFix, 1.f));

	_float	fSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4((_float4*)_pWorldMatrix->m[0])));
	_float	fSizeY = XMVectorGetY(XMVector3Length(XMLoadFloat4((_float4*)_pWorldMatrix->m[1])));
	_float	fSizeZ = XMVectorGetZ(XMVector3Length(XMLoadFloat4((_float4*)_pWorldMatrix->m[2])));

	_matrix RotationMatirx = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuat));

	XMStoreFloat4((_float4*)_pWorldMatrix->m[0], RotationMatirx.r[0] * fSizeX);
	XMStoreFloat4((_float4*)_pWorldMatrix->m[1], RotationMatirx.r[1] * fSizeY);
	XMStoreFloat4((_float4*)_pWorldMatrix->m[2], RotationMatirx.r[2] * fSizeZ);

	//m_pTransform->RotationQuaternion(XMLoadFloat4(&vQuat));
	//m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPosition) - vErrorFix, 1.f));
}

void CPhysXCollider::Update_TransformPhysX()
{
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	WorldMatrix = XMLoadFloat4x4(&m_PivotMatrix) * WorldMatrix;
	//WorldMatrix.r[3] = WorldMatrix.r[3] + XMLoadFloat3(&(_float3)m_PivotMatrix.m[3]);
	_vector svScale, svQuat, svPos;
	XMMatrixDecompose(&svScale, &svQuat, &svPos, WorldMatrix);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, svPos);

	_float4 vQuat;
	XMStoreFloat4(&vQuat, svQuat);

	PxTransform transform = m_pShape->getLocalPose();
	transform.p = ToPxVec3(vPosition);
	transform.q = ToPxQuat(vQuat);
	m_pRigidActor->setGlobalPose(transform);

}

void CPhysXCollider::Update_TransformPhysX(_fmatrix _WorldMatrix)
{
	_matrix WorldMatrix = _WorldMatrix;
	WorldMatrix = XMLoadFloat4x4(&m_PivotMatrix) * WorldMatrix;
	//WorldMatrix.r[3] = WorldMatrix.r[3] + XMLoadFloat3(&(_float3)m_PivotMatrix.m[3]);
	_vector svScale, svQuat, svPos;
	XMMatrixDecompose(&svScale, &svQuat, &svPos, WorldMatrix);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, svPos);
	
	_float4 vQuat;
	XMStoreFloat4(&vQuat, svQuat);

	PxTransform transform = m_pShape->getLocalPose();
	transform.p = ToPxVec3(vPosition);
	transform.q = ToPxQuat(vQuat);
	m_pRigidActor->setGlobalPose(transform);

}

void CPhysXCollider::Free()
{
	//Safe_Release(m_pTransform);
	if (!m_isCloned)
	{
		if (nullptr != m_pShape)
		{
			if (nullptr != m_pShape->getActor())
				m_pShape->getActor()->detachShape(*m_pShape);
		}

		CPhysXScene_Manager* m_pPhysX = CPhysXScene_Manager::Get_Instance();
		m_pPhysX->Remove_Actor(m_pRigidActor);

		Safe_PxRelease(m_pShape);
		Safe_PxRelease(m_pRigidActor);
		Safe_PxRelease(m_pMaterial);
	}
	__super::Free();
}
