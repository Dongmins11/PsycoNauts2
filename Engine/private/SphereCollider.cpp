#include "..\public\SphereCollider.h"
#include "PhysXScene_Manager.h"
CSphereCollider::CSphereCollider(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CPhysXCollider(_pDevice, _pDeviceContext)
{
}

CSphereCollider::CSphereCollider(const CSphereCollider & _rhs)
	: CPhysXCollider(_rhs)
{
}

HRESULT CSphereCollider::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSphereCollider::NativeConstruct(void * _pArg)
{
	DESC tDesc;
	if (nullptr != _pArg)
	{
		tDesc = *(DESC*)_pArg;
		m_vScale = tDesc.vScale;
	}

	if (FAILED(__super::NativeConstruct(&tDesc.tColliderDesc)))
		return E_FAIL;

	CPhysXScene_Manager* m_pPhysX = CPhysXScene_Manager::Get_Instance();
	if (FAILED(m_pPhysX->Create_Sphere(this)))
		return E_FAIL;


	Update_TransformPhysX();

	return S_OK;
}

const _float3 & CSphereCollider::Get_Scale() const
{
	return m_vScale;
}

CSphereCollider * CSphereCollider::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	CSphereCollider* pInstance = new CSphereCollider(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CSphereCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CSphereCollider::Clone(void * _pArg)
{
	CSphereCollider* pInstance = new CSphereCollider(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("Failed To Create CSphereCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSphereCollider::Free()
{
	__super::Free();
}
