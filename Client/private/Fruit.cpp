#include "stdafx.h"
#include "..\public\Fruit.h"
#include "CollisionObject.h"
#include "EnviromentTrigger.h"
#include "Scene_Stage03.h"
#include "PlatingPlate_Phase0.h"
#include "PlatingPlate_Phase1.h"
#include "PlatingPlate_Phase2.h"
#include "Choppig_HandKnife.h"
#include "CookingProgressBarUI.h"
#include "Blender.h"
#include "PPlate.h"
#include "KitchenPan.h"
#include "FruitSpawner.h"

CFruit::CFruit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CFruit::CFruit(const CFruit & rhs)
	:CGameObject(rhs)
{
}

HRESULT CFruit::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CFruit::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	return S_OK;
}

_int CFruit::Tick(_double TimeDelta)
{
	if (__super::Tick(TimeDelta))
		return 0;


	//if (m_bDead_Frit)
	//{
	//	m_isDestroy = true;
	//	Dead_And_SettingFrit();
	//	return 0;
	//}


	if (m_bDead_Frit)
	{
		m_Respawn += TimeDelta;
		if (3.0 <= m_Respawn)
		{
			m_Respawn = 0.0;
			m_pColliderCom->SetGravity(false);
			m_isMove = false;
			m_isPicking = false;
			m_bDead_Frit = false;
			Cooking_MeshSetting();
		}
		return 0;
	}


	//if (m_bReset)
	//{
	//	WorldMatrixSetting(XMLoadFloat4x4(&m_fWorldMatrix));
	//	m_eCurrentModelType = FMT_IDLE;
	//	Set_CutSceneAnimation(0);
	//	m_bReset = false;
	//}

	Update_Cooking(TimeDelta);

	//if (FAILED(Create_Effect()))
	//	return -1;

	return _int();
}

_int CFruit::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	if (m_pColliderCom && !m_isPicking)
	{
		m_pColliderCom->Update_Transform();
	}


	return 0;
}

HRESULT CFruit::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CFruit::Set_StartBlending()
{
	m_isStartCooking = true;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pCookingUI = (CCookingProgressBarUI*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), m_iCookingUINumber);
	if (nullptr == pCookingUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pCookingUI->Set_IsRender(true);
	pCookingUI->Set_MaxTime(30.f);
	RELEASE_INSTANCE(CGameInstance);
}

void CFruit::Set_EndBaking()
{
	Add_CookingNumber(2 << (_uint)m_eCurrentCookingType);
	m_eCurrentModelType = FMT_GRIDDLED;
	m_pModelCom[m_eCurrentModelType]->Set_Animation(0);
	m_isFinishCooking = true;
	pCookingUI->Set_IsFinished(true);
	m_pColliderCom->Add_Actor();
	m_pColliderCom->SetGravity(false);
	m_pColliderCom->Reset_Power();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
	m_pColliderCom->Update_TransformPhysX();
}

void CFruit::Cooking_MeshSetting()
{
	m_isPlating = false;
	// 현재 요리된 상태(덧셈으로표현)
	m_iCookingNumber = 0;

	// 요리를 할 준비가 되어있는가?
	m_isReadyCooking = false;
	// 요리가 시작하였는가?
	m_isStartCooking = false;
	// 요리가 끝났는가?
	m_isFinishCooking = false;
	// 현재 요리중인 시간
	m_lfCookingTimeAcc = 0.0;
	// 현재 요리의 종료 시간
	m_lfCookingTimeMax = 0.0;
	// 현재 요리중인 시간
	m_lfLerpToCookWareTimeAcc = 0.0;
	m_iCookingUINumber = 0;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_BackUpPostion));
	m_pTransformCom->Set_MyLook(XMLoadFloat4(&m_BackUpLook));

	m_eCurrentModelType = FMT_IDLE;
	Set_CutSceneAnimation(0);
}

void CFruit::Set_CutSceneAnimation(_uint Index)
{
	m_pModelCom[m_eCurrentModelType]->Set_Animation(Index);
}

