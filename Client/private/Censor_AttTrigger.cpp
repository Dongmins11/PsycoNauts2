#include "stdafx.h"
#include "..\public\Censor_AttTrigger.h"
#include "GameInstance.h"

CCensor_AttTrigger::CCensor_AttTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCensor_AttTrigger::CCensor_AttTrigger(const CCensor_AttTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCensor_AttTrigger::NativeConstruct(void* _pArg, _uint iCurrentScene)
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

HRESULT CCensor_AttTrigger::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CCensor_AttTrigger::Tick(_double TimeDelta)
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

_int CCensor_AttTrigger::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CCensor_AttTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CCensor_AttTrigger::OnTriggerEnter(CCollisionObject & collision)
{
}

void CCensor_AttTrigger::OnCollisionEnter(CCollisionObject & collision)
{
}

void CCensor_AttTrigger::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CCensor_AttTrigger::OnTriggerExit(CCollisionObject & collision)
{
}

void CCensor_AttTrigger::Attack(_bool _IsAttack)
{
	m_IsAttack = _IsAttack;
}

HRESULT CCensor_AttTrigger::SetUp_Component()
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
	tSphereColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_AttCollider"), (CComponent**)&m_pAttColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CCensor_AttTrigger * CCensor_AttTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CCensor_AttTrigger*	pInstance = new CCensor_AttTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg, SCENE_STATIC)))
	{
		MSGBOX("Failed to Created : CCensor_AttTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCensor_AttTrigger::Clone(void * pArg, _uint iSceneIndex)
{
	return nullptr;
}

void CCensor_AttTrigger::Free()
{
	__super::Free();
	if (m_pAttColliderCom)
		m_pAttColliderCom->Remove_Actor();
	Safe_Release(m_pAttColliderCom);
	Safe_Release(m_pTransformCom);
}
