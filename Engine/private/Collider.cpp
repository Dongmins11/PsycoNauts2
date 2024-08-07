#include "..\Public\Collider.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_eType(rhs.m_eType)
	, m_isCollision(false)
{
	Safe_AddRef(m_pInputLayout);
}

_float3 CCollider::Get_ColliderCenter(TYPE _Coltype)
{
	switch (_Coltype)
	{
	case Engine::CCollider::TYPE_AABB:
		return m_pAABB->Center;
		break;
	case Engine::CCollider::TYPE_OBB:
		return m_pOBB->Center;
		break;
	case Engine::CCollider::TYPE_SPHERE:
		return m_pSphere->Center;
		break;
	default:
		break;
	}
	return _float3(0.f, 0.f, 0.f);
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eColliderType)
{
	m_eType = eColliderType;

	m_pEffect = new BasicEffect(m_pDevice);

	const void*	pShaderByteCodes = nullptr;
	size_t	ShaderCodeLength = 0;

	m_pEffect->SetVertexColorEnabled(true);

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &ShaderCodeLength);		

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, ShaderCodeLength, &m_pInputLayout)))
		return E_FAIL;



	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pDeviceContext);

	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB = new BoundingBox(m_ColliderDesc.vCenter, m_ColliderDesc.vExtents);
		break;
	case TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(m_ColliderDesc.vCenter, m_ColliderDesc.vExtents, _float4(0.f, 0.f, 0.f, 1.f));
		break;
	case TYPE_SPHERE:
		m_pSphere = new BoundingSphere(m_ColliderDesc.vCenter, m_ColliderDesc.fRadius);
		break;
	}	

	return S_OK;
}
void CCollider::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix;

	

	if (nullptr != m_pAABB)
	{
		m_pAABB->Center = m_ColliderDesc.vCenter;
		m_pAABB->Extents = m_ColliderDesc.vExtents;

		m_pAABB->Transform(*m_pAABB, Remove_Rotation(WorldMatrix));
	}

	if (nullptr != m_pOBB)
	{
		m_pOBB->Center = m_ColliderDesc.vCenter;
		m_pOBB->Extents = m_ColliderDesc.vExtents;
		m_pOBB->Orientation = _float4(0.f, 0.f, 0.f, 1.f);

		m_pOBB->Transform(*m_pOBB, WorldMatrix);
	}

	if (nullptr != m_pSphere)
	{
		m_pSphere->Center = m_ColliderDesc.vCenter;
		m_pSphere->Radius = m_ColliderDesc.fRadius;

		m_pSphere->Transform(*m_pSphere, WorldMatrix);
	}
}

_bool CCollider::RayCastCollider(_fvector vOriginPos, _fvector vRayDir, _float * fDist, CCollider * pTarget)
{
	_bool isPicking = false;
	float _fDist = 0.f;

	if (XMVector3Equal(XMVectorZero(), vRayDir))
		return false;

	if (nullptr == pTarget)
		return false;

	switch (pTarget->m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
	{
		isPicking = pTarget->m_pAABB->Intersects(vOriginPos, XMVector3Normalize(vRayDir), _fDist);
		break;

	}
	case Engine::CCollider::TYPE_OBB:
	{
		isPicking = pTarget->m_pOBB->Intersects(vOriginPos, XMVector3Normalize(vRayDir), _fDist);
		break;
	}
	case Engine::CCollider::TYPE_SPHERE:
	{
		isPicking = pTarget->m_pSphere->Intersects(vOriginPos, XMVector3Normalize(vRayDir), _fDist);
		break;
	}
	default:
		break;
	}
	*fDist = _fDist;
	return isPicking;
}

_bool CCollider::PickingCollider(_fvector vOriginPos, _fvector vRayDir, _float * fDist)
{
	_bool isPicking = false;
	float _fDist = 0.f;

	switch (m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		isPicking = m_pAABB->Intersects(vOriginPos, XMVector3Normalize(vRayDir), _fDist);
		break;
	case Engine::CCollider::TYPE_OBB:
		isPicking = m_pOBB->Intersects(vOriginPos, XMVector3Normalize(vRayDir), _fDist);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		isPicking = m_pSphere->Intersects(vOriginPos, XMVector3Normalize(vRayDir), _fDist);
		break;
	default:
		break;
	}
	*fDist = _fDist;
	return isPicking;
}

