#include "stdafx.h"
#include "..\public\AttTrigger_GoatH.h"
#include "GameInstance.h"

CAttTrigger_GoatH::CAttTrigger_GoatH(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CAttTrigger_GoatH::CAttTrigger_GoatH(const CAttTrigger_GoatH & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAttTrigger_GoatH::NativeConstruct(void* _pArg, _uint iCurrentScene)
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

HRESULT CAttTrigger_GoatH::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CAttTrigger_GoatH::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pAttColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pParentMatrix));

	return 0;
}

_int CAttTrigger_GoatH::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CAttTrigger_GoatH::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CAttTrigger_GoatH::OnTriggerEnter(CCollisionObject & collision)
{
}

void CAttTrigger_GoatH::OnCollisionEnter(CCollisionObject & collision)
{
}

void CAttTrigger_GoatH::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CAttTrigger_GoatH::OnTriggerExit(CCollisionObject & collision)
{
}

HRESULT CAttTrigger_GoatH::SetUp_Component()
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
	tSphereColliderDesc.vScale = _float3(5.f, 5.f, 5.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_AttCollider"), (CComponent**)&m_pAttColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CAttTrigger_GoatH * CAttTrigger_GoatH::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CAttTrigger_GoatH*	pInstance = new CAttTrigger_GoatH(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg, SCENE_STATIC)))
	{
		MSGBOX("Failed to Created : CAttTrigger_GoatH");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAttTrigger_GoatH::Clone(void * pArg, _uint iSceneIndex)
{
	return nullptr;
}

void CAttTrigger_GoatH::Free()
{
	__super::Free();
	if (m_pAttColliderCom)
	{
		m_pAttColliderCom->Remove_Actor();
		Safe_Release(m_pAttColliderCom);
	}
	Safe_Release(m_pTransformCom);
}