void CFruit::Set_FruitToPlate()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPlatePosition));
	m_pTransformCom->Set_MyLook(XMVectorSet(0.f, 0.f, -1.f, 0.f));
	m_pColliderCom->SetGravity(false);
	m_pColliderCom->Reset_Power();
	m_pColliderCom->SetMass(10.f);
	m_pModelCom[m_eCurrentModelType]->Set_Animation(0);

	m_pColliderCom->Update_TransformPhysX();
}

void CFruit::Dead_And_SettingFrit()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/*_uint Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_FruitSpawner"));
	if (15 >= Count)
	{*/
		CFruitSpawner* pTarget = (CFruitSpawner*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), m_SpwnerIndex);
		pTarget->Reset_FruitObject();
	//}
	RELEASE_INSTANCE(CGameInstance);

}

void CFruit::Bone_MatrixCalculation(_uint iFruitType)
{
	if (nullptr != m_pCombinedMatrix[iFruitType] && nullptr != m_pWorldMatrix)
	{
		_float	fYGab = 0.f;
		_float	fCorrectionValue = 0.3f;
		_float4 vPos{};
		_float4 vBonePos{};
		_float3 vLook{};
		_matrix BoneMatrix{};
	
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform* pHandTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
		if (nullptr == pHandTrans)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		XMStoreFloat4(&vPos ,pHandTrans->Get_Position());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

		BoneMatrix = XMLoadFloat4x4(&m_OffsetMatrix[iFruitType]) * XMLoadFloat4x4(m_pCombinedMatrix[iFruitType]) * XMLoadFloat4x4(&m_PivotMatrix[iFruitType]) * XMLoadFloat4x4(m_pWorldMatrix);

		XMStoreFloat4(&vBonePos, BoneMatrix.r[3]);

		fYGab = vBonePos.y - vPos.y;
		vPos.y -= (fYGab - fCorrectionValue);
		
		XMStoreFloat3(&vLook, -pHandTrans->Get_State(CTransform::STATE_LOOK));
		vLook.y = 0.f;


		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
		m_pTransformCom->Set_MyLook(XMLoadFloat3(&vLook));

	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
		_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		if (nullptr == pPlayerTrans)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

}

void CFruit::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_TRIGGER)
	{
		if (!m_isMove || m_isPicking || m_isStartCooking)
			return;

		CEnviromentTrigger* pTrigger = (CEnviromentTrigger*)collision.Get_Object();
		if (pTrigger->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_COOKING)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CookType eCookType = CookType::COOK_END;
			switch (pTrigger->Get_TriggerNum())
			{
			case 0:
				eCookType = CookType::COOK_BOIL;
				break;
			case 1:
				eCookType = CookType::COOK_CUT;
				break;
			case 2:
				eCookType = CookType::COOK_BLEND;
				break;
			case 3:
				eCookType = CookType::COOK_BAKE;
				break;
			default:
				break;
			}
			
			_bool isChecking = Checking_FruitCooking(eCookType,&m_iCookingUINumber);

			if (isChecking)
			{
 				Ready_Setting_Cooking(pTrigger->Get_TriggerNum());
			}
			else
				OutputDebugStringW(TEXT("InCorrectFruitOnTrigger\n"));

			RELEASE_INSTANCE(CGameInstance);
		}

	}
	if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_DEAD)
	{
		OutputDebugStringW(TEXT("OnTriggerEnter - 과일 사망됌\n"));
		m_bDead_Frit = true;
	}
}


void CFruit::OnTriggerExit(CCollisionObject & collision)
{
}

