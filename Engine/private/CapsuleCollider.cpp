#include "..\public\CapsuleCollider.h"

CCapsuleCollider::CCapsuleCollider(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CPhysXCollider(_pDevice, _pDeviceContext)
{
}

CCapsuleCollider::CCapsuleCollider(const CCapsuleCollider & _rhs)
	: CPhysXCollider(_rhs)
{
}

HRESULT CCapsuleCollider::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCapsuleCollider::NativeConstruct(void * _pArg)
{/*
	DESC tDesc;
	if (nullptr != _pArg)
	{
		tDesc = *(DESC*)_pArg;
		m_vScale = tDesc.vScale;
	}

	if (FAILED(__super::NativeConstruct(&tDesc.tColliderDesc)))
		return E_FAIL;

	if (FAILED(m_pPhysX->Create_Box(this)))
		return E_FAIL;

	Update_TransformPhysX();*/

	return S_OK;
}

const _float3 & CCapsuleCollider::Get_Scale() const
{
	return m_vScale;
}

CCapsuleCollider * CCapsuleCollider::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	CCapsuleCollider* pInstance = new CCapsuleCollider(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CCapsuleCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCapsuleCollider::Clone(void * _pArg)
{
	CCapsuleCollider* pInstance = new CCapsuleCollider(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("Failed To Create CCapsuleCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCapsuleCollider::Free()
{
	__super::Free();
}
