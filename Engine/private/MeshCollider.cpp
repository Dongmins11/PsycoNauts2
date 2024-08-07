#include "..\public\MeshCollider.h"

CMeshCollider::CMeshCollider(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CPhysXCollider(_pDevice, _pDeviceContext)
{
}

CMeshCollider::CMeshCollider(const CMeshCollider & _rhs)
	:CPhysXCollider(_rhs)
{
}

HRESULT CMeshCollider::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshCollider::NativeConstruct(void * _pArg)
{
	DESC tDesc;
	if (nullptr != _pArg)
	{
		tDesc = *(DESC*)_pArg;
	}

	if (FAILED(__super::NativeConstruct(&tDesc.tColliderDesc)))
		return E_FAIL;

	
	CPhysXScene_Manager* m_pPhysX = CPhysXScene_Manager::Get_Instance();
	if (FAILED(m_pPhysX->Create_MeshCollider(this, tDesc.tModelDataDesc.pVerticesPos, tDesc.tModelDataDesc.iVerticesSize, tDesc.tModelDataDesc.pIndicies, tDesc.tModelDataDesc.iIndiciesSize)))
		return E_FAIL;

	Update_TransformPhysX();

	return S_OK;
}

CMeshCollider * CMeshCollider::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	CMeshCollider* pInstance = new CMeshCollider(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CMeshCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshCollider::Clone(void * _pArg)
{
	CMeshCollider* pInstance = new CMeshCollider(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("Failed To Create CMeshCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshCollider::Free()
{
	__super::Free();
}
