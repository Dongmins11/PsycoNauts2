#include "stdafx.h"
#include "..\public\JudgeWeapon_AttTrigger.h"
#include "GameInstance.h"

CJudgeWeapon_AttTrigger::CJudgeWeapon_AttTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CJudgeWeapon_AttTrigger::CJudgeWeapon_AttTrigger(const CJudgeWeapon_AttTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CJudgeWeapon_AttTrigger::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		m_pParentMatrix = (_float4x4*)_pArg;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_isPushing = true;

	m_pAttColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER_ATTACK);
	return S_OK;
}

HRESULT CJudgeWeapon_AttTrigger::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CJudgeWeapon_AttTrigger::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (m_IsAttack&& !m_isDestroy)
	{
		m_pAttColliderCom->Add_Actor();
		m_pAttColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pParentMatrix));
	}
	else
		m_pAttColliderCom->Remove_Actor();

	return 0;
}

_int CJudgeWeapon_AttTrigger::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CJudgeWeapon_AttTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CJudgeWeapon_AttTrigger::OnTriggerEnter(CCollisionObject & collision)
{
}

void CJudgeWeapon_AttTrigger::OnCollisionEnter(CCollisionObject & collision)
{
}

void CJudgeWeapon_AttTrigger::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CJudgeWeapon_AttTrigger::OnTriggerExit(CCollisionObject & collision)
{
}

void CJudgeWeapon_AttTrigger::Attack(_bool _IsAttack)
{
	m_IsAttack = _IsAttack;
}

void CJudgeWeapon_AttTrigger::Set_ColliderDir(_vector vDir)
{
	XMStoreFloat4(&m_vColliderDir, vDir);
}

HRESULT CJudgeWeapon_AttTrigger::SetUp_Component()
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
	tSphereColliderDesc.vScale = _float3(3.f, 3.f, 3.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_AttCollider"), (CComponent**)&m_pAttColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CJudgeWeapon_AttTrigger * CJudgeWeapon_AttTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CJudgeWeapon_AttTrigger*	pInstance = new CJudgeWeapon_AttTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CJudgeWeapon_AttTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJudgeWeapon_AttTrigger::Clone(void * pArg, _uint iSceneIndex)
{
	CJudgeWeapon_AttTrigger*	pInstance = new CJudgeWeapon_AttTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CJudge_AttTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJudgeWeapon_AttTrigger::Free()
{
	__super::Free();
	if (m_pAttColliderCom)
	{
		m_pAttColliderCom->Remove_Actor();
		Safe_Release(m_pAttColliderCom);
	}
	Safe_Release(m_pTransformCom);
}