void CFruit::Curling(_vector pTransform, _float Range, CBoxCollider* pCollider)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->isIn_WorldSpace(pTransform, Range))
	{
		_int iPlayerCurlingIndex = 0;
		_int iDeleteCurlingIndex = 0;
		if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
		{
			if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
			{
				m_isDestroy = true;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			Stage_RenderBranch(iPlayerCurlingIndex, pCollider);
		}
	}
	else
	{
		if (nullptr != pCollider)
		{
			if (m_isRender && pCollider)
			{
				pCollider->Remove_Actor();
			}
		}
		m_isRender = false;
	}

	if (!m_pColliderCom)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_isPicking)
	{
		if (m_pTransformCom)
		{
			if (m_isFinishCooking)
				Reset_Setting_Cooking();

			Bone_MatrixCalculation((_uint)m_eCurrentModelType);

			m_pColliderCom->SetGravity(false);
			m_isMove = true;
			m_pModelCom[m_eCurrentModelType]->Set_Animation(10);
			m_pColliderCom->Reset_Power();
		}
	}
	else if (m_isMove && !m_isPicking)
	{
		if (m_isPlating)
		{
			Set_FruitToPlate();
		}
		else
		{
			m_pColliderCom->SetGravity(true);
		}
	}
	else
	{
		m_pColliderCom->Reset_Power();
	}
	if(!m_isPicking)
		m_pColliderCom->Update_TransformPhysX();


	RELEASE_INSTANCE(CGameInstance);
	return;
}

void CFruit::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	if(m_pColliderCom)
		m_pColliderCom->Add_Force(vDir, eMode);

}

void CFruit::Setting_Throwing(_fvector vDir)
{
	if (!m_pColliderCom)
		return;

	m_isColController = false;
	m_pColliderCom->Add_Force(vDir*20.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
}

HRESULT CFruit::WorldMatrixSetting(_matrix WorldMatrix)
{
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vPostion = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float3 fPos;
	_vector NewvLook = vPostion + (vRight / 2.f);
	m_pTransformCom->Set_MyLook(NewvLook - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPos.x, fPos.y+3.f, fPos.z, 1.f));

	XMStoreFloat4(&m_BackUpPostion, vPostion);
	XMStoreFloat4(&m_BackUpLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	//vPos = XMVectorSet(fPos.x, fPos.y + 3.f, fPos.z, 1.f);
	return S_OK;
}

HRESULT CFruit::PhysColliderBox_Setting()
{
	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isGravity = false;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;
	//m_eLayerType = LayerType::LAYERTYPE_PICK;
	m_eLayerType = LayerType::LAYERTYPE_MONSTER_FRUIT;


	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = { 0.5f,1.2f,0.5f };

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.7f, 0.f);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Reset_Power();
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();
	
	return S_OK;
}

