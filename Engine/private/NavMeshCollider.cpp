#include "NavMeshCollider.h"

CNavMeshCollider::CNavMeshCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
	: CPhysXCollider(_pDevice, _pDeviceContext)
{
}

CNavMeshCollider::CNavMeshCollider(const CNavMeshCollider& _rhs)
	: CPhysXCollider(_rhs)
{
}

HRESULT CNavMeshCollider::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavMeshCollider::NativeConstruct(void* _pArg)
{
	DESC tDesc;
	if (_pArg)
	{
		tDesc = *(static_cast<DESC*>(_pArg));
		m_vecPoints = tDesc.vecPoints;
	}

	if (FAILED(__super::NativeConstruct(&tDesc.tColliderDesc)))
		return E_FAIL;

	CPhysXScene_Manager* pPX = CPhysXScene_Manager::Get_Instance();
	if (FAILED(pPX->Create_NaviMesh(this,tDesc.m_isPlane)))
		return E_FAIL;

	Update_TransformPhysX();
	return S_OK;
}

vector<_float3*>& CNavMeshCollider::Get_NaviPoints()
{
	return m_vecPoints;
}

CNavMeshCollider* CNavMeshCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
{
	CNavMeshCollider* pInstance = new CNavMeshCollider(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CNavMeshCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavMeshCollider::Clone(void* _pArg)
{
	CNavMeshCollider* pInstance = new CNavMeshCollider(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("CNavMeshCollider Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavMeshCollider::Free()
{
	for (auto& data : m_vecPoints)
		Safe_Delete(data);

	m_vecPoints.clear();
	__super::Free();
}
