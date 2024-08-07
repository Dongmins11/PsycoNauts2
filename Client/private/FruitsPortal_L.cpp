#include "stdafx.h"
#include "..\public\FruitsPortal_L.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Fruits.h"
#include "Boss_Goat.h"

CFruitsPortal_L::CFruitsPortal_L(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CFruitsPortal_L::CFruitsPortal_L(const CFruitsPortal_L & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFruitsPortal_L::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		FRUITPORTALINFO			tPortalInfo;
		ZeroMemory(&tPortalInfo, sizeof(FRUITPORTALINFO));
		tPortalInfo = *(FRUITPORTALINFO*)_pArg;

		_float4x4		WorldMatrix;
		WorldMatrix = tPortalInfo.Worldmatrix;
		m_pParent = tPortalInfo.pParent;
		m_pTransformCom->Set_WorldMatrix(tPortalInfo.Worldmatrix);
	}

	m_pBoxCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_STATIC);
	return S_OK;
}

HRESULT CFruitsPortal_L::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CFruitsPortal_L::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pBoxCollider->Update_TransformPhysX();

	return 0;
}

_int CFruitsPortal_L::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CFruitsPortal_L::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CFruitsPortal_L::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PICK)
	{
		if (((CFruits*)collision.Get_Object())->Get_Type() == ((CBoss_Goat*)m_pParent)->Get_FruitType())
		{
			((CBoss_Goat*)m_pParent)->Check_ClearWave();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE05, TEXT("Layer_Fruit_ForFinalStage"));

			for (_uint i = 0; i < iLayerSize; ++i)
			{
				pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_Fruit_ForFinalStage"), i)->Set_IsDestroy(true);
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CFruitsPortal_L::OnCollisionEnter(CCollisionObject & collision)
{
}

void CFruitsPortal_L::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CFruitsPortal_L::OnTriggerExit(CCollisionObject & collision)
{
}

HRESULT CFruitsPortal_L::SetUp_Component()
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

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(4.f, 4.f, 4.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_BoxCollider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CFruitsPortal_L * CFruitsPortal_L::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CFruitsPortal_L*	pInstance = new CFruitsPortal_L(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg, SCENE_STATIC)))
	{
		MSGBOX("Failed to Created : CFruitsPortal_L");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFruitsPortal_L::Clone(void * pArg, _uint iSceneIndex)
{
	return nullptr;
}

void CFruitsPortal_L::Free()
{
	__super::Free();
	if (m_pBoxCollider)
	{
		m_pBoxCollider->Remove_Actor();
		Safe_Release(m_pBoxCollider);
	}
	Safe_Release(m_pTransformCom);
}
