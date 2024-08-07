#include "stdafx.h"
#include "..\public\Player_BrainWashing.h"
#include "Player_IdleAnim.h"
#include "Camera_Fly.h"



IMPLEMENT_SINGLETON(CPlayer_BrainWashing)

CPlayer_BrainWashing::CPlayer_BrainWashing()
{
}

void CPlayer_BrainWashing::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	_bool	bKeyCheck = false;

	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("BrainWashing.wav"), CHANNELID::PLAYER_SKILLE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		m_bFirstCheck = true;
	}

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('T' == pAnimKeyDesc.iKey[i] && CPlayer_AnimState::KEY_PRESING == pAnimKeyDesc.eKeyState[i])
		{
			m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
			m_iRootIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
			m_iBottomIndex = PLAYER_PSIBLAST_CHARGE_LOOP;

			bKeyCheck = true;
		}
	}

	if (true == bKeyCheck)
	{
		m_lfTimeAcc += TimeDelta;
		Lay_Cast_BrainWashing_Check();
	}

	if (false == bKeyCheck)
	{
		m_lfTimeAcc = 0.0;
		LayObject_Rlease();
		Reset_Anim(pPlayer_AnimState);
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_ConnectRect"));
		if (0 < iSize)
		{
			if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_ConnectRect"), 0)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

		}
		//iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BrainWasing_Trail"));
		//if (0 < iSize)
		//{
		//	if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BrainWasing_Trail"), 0)))
		//	{
		//		RELEASE_INSTANCE(CGameInstance);
		//		return;
		//	}
		//}
		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_SKILLE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}


		RELEASE_INSTANCE(CGameInstance);

		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}
	if (true == bKeyCheck && 4.f <= m_lfTimeAcc)
	{
		BrainWashing();
		m_lfTimeAcc = 0.0;
		Reset_Anim(pPlayer_AnimState);
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_ConnectRect"));
		if (0 < iSize)
		{
			if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_ConnectRect"), 0)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		//iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BrainWasing_Trail"));
		//if (0 < iSize)
		//{
		//	if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BrainWasing_Trail"), 0)))
		//	{
		//		RELEASE_INSTANCE(CGameInstance);
		//		return;
		//	}
		//}


		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_SKILLE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	if (false == m_bEffectCreate && true == bKeyCheck && nullptr != m_pRayObject)
	{
		m_bEffectCreate = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


		/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_ConnectRect"),
			SCENE_STATIC, TEXT("Effect_Repeat_ConnectRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		CTransform* pObjectTrans = (CTransform*)m_pRayObject->Get_Components(L"Com_Transform");

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, pObjectTrans, TEXT("Layer_Repeat_ConnectRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_vector vPos = XMVectorSet(0.0f, 0.5f, 0.f, 0.f);
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_ConnectRect"), TEXT("Com_Option"), vPos);


		//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Bulb_Trail"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BrainWasing_Trail"), nullptr)))
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}

		//if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_BrainWasing_Trail"))))
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}

		RELEASE_INSTANCE(CGameInstance);
	}

	m_bKeyCheck = bKeyCheck;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_BrainWashing::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_BrainWashing::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bKeyCheck = false;
	m_bFirstCheck = false;
	m_bEffectCreate = false;
}

void CPlayer_BrainWashing::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_BrainWashing::Animation_BlendSpeedSet(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{

}

void CPlayer_BrainWashing::Create_Brain_Washing_Effect(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
}

void CPlayer_BrainWashing::BrainWashing()
{
	if (nullptr != m_pRayObject)
	{
		CGameInstance* pGamInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGamInstance);

		CGameObject* pCamObject = pGamInstance->Get_GameObject(SCENE_STATIC, L"Layer_Camera", 0);
		CTransform* pObjectTrans = (CTransform*)m_pRayObject->Get_Components(L"Com_Transform");

		if (nullptr == pCamObject || nullptr == pObjectTrans)
		{
			MSGBOX("Failed to Finder Camera or RayObject : CPlayer_BrainWashing : Funtion BrainWashing");
			Safe_Release(pGamInstance);
			return;
		}
		
		if (FAILED(pGamInstance->PlaySoundOnce(TEXT("CutSceneMouseSound.wav"), CHANNELID::MOUSE_CUTSCENE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		Safe_Release(pGamInstance);

		dynamic_cast<CCamera_Fly*>(pCamObject)->Set_Camera_Transform_Ptr(CCamera_Fly::CAMERATYPE::CAMERATYPE_TEST, pObjectTrans->Get_WorldMatrixPtr());
		
		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGamInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		if (nullptr == pRenderer)
		{
			MSGBOX("Failed to Finder Camera or RayObject : CPlayer_BrainWashing : Funtion BrainWashing");
			Safe_Release(pGamInstance);
			return;
		}
		pRenderer->Set_IsSSAA(false);
		m_pPlayer->Set_RenderCheck(true);


		m_pRayObject->Set_IsPicking(true);
		m_pRayObject = nullptr;
	}
}

void CPlayer_BrainWashing::LayObject_Rlease()
{
	if (nullptr != m_pRayObject)
	{
		m_pRayObject->Set_IsPicking(false);
		m_pRayObject = nullptr;
	}
}

void CPlayer_BrainWashing::Lay_Cast_BrainWashing_Check()
{
	if (nullptr == m_pRayObject)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		const _tchar* pLayerTag;

		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		XMStoreFloat3(&tDescIn.vOrigin, m_pTransform->Get_Position());
		XMStoreFloat3(&tDescIn.vUnitDir,XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK)));
		tDescIn.fDistance = 50.f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PICK);

		tDescOut.ppHitObject = &m_pRayObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (m_pRayObject)
			{
				//Setting_Throwing
				pLayerTag = m_pRayObject->Get_LayerName();
			}
		}

		Safe_Release(pGameInstance);
	}
}

void CPlayer_BrainWashing::Free()
{
}

