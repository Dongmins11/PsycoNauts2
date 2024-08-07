#include "stdafx.h"
#include "..\public\Boss_Dish.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "Boss_Goat.h"
#include "Player_RightHand.h"
#include "Player_LeftHand.h"
#include "CollisionObject.h"

CBoss_Dish::CBoss_Dish(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBoss_Dish::CBoss_Dish(const CBoss_Dish & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoss_Dish::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Dish::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != pArg)
		m_eMyType = *(CBoss_Dish::BOSS_FOODTYPE*)pArg;


	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_vFristPosition = _float4(40.f, 13.f, 43.583f, 1.f);
	m_vEndPosition = _float4(40.f, 0.090f, 43.583f, 1.f);


	
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(42.426f, 10.f, 43.583f, 1.f));

	return S_OK;
}

_int CBoss_Dish::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (FAILED(First_LerpPositionUpdate(TimeDelta)))
		return -1;

	if (FAILED(Grab_Dish_PositionUpdate(TimeDelta)))
		return -1;


	if (false == m_bDishGrabCheck)
	{
		m_pPhysXColliderCom->Update_TransformPhysX();

		XMStoreFloat4x4(&m_ColiderMatirx,m_pTransformCom->Get_WorldMatrix());

		m_pBoneMatrix = &m_ColiderMatirx;
		if(nullptr != m_pBoneMatrix)
			m_pColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
	}
	return _int();
}

_int CBoss_Dish::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (false == m_bDishGrabCheck)
	{
		m_pPhysXColliderCom->Update_Transform();
		if (nullptr != m_pBoneMatrix)
			m_pColliderCom->Update_Transform(m_pBoneMatrix);
	}


	if (true != m_isDestroy)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
	else
	{
		if (!m_isInteraction)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_isInteraction = true;
			/* For. Layer_BlueSpark 이펙트 생성 */
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"),
				SCENE_STATIC, TEXT("Effect_FoodHitEffect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			/* For. Layer_BlueSpark 상속 */
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"), m_pTransformCom->Get_Position(), true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			pGameInstance->Setting_Effect_LocalBillboard(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"), TEXT("Com_Option"), true);
			RELEASE_INSTANCE(CGameInstance);
		}
	}

	return 0;
}

HRESULT CBoss_Dish::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CBoss_Dish::OnTriggerEnter(CCollisionObject & collision)
{
}

void CBoss_Dish::OnTriggerExit(CCollisionObject & collision)
{
}