_bool CCollider::Collision_AABB(CCollider * pTarget, TYPE _Coltype)
{
	switch (_Coltype)
	{
	case Engine::CCollider::TYPE_AABB:
		return m_isCollision = m_pAABB->Intersects(*pTarget->m_pAABB);
		break;
	case Engine::CCollider::TYPE_OBB:
		return m_isCollision = m_pAABB->Intersects(*pTarget->m_pOBB);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		return m_isCollision = m_pAABB->Intersects(*pTarget->m_pSphere);
		break;
	}
}

_bool CCollider::Collision_OBB(CCollider * pTarget, TYPE _Coltype)
{
	switch (_Coltype)
	{
	case Engine::CCollider::TYPE_AABB:
		return m_isCollision = m_pOBB->Intersects(*pTarget->m_pAABB);
		break;
	case Engine::CCollider::TYPE_OBB:
		return m_isCollision = m_pOBB->Intersects(*pTarget->m_pOBB);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		return m_isCollision = m_pOBB->Intersects(*pTarget->m_pSphere);
		break;
	}
}

_bool CCollider::Collision_Sphere(CCollider * pTarget, TYPE _Coltype)
{
	switch (_Coltype)
	{
	case Engine::CCollider::TYPE_AABB:
		return m_isCollision = m_pSphere->Intersects(*pTarget->m_pAABB);
		break;
	case Engine::CCollider::TYPE_OBB:
		return m_isCollision = m_pSphere->Intersects(*pTarget->m_pOBB);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		return m_isCollision = m_pSphere->Intersects(*pTarget->m_pSphere);
		break;
	}
}

#ifdef _DEBUG

HRESULT CCollider::Render(_fvector _vColor)
{
	if (nullptr == m_pEffect ||
		nullptr == m_pBatch)
		return E_FAIL;

	CPipeLine*	pPipeLine = GET_INSTANCE(CPipeLine);

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(pPipeLine->Get_Transform(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform(CPipeLine::D3DTS_PROJ));



	m_pEffect->Apply(m_pDeviceContext);
	_vector	vColor;
	if (XMVector3Equal(_vColor, XMVectorZero()))
	{
		if (false == m_ColliderColorCheck)
			vColor = m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);
		else
			vColor = m_ColliderColor;
	}
	else
	{
		vColor = m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : _vColor;
	}
	m_pBatch->Begin();

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, vColor);
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, vColor);
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, vColor);
		break;
	}

	

	m_pBatch->End();

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}

#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

CCollider::OBBDESC CCollider::Compute_OBBDesc()
{
	OBBDESC		OBBDesc;

	_float3		vPoints[8];
	_vector		vPointVector[8];

	m_pOBB->GetCorners(vPoints);

	for (_uint i = 0; i < 8; ++i)
		vPointVector[i] = XMLoadFloat3(&vPoints[i]);

	OBBDesc.vCenter = m_pOBB->Center;
	XMStoreFloat3(&OBBDesc.vExtentDir[0], (vPointVector[2] + vPointVector[5]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vExtentDir[1], (vPointVector[2] + vPointVector[7]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vExtentDir[2], (vPointVector[2] + vPointVector[0]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vAlignDir[0], XMVector3Normalize(vPointVector[2] - vPointVector[3]));
	XMStoreFloat3(&OBBDesc.vAlignDir[1], XMVector3Normalize(vPointVector[2] - vPointVector[1]));
	XMStoreFloat3(&OBBDesc.vAlignDir[2], XMVector3Normalize(vPointVector[2] - vPointVector[6]));

	return OBBDesc;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eColliderType)
{
	CCollider*	pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eColliderType)))
	{
		MSGBOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere);

	Safe_Release(m_pInputLayout);
		


}