HRESULT CFruit::Update_Cooking(_double TimeDelta)
{
	if (m_eCurrentCookingType == CookType::COOK_BLEND && !m_isStartCooking && !m_isReadyCooking)
	{
		m_lfLerpToCookWareTimeAcc += TimeDelta;
		if (m_lfLerpToCookWareTimeAcc > 1.0)
			m_lfLerpToCookWareTimeAcc = 1.0;
		_vector vPos = XMVectorLerp(XMLoadFloat3(&m_vCookWareHitPosition), XMLoadFloat3(&m_vCookWarePosition), (_float)m_lfLerpToCookWareTimeAcc);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom->Set_MyLook(XMVectorSet(0.f, 0.f, -1.f, 0.f));
		m_pColliderCom->SetGravity(false);
		m_pColliderCom->Reset_Power();
		m_pColliderCom->Update_TransformPhysX();

		if (m_lfLerpToCookWareTimeAcc >= 1.0)
		{
			m_isReadyCooking = true;
			m_lfLerpToCookWareTimeAcc = 0.0;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
			m_pTransformCom->Set_MyLook(XMVectorSet(0.f, 0.f, -1.f, 0.f));
			m_pColliderCom->SetGravity(false);
			m_pColliderCom->Reset_Power();
			m_pColliderCom->Update_TransformPhysX();
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pPPlate = pGameInstance->Get_GameObject(SCENE::SCENE_STAGE03, L"Layer_PPlate", (_uint)CPPlate::BUTTON_TYPE_MIXER);
			CTransform* pPos = (CTransform*)pPPlate->Get_Components(TEXT("Com_Transform"));
			m_vBlenderFinishPosition = STOREFLOAT3(pPos->Get_Position());
			RELEASE_INSTANCE(CGameInstance);
			dynamic_cast<CPPlate*>(pPPlate)->Set_ButtonUpStateCheck(CPPlate::BUTTON_TYPE::BUTTON_TYPE_MIXER, true);
		}
		return S_OK;
	}
	else if (m_eCurrentCookingType == CookType::COOK_BLEND && m_isFinishCooking)
	{
		m_lfLerpToCookWareTimeAcc += TimeDelta;
		if (m_lfLerpToCookWareTimeAcc > 1.0)
			m_lfLerpToCookWareTimeAcc = 1.0;

		/* 성준 값 변경 필요함 */
		_vector vPos =XMVectorCatmullRom(XMVectorSet(m_vCookWarePosition.x+2.f, m_vCookWarePosition.y - 90.f, m_vCookWarePosition.z-2.f, 1.f)
			, XMLoadFloat3(&m_vCookWarePosition)
			, XMLoadFloat3(&m_vCookWareCatmullRomEndPosition)
			, XMVectorSet(m_vCookWareCatmullRomEndPosition.x+2.f, m_vCookWareCatmullRomEndPosition.y - 90.f, m_vCookWareCatmullRomEndPosition.z-2.f, 1.f)
			,(_float)m_lfLerpToCookWareTimeAcc);
		
		//_vector vPos = XMVectorLerp(XMLoadFloat3(&m_vCookWarePosition), XMLoadFloat3(&m_vBlenderFinishPosition), (_float)m_lfLerpToCookWareTimeAcc);
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CBlender* pBlender = (CBlender*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender6"), 0);
		pBlender->Set_Create_Effect(false);
		RELEASE_INSTANCE(CGameInstance);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pColliderCom->SetGravity(true);
		m_pColliderCom->Reset_Power();
		m_pColliderCom->Update_TransformPhysX();

		if (m_lfLerpToCookWareTimeAcc >= 1.0)
		{
			Reset_Setting_Cooking();
		}

	}
	else if (m_eCurrentCookingType == CookType::COOK_BLEND && !m_isStartCooking && m_isReadyCooking)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		m_pTransformCom->Set_MyLook(XMVectorSet(0.f, 0.f, -1.f, 0.f));
		m_pColliderCom->SetGravity(false);
		m_pColliderCom->Reset_Power();
		m_pColliderCom->Update_TransformPhysX();
	}

	if (m_eCurrentCookingType == CookType::COOK_BOIL && m_isFinishCooking)
	{
		m_pColliderCom->SetGravity(false);
		m_pColliderCom->Reset_Power();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pColliderCom->Update_TransformPhysX();
		m_pModelCom[m_eCurrentModelType]->Set_Animation(13);

	}
	if (!m_isStartCooking || m_isFinishCooking || !m_isReadyCooking)
		return S_OK;

	if (m_eCurrentCookingType == CookType::COOK_BOIL)
	{
		m_lfCookingTimeAcc += TimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pColliderCom->Remove_Actor();
		m_pModelCom[m_eCurrentModelType]->Set_Animation(13);
		pCookingUI->Set_TimeAcc(m_lfCookingTimeAcc);	

		if (m_lfCookingTimeAcc > m_lfCookingTimeMax)
		{
			Add_CookingNumber(2 << (_uint)m_eCurrentCookingType);
			m_eCurrentModelType = FMT_BOILED;
			m_isFinishCooking = true;
			pCookingUI->Set_IsFinished(true);
			m_pColliderCom->Add_Actor();
			m_pModelCom[m_eCurrentModelType]->Set_Animation(13);

			//Reset_Setting_Cooking();
		}
	}
	else if (m_eCurrentCookingType == CookType::COOK_CUT)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		m_pTransformCom->Set_MyLook(XMVectorSet(0.f, 0.f, -1.f, 0.f));
		m_pColliderCom->SetGravity(false);
		m_pColliderCom->Reset_Power();
		m_pColliderCom->Update_TransformPhysX();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CChoppig_HandKnife* pHandKnifeObject = (CChoppig_HandKnife*)pGameInstance->Get_GameObject(SCENE_STAGE03, L"Layer_HandKnife", 0);
		if (pHandKnifeObject->Get_KnifeAnimFinished())
		{
			// Effect 생성 오제식 최제식 오대식이
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				SCENE_STAGE03, TEXT("Layer_PigChopRandingSmoke"),
				SCENE_STATIC, TEXT("Effect_PigChopRandingSmoke"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE03, TEXT("Layer_PigChopRandingSmoke"), XMVectorSet(185.f, 13.35f, 156.f, 1.f), true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			// Cutting 모델 변경위치
			Add_CookingNumber(2 << (_uint)m_eCurrentCookingType);
			m_eCurrentModelType = FMT_CUTTING;

			pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), m_iCookingUINumber)->Set_IsRender(true);
			m_isFinishCooking = true;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (m_eCurrentCookingType == CookType::COOK_BLEND)
	{
		m_lfCookingTimeAcc += TimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pColliderCom->Remove_Actor();
		pCookingUI->Set_TimeAcc(m_lfCookingTimeAcc);

		if (m_lfCookingTimeAcc > m_lfCookingTimeMax)
		{
			Add_CookingNumber(2 << (_uint)m_eCurrentCookingType);
			m_eCurrentModelType = FMT_BLENDED;
			m_isFinishCooking = true;
			pCookingUI->Set_IsFinished(true);
			m_pColliderCom->Add_Actor();
			m_pColliderCom->SetGravity(false);
			m_pColliderCom->Reset_Power();
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
			m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
			m_pColliderCom->Update_TransformPhysX();

			//Reset_Setting_Cooking();
		}

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		//m_pTransformCom->Set_MyLook(XMVectorSet(0.f, 0.f, -1.f, 0.f));
		//m_pColliderCom->SetGravity(false);
		//m_pColliderCom->Reset_Power();
		//m_pColliderCom->Update_TransformPhysX();
	}
	else if (m_eCurrentCookingType == CookType::COOK_BAKE)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vCookWarePosition));
		m_pColliderCom->Remove_Actor();
		pCookingUI->Set_TimeAcc(m_lfCookingTimeAcc);
	}
	
	return S_OK;
}