HRESULT CBoss_Dish::SetUp_Component()
{
	/* Com_Renderer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	switch (m_eMyType)
	{
	case Client::CBoss_Dish::BOSS_FOODTYPE_DISH:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_MainDish"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	}
	case Client::CBoss_Dish::BOSS_FOODTYPE_SOUP:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Main_Soup"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	}
	case Client::CBoss_Dish::BOSS_FOODTYPE_COOKIE:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Main_Cookie"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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
	tColliderDesc.fMass = PX_MAX_F32;
	m_eTriggerLayerType = LayerType::LAYERTYPE_STATIC;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;

	if(BOSS_FOODTYPE_COOKIE == m_eMyType)
		tBoxColliderDesc.vScale = _float3(7.f, 4.f, 7.4f);
	else
		tBoxColliderDesc.vScale = _float3(6.f, 4.f, 6.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_PhysXCollider"), (CComponent**)&m_pPhysXColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 2.f, 0.f);

	m_pPhysXColliderCom->SetPivotMatrix(smatPviot);
	m_pPhysXColliderCom->Set_ShapeLayer(m_eTriggerLayerType);
	m_pPhysXColliderCom->Reset_Power();
	m_pPhysXColliderCom->Update_TransformPhysX();
	m_pPhysXColliderCom->Update_Transform();


	//////////트리거//////////////

	CPhysXCollider::DESC tTriggerColliderDesc;

	tTriggerColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tTriggerColliderDesc.isVisualization = true;
	tTriggerColliderDesc.isTrigger = true;
	tTriggerColliderDesc.pGameObject = this;
	tTriggerColliderDesc.isSceneQuery = true;
	m_eLayerType = LayerType::LAYERTYPE_BOSS_DISH;

	
	CBoxCollider::DESC tTriggerBoxColliderDesc;
	tTriggerBoxColliderDesc.tColliderDesc = tTriggerColliderDesc;
	tTriggerBoxColliderDesc.vScale = _float3(1.f, 2.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_TirggerPhysXCollider"), (CComponent**)&m_pColliderCom, &tTriggerBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviotTwo;

	if (BOSS_FOODTYPE_COOKIE == m_eMyType)
		smatPviotTwo = XMMatrixTranslation(0.f, 1.f, -4.1f);
	else
		smatPviotTwo = XMMatrixTranslation(0.f, 1.f, -3.4f);

	m_pColliderCom->SetPivotMatrix(smatPviotTwo);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Reset_Power();
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	return S_OK;
}

HRESULT CBoss_Dish::SetUp_ConstantTable()
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


HRESULT CBoss_Dish::Colider_PosUpdate()
{
	return S_OK;
}

HRESULT CBoss_Dish::First_LerpPositionUpdate(_double TimeDelta)
{
	if (false == m_bLerpPositionCheck)
	{
		m_lfRatio += TimeDelta + m_lfAddTime;

		if (1.0 <= m_lfRatio)
			m_lfRatio = 1.0;

		_vector vLerpPosition = XMVectorLerp(XMLoadFloat4(&m_vFristPosition), XMLoadFloat4(&m_vEndPosition), (_float)m_lfRatio);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPosition);

		m_lfAddTime += 0.002;

		if (1.0 <= m_lfRatio)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Camera_Shaking(true, 0.5f);
			pGameInstance->PlaySoundOnce(L"Cookingplate.wav", STATICK_MESH);

			Safe_Release(pGameInstance);

			m_lfAddTime = 0.0;
			m_lfRatio = 0.0;
			m_bLerpPositionCheck = true;
		}

	}


	if (false == m_CamSwitchingCheck && true == m_bLerpPositionCheck)
	{
		m_lfRatio += TimeDelta;

		if (2.0 <= m_lfRatio)
		{
			m_lfRatio = 0.0;
			m_CamSwitchingCheck = true;
		}
	}


	return S_OK;
}

HRESULT CBoss_Dish::Grab_Dish_PositionUpdate(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pRightHand = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_RightHand", 0);
	CGameObject* pLeftHand = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_LeftHand", 0);
	if (nullptr == pRightHand || nullptr == pLeftHand)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);


	if (BOSS_FOODTYPE_SOUP == m_eMyType)
	{
		if (true == dynamic_cast<CPlayer_LeftHand*>(pLeftHand)->Get_DishGrabCheck())
		{
			m_pPhysXColliderCom->Remove_Actor();
			m_pColliderCom->Remove_Actor();
			m_bDishGrabCheck = true;
			_float4 vRightHandPosition = *(_float4*)dynamic_cast<CPlayer_LeftHand*>(pLeftHand)->Get_BoneMatrix()->m[3];

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vRightHandPosition));
		}


		if (true == dynamic_cast<CPlayer_LeftHand*>(pLeftHand)->Get_DishThrowCheck() && false == m_bDishThrowCheck)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pBossObject = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
			if (nullptr == pBossObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL; 
			}
			RELEASE_INSTANCE(CGameInstance);

			XMStoreFloat4(&m_vFristPosition, m_pTransformCom->Get_Position());

			if (BOSS_FOODTYPE_DISH == m_eMyType)
				m_vEndPosition = *(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_O).m[3];
			else if (BOSS_FOODTYPE_SOUP == m_eMyType)
				m_vEndPosition = *(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_H).m[3];
			else if (BOSS_FOODTYPE_COOKIE == m_eMyType)
				m_vEndPosition = *(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_F).m[3];
			m_pPhysXColliderCom->Add_Actor();
			m_bDishThrowCheck = true;
		}

		if (true == m_bDishThrowCheck)
		{
			m_bDishThrowTimeAcc += TimeDelta * 5.0;

			if (1.0 <= m_bDishThrowTimeAcc)
				m_bDishThrowTimeAcc = 1.0;

			m_pTransformCom->RotationPerSec(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * 4.0);
			_vector vLerpPositon = XMVectorLerp(XMLoadFloat4(&m_vFristPosition), XMLoadFloat4(&m_vEndPosition), (_float)m_bDishThrowTimeAcc);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPositon);
			m_pPhysXColliderCom->Update_TransformPhysX();

			if (1.0 <= m_bDishThrowTimeAcc)
			{
				m_bDishThrowTimeAcc = 0.0;
				m_bDishThrowCheck = false;
				m_isDestroy = true;


				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DishEndType_On();
				Safe_Release(pGameInstance);
			}
		}
	}
	else
	{
		if (true == dynamic_cast<CPlayer_RightHand*>(pRightHand)->Get_DishGrabCheck())
		{
			m_pPhysXColliderCom->Remove_Actor();
			m_pColliderCom->Remove_Actor();
			m_bDishGrabCheck = true;
			_float4 vRightHandPosition = *(_float4*)dynamic_cast<CPlayer_RightHand*>(pRightHand)->Get_BoneMatrix()->m[3];

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vRightHandPosition));
		}


		if (true == dynamic_cast<CPlayer_RightHand*>(pRightHand)->Get_DishThrowCheck() && false == m_bDishThrowCheck)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pBossObject = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
			if (nullptr == pBossObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			RELEASE_INSTANCE(CGameInstance);

			XMStoreFloat4(&m_vFristPosition, m_pTransformCom->Get_Position());

			if (BOSS_FOODTYPE_DISH == m_eMyType)
				m_vEndPosition = *(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_O).m[3];
			else if (BOSS_FOODTYPE_SOUP == m_eMyType)
				m_vEndPosition = *(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_H).m[3];
			else if (BOSS_FOODTYPE_COOKIE == m_eMyType)
				m_vEndPosition = *(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_F).m[3];

			m_bDishThrowCheck = true;
			m_pPhysXColliderCom->Add_Actor();
		}

		if (true == m_bDishThrowCheck)
		{
			m_bDishThrowTimeAcc += TimeDelta * 5.0;

			if (1.0 <= m_bDishThrowTimeAcc)
				m_bDishThrowTimeAcc = 1.0;

			m_pTransformCom->RotationPerSec(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * 4.0);
			_vector vLerpPositon = XMVectorLerp(XMLoadFloat4(&m_vFristPosition), XMLoadFloat4(&m_vEndPosition), (_float)m_bDishThrowTimeAcc);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPositon);
			m_pPhysXColliderCom->Update_TransformPhysX();

			if (1.0 <= m_bDishThrowTimeAcc)
			{
				m_bDishThrowTimeAcc = 0.0;
				m_bDishThrowCheck = false;
				m_isDestroy = true;

				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DishEndType_On();
				Safe_Release(pGameInstance);
			}
		}
	}


	return S_OK;
}

HRESULT CBoss_Dish::Dish_Throw_PositionUpdate(_double TimeDelta)
{
	return S_OK;
}


CBoss_Dish * CBoss_Dish::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBoss_Dish*	pInstance = new CBoss_Dish(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBoss_Dish");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_Dish::Clone(void * pArg, _uint iCurrentScene)
{
	CBoss_Dish*	pInstance = new CBoss_Dish(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBoss_Dish");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Dish::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pModelCom);

	if (nullptr != m_pPhysXColliderCom)
		m_pPhysXColliderCom->Remove_Actor();

	Safe_Release(m_pPhysXColliderCom);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);

}
