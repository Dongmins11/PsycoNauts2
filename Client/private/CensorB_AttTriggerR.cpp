#include "stdafx.h"
#include "..\public\CensorB_AttTriggerR.h"
#include "GameInstance.h"

CCensorB_AttTriggerR::CCensorB_AttTriggerR(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCensorB_AttTriggerR::CCensorB_AttTriggerR(const CCensorB_AttTriggerR & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCensorB_AttTriggerR::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		m_pParentMatrix = (_float4x4*)_pArg;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pAttColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER_ATTACK);
	return S_OK;
}

HRESULT CCensorB_AttTriggerR::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CCensorB_AttTriggerR::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (m_IsAttack&&
		!m_isDestroy)
	{
		m_pAttColliderCom->Add_Actor();
		m_pAttColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pParentMatrix));
	}
	else
		m_pAttColliderCom->Remove_Actor();

	return 0;
}

_int CCensorB_AttTriggerR::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CCensorB_AttTriggerR::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CCensorB_AttTriggerR::OnTriggerEnter(CCollisionObject & collision)
{
}

void CCensorB_AttTriggerR::OnCollisionEnter(CCollisionObject & collision)
{
}

void CCensorB_AttTriggerR::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CCensorB_AttTriggerR::OnTriggerExit(CCollisionObject & collision)
{
}

void CCensorB_AttTriggerR::Attack(_bool _IsAttack)
{
	m_IsAttack = _IsAttack;
}

HRESULT CCensorB_AttTriggerR::SetUp_Component()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = true;

	tColliderDesc.isSceneQuery = false;
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_AttCollider"), (CComponent**)&m_pAttColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CCensorB_AttTriggerR * CCensorB_AttTriggerR::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCensorB_AttTriggerR*	pInstance = new CCensorB_AttTriggerR(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCensorB_AttTriggerR");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCensorB_AttTriggerR::Clone(void * pArg, _uint iSceneIndex)
{
	CCensorB_AttTriggerR*	pInstance = new CCensorB_AttTriggerR(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CCensorB_AttTriggerR");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCensorB_AttTriggerR::Free()
{
	__super::Free();
	if (m_pAttColliderCom)
		m_pAttColliderCom->Remove_Actor();
	Safe_Release(m_pAttColliderCom);
	Safe_Release(m_pTransformCom);
}
