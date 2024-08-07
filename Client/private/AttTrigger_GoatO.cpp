#include "stdafx.h"
#include "..\public\AttTrigger_GoatO.h"
#include "GameInstance.h"

CAttTrigger_GoatO::CAttTrigger_GoatO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CAttTrigger_GoatO::CAttTrigger_GoatO(const CAttTrigger_GoatO & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAttTrigger_GoatO::NativeConstruct(void* _pArg, _uint iCurrentScene)
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

HRESULT CAttTrigger_GoatO::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CAttTrigger_GoatO::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pAttColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pParentMatrix));

	return 0;
}

_int CAttTrigger_GoatO::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CAttTrigger_GoatO::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CAttTrigger_GoatO::OnTriggerEnter(CCollisionObject & collision)
{
}

void CAttTrigger_GoatO::OnCollisionEnter(CCollisionObject & collision)
{
}

void CAttTrigger_GoatO::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CAttTrigger_GoatO::OnTriggerExit(CCollisionObject & collision)
{
}

HRESULT CAttTrigger_GoatO::SetUp_Component()
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

CAttTrigger_GoatO * CAttTrigger_GoatO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CAttTrigger_GoatO*	pInstance = new CAttTrigger_GoatO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg, SCENE_STATIC)))
	{
		MSGBOX("Failed to Created : CAttTrigger_GoatO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAttTrigger_GoatO::Clone(void * pArg, _uint iSceneIndex)
{
	return nullptr;
}

void CAttTrigger_GoatO::Free()
{
	__super::Free();
	if (m_pAttColliderCom)
	{
		m_pAttColliderCom->Remove_Actor();
		Safe_Release(m_pAttColliderCom);
	}
	Safe_Release(m_pTransformCom);
}
