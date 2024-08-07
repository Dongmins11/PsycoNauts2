#include "stdafx.h"
#include "..\public\Boss_Food_Projectile.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "Boss_Goat.h"
#include "Boss_Dish.h"
#include "CollisionObject.h"

CBoss_Food_Projectile::CBoss_Food_Projectile(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBoss_Food_Projectile::CBoss_Food_Projectile(const CBoss_Food_Projectile & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoss_Food_Projectile::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Food_Projectile::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;


	if (nullptr != pArg)
		m_eFoodType = *(CBoss_Food_Projectile::FOOD_TYPE*)(pArg);

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	m_pTransformCom->Set_MyLook(pPlayerTrans->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

	_uint iSize = pGameInstance->Get_LayerSize(SCENE_STAGE05, L"Boss_Dish_One");

	if (0 > iSize)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	CTransform* pDishTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STAGE05, TEXT("Boss_Dish_One"), TEXT("Com_Transform"), 0);
	
	if (nullptr != pDishTrans)
	{
		XMStoreFloat4(&m_vStartPosition, pDishTrans->Get_State(CTransform::STATE_POSITION));
		m_vStartPosition.y += 0.5f;

		if (CBoss_Dish::BOSS_FOODTYPE_SOUP == pPlayer->Get_DishType())
		{
			m_vStartPosition.y += 0.5f;

			XMStoreFloat4(&m_vEndPosition, (pPlayerTrans->Get_Position()
				+ (XMVector3Normalize(pPlayerTrans->Get_State(CTransform::STATE_LOOK)) * 1.3f))
				+ (XMVector3Normalize(pPlayerTrans->Get_State(CTransform::STATE_UP)) * 1.3f)
				+ (XMVector3Normalize(pPlayerTrans->Get_State(CTransform::STATE_RIGHT)) * 0.9f));
		}
		else
		{
			m_vStartPosition.y += 1.5f;

			XMStoreFloat4(&m_vEndPosition, (pPlayerTrans->Get_Position()
				+ (XMVector3Normalize(pPlayerTrans->Get_State(CTransform::STATE_LOOK)) * 1.3f))
				+ (XMVector3Normalize(pPlayerTrans->Get_State(CTransform::STATE_UP)) * 0.6f)
				+ (XMVector3Normalize(pPlayerTrans->Get_State(CTransform::STATE_RIGHT)) * 0.9f));
		}
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HitEffect"),
		SCENE_STATIC, TEXT("Effect_HitEffect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Layer_BlueSpark »ó¼Ó */

	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, false, m_pTransformCom, TEXT("Layer_HitEffect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	//pGameInstance->Setting_Effect_LocalBillboard(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HitEffect"), TEXT("Com_Option"), true);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CBoss_Food_Projectile::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (FAILED(Object_PosUpdate(TimeDelta)))
		return -1;

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	return _int();
}

_int CBoss_Food_Projectile::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_Transform();

	if (true != m_isDestroy)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}

	return 0;
}

HRESULT CBoss_Food_Projectile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CBoss_Food_Projectile::OnTriggerEnter(CCollisionObject & collision)
{
}


HRESULT CBoss_Food_Projectile::SetUp_Component()
{
	/* Com_Renderer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	

	switch (m_eFoodType)
	{
	case Client::CBoss_Food_Projectile::FOOD_TYPE_BROCOL:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Dish_BroCcoli"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_CARROT:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Dish_Hash"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_POTATO:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Dish_Potato"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_SOUP:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Soup_Blob"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_COOKIE_ONE:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_One"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_COOKIE_TWO:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_Two"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_COOKIE_THREE:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_Three"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case Client::CBoss_Food_Projectile::FOOD_TYPE_COOKIE_FOR:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_For"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	}



	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.isTrigger = true;
	m_eLayerType = LayerType::LAYERTYPE_PLAYER_SKILL;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(3.f, 3.f, 3.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_PhysXCollider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.f, 0.f);

	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Reset_Power();
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();


	return S_OK;
}

HRESULT CBoss_Food_Projectile::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());

	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));

	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));


	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CBoss_Food_Projectile::Object_PosUpdate(_double TimeDelta)
{
	if (false == m_isDestroy)
	{
		if (false == m_bFirstLerpCheck)
		{
			m_lfLerpRatio += TimeDelta * 7.0;

			if (1.0 <= m_lfLerpRatio)
				m_lfLerpRatio = 1.0;

			_vector vLerpPosition = XMVectorLerp(XMLoadFloat4(&m_vStartPosition), XMLoadFloat4(&m_vEndPosition), (_float)m_lfLerpRatio);

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPosition);

			if (1.0 <= m_lfLerpRatio)
			{
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);

				CGameObject* pBossObject = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
				CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
				if (nullptr == pBossObject || nullptr == pPlayerObject)
				{
					Safe_Release(pGameInstance);
					return E_FAIL;
				}

				Safe_Release(pGameInstance);

				_float4x4 BossMat;
				
				if (CBoss_Dish::BOSS_FOODTYPE_DISH == dynamic_cast<CPlayer*>(pPlayerObject)->Get_DishType())
					BossMat = dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_O);
				else if(CBoss_Dish::BOSS_FOODTYPE_SOUP == dynamic_cast<CPlayer*>(pPlayerObject)->Get_DishType())
					BossMat = dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_H);
				else if(CBoss_Dish::BOSS_FOODTYPE_COOKIE == dynamic_cast<CPlayer*>(pPlayerObject)->Get_DishType())
					BossMat = dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_F);

				XMStoreFloat4(&m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_SECOND], m_pTransformCom->Get_Position());
				m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_FIRST] = m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_SECOND];
				m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_FIRST].y -= 40.f;

				m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_THIRD] = *(_float4*)BossMat.m[3];
				m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_FORTH] = m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_THIRD];
				m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_FORTH].y -= 30.f;

				m_bFirstLerpCheck = true;
				m_bCatMulShootCheck = true;
				m_lfLerpRatio = 0.0;
			}

		}


		if (true == m_bCatMulShootCheck)
		{
			m_lfCatMulShootRatio += TimeDelta * 2.0;

			if (1.0 <= m_lfCatMulShootRatio)
				m_lfCatMulShootRatio = 1.0;

			_vector vShootPostion = XMVectorCatmullRom(XMLoadFloat4(&m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_FIRST]), XMLoadFloat4(&m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_SECOND]),
				XMLoadFloat4(&m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_THIRD]), XMLoadFloat4(&m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_FORTH]),
				(_float)m_lfCatMulShootRatio);

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vShootPostion);

			if (1.0 <= m_lfCatMulShootRatio)
			{
				m_bCatMulShootCheck = false;
				m_lfCatMulShootRatio = 0.0;
				m_isDestroy = true;


			}
		}
	}


	return S_OK;
}

HRESULT CBoss_Food_Projectile::Colider_PosUpdate()
{
	return S_OK;
}


CBoss_Food_Projectile * CBoss_Food_Projectile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBoss_Food_Projectile*	pInstance = new CBoss_Food_Projectile(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBoss_Food_Projectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_Food_Projectile::Clone(void * pArg, _uint iCurrentScene)
{
	CBoss_Food_Projectile*	pInstance = new CBoss_Food_Projectile(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBoss_Food_Projectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Food_Projectile::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pModelCom);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
}