HRESULT CFruit::SetUp_BoneMatrices(const char * pBoneName,_uint iModelIndex)
{
	if (nullptr == m_pModelCom[iModelIndex])
	{
#ifdef _DEBUG
		MSGBOX("Failed to SetUp_BoneMatrices : Fruits : nullptr Model");
#endif // _DEBUG
		return E_FAIL;
	}

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	m_OffsetMatrix[iModelIndex] = m_pModelCom[iModelIndex]->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix[iModelIndex] = m_pModelCom[iModelIndex]->Get_PivotMatrix();
	m_pCombinedMatrix[iModelIndex] = m_pModelCom[iModelIndex]->Get_CombinedMatrixPtr(pBoneName);
	m_pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

_bool CFruit::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* Player = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (Player)
	{
		*pCurlingIndex = Player->Get_CurlingIndex();
		*pDeleteIndex = Player->Get_Delete_CurlingIndex();
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
}

void CFruit::Stage_RenderBranch(_int iPlayerCurlingIndex, CBoxCollider* pCollider)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
		case SCENE_STAGE03:
		{
			if (iPlayerCurlingIndex == m_iCurlingIndex)
			{
				if (nullptr != pCollider)
				{
					if (!m_isRender && pCollider)
						pCollider->Add_Actor();
				}
				m_isRender = true;
			}
			else
			{
				if (nullptr != pCollider)
				{
					if (m_isRender && pCollider)
					{
						pCollider->Remove_Actor();
					}
				}
				m_isRender = false;
			}
			break;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CFruit::Ready_Setting_Cooking(_uint iCookWareType)
{
	/* CookWareType Number */
	// 0 : Boiler 즉시 시작
	// 1 : Cutter 버튼 클릭시 시작
	// 2 : Blender 버튼 클릭시 시작
	// 3 : Baker 

	m_isReadyCooking = true;
	m_lfCookingTimeAcc = 0.0;
	m_lfCookingTimeMax = 30.0;

	if (iCookWareType != 0)
	{
		//커팅위치
		if (iCookWareType == 1)
		{
			m_vCookWarePosition = {185.f,13.35f,156.f};
			m_isStartCooking = true;
		}
		//블랜더위치
		if (iCookWareType == 2)
		{
			m_vCookWareHitPosition = STOREFLOAT3(m_pTransformCom->Get_Position());
			m_lfLerpToCookWareTimeAcc = 0.0;
			m_vCookWarePosition = { 281.13f,8.86f,150.82f };
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CBlender* pBlender = (CBlender*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender6"), 0);
			if (pBlender)
				pBlender->Set_Blender_Fruit(this);
			m_vCookWareCatmullRomEndPosition = { 273.27f,4.93f,157.39f };
			RELEASE_INSTANCE(CGameInstance);
			m_isReadyCooking = false;
			m_isStartCooking = false;
		}
		//프라이팬위치
		if (iCookWareType == 3)
		{
			m_vCookWareHitPosition = STOREFLOAT3(m_pTransformCom->Get_Position());
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CKitchenPan* pPan = (CKitchenPan*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_KitchenPan"), 0);
			if (pPan)
			{
				pPan->Set_IsCookFinished(false);
				pPan->Set_Pan_Fruit(this);
				pPan->Set_RenderFire();
			}
			pCookingUI = (CCookingProgressBarUI*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), m_iCookingUINumber);
			pCookingUI->Set_IsRender(true);
			pCookingUI->Set_MaxTime(10.f);
			m_lfCookingTimeMax = 10.0;
			m_isStartCooking = true;
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	else
	{
		//반신욕위치
		m_vCookWarePosition = {283.5f,13.f,202.f};
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pCookingUI = (CCookingProgressBarUI*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), m_iCookingUINumber);
		pCookingUI->Set_IsRender(true);
		pCookingUI->Set_MaxTime(30.f);
		RELEASE_INSTANCE(CGameInstance);

		m_isStartCooking = true;
	}


	m_eCurrentCookingType = CookType(iCookWareType+1);
}

void CFruit::Reset_Setting_Cooking()
{
	m_isReadyCooking = false;
	m_isStartCooking = false;
	m_isFinishCooking = false;
	m_lfCookingTimeAcc = 0.0;
	m_lfCookingTimeMax = 0.0;
	m_eCurrentCookingType = CookType::COOK_END;
}

_bool CFruit::Checking_FruitCooking(CookType eCookType,_uint* iUINumber)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_bool isCheck = false;
	_uint iPhaseNum = ((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"),0))->Get_Stage03PhaseNum();
	switch (iPhaseNum)
	{
	case 0:
		isCheck = ((CPlatingPlate_Phase0*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), iPhaseNum))->Checking_FruitCooking(m_eFruitType, eCookType,iUINumber);
		break;
	case 1:
		isCheck = ((CPlatingPlate_Phase1*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), iPhaseNum))->Checking_FruitCooking(m_eFruitType, eCookType,iUINumber);
		break;
	case 2:
		isCheck = ((CPlatingPlate_Phase2*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), iPhaseNum))->Checking_FruitCooking(m_eFruitType, eCookType,iUINumber);
		break;
	default:
		break;
	}

	if (isCheck)
	{
		if ((m_iCookingNumber &  (2 << (_uint)eCookType)) == (2 << (_uint)eCookType))
		{
			OutputDebugStringW(TEXT("Already Cooking By this CookWare"));
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return isCheck;

}

_float CFruit::Get_Camera_Distance()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float  fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - pGameInstance->Get_CamPosition()));
	RELEASE_INSTANCE(CGameInstance);
	return fDistance;
}

void CFruit::Free()
{
	__super::Free();
}