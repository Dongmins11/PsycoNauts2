#include "..\public\BoxCollider.h"

CBoxCollider::CBoxCollider(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CPhysXCollider(_pDevice, _pDeviceContext)
{
}

CBoxCollider::CBoxCollider(const CBoxCollider & _rhs)
	:CPhysXCollider(_rhs)
{
}

HRESULT CBoxCollider::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxCollider::NativeConstruct(void * _pArg)
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
	if (FAILED(m_pPhysX->Create_Box(this)))
		return E_FAIL;

	Update_TransformPhysX();

	return S_OK;
}

const _float3 & CBoxCollider::Get_Scale() const
{
	return m_vScale;
}

CBoxCollider * CBoxCollider::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	CBoxCollider* pInstance = new CBoxCollider(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CBoxCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CBoxCollider::Clone(void * _pArg)
{
	CBoxCollider* pInstance = new CBoxCollider(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("Failed To Create CBoxCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoxCollider::Free()
{
	__super::Free();
}
