#include "stdafx.h"
#include "..\public\Player_GroundSlide.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"
#include "Player_StopAnim.h"
#include "Camera_Fly.h"

IMPLEMENT_SINGLETON(CPlayer_GroundSlide)

CPlayer_GroundSlide::CPlayer_GroundSlide()
{
}

void CPlayer_GroundSlide::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_MotionCheck();

	Effect_Create();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->PlaySoundOnce(TEXT("GroundSlide.wav"), CHANNELID::PLAYER_GROUND_SLIDESOUND)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	RELEASE_INSTANCE(CGameInstance);
	//if (pGameInstance->Key_Pressing(VK_NUMPAD0))
	//{
	//	m_fLookFixValueX += 0.1f;
	//}
	//if (pGameInstance->Key_Pressing(VK_NUMPAD1))
	//{
	//	m_fLookFixValueX -= 0.1f;
	//}
	//if (pGameInstance->Key_Pressing(VK_NUMPAD2))
	//{
	//	m_fLookFixValueY += 0.1f;
	//}
	//if (pGameInstance->Key_Pressing(VK_NUMPAD3))
	//{
	//	m_fLookFixValueY -= 0.1f;
	//}
	//if (pGameInstance->Key_Pressing(VK_NUMPAD4))
	//{
	//	m_fLookFixValueZ += 0.1f;
	//}
	//if (pGameInstance->Key_Pressing(VK_NUMPAD5))
	//{
	//	m_fLookFixValueZ -= 0.1f;
	//}

	//	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsLook_Fix(true, _float3(m_fLookFixValueX, m_fLookFixValueY, m_fLookFixValueZ));


	if (true == Key_SetUp_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Ground_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_GroundSlide::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	Player_Move_LookUpdate(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_GroundSlide::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bFristCheck = false;
}

void CPlayer_GroundSlide::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bFristCheck = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_GroundSlide::First_MotionCheck()
{
	if (false == m_bFristCheck)
	{
		m_fLookFixValueX = 0.5f;
		m_fLookFixValueY = 0.5f;
		m_fLookFixValueZ = 0.5f;

		m_iUpperIndex = PLAYER_GROUND_SLIDE;
		m_iRootIndex = PLAYER_GROUND_SLIDE;
		m_iBottomIndex = PLAYER_GROUND_SLIDE;

		m_bFristCheck = true;
	}
}

void CPlayer_GroundSlide::Player_Move_LookUpdate(CPlayer_AnimState * pPlayer_AnimState,const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_bIsMoveOn)
	{
		Handle_Movement(pAnimKeyDesc);
		SetUp_SlideLook(TimeDelta);
	}
}

_bool CPlayer_GroundSlide::Key_SetUp_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool bMoveCheck = false;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bMoveCheck = true;
		}
		else if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
			CPlayer_Jump::Get_Instance()->Set_GroundSlide(true);
			CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			if (nullptr != m_pEffect)
			{
				m_pEffect->Set_IsDestroy(true);
				Safe_Release(m_pEffect);
				m_pEffect = nullptr;
			}

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
			if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_GROUND_SLIDESOUND)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}
			RELEASE_INSTANCE(CGameInstance);

			return true; 
		}
	}

	m_bIsMoveOn = bMoveCheck;

	return false;
}

_bool CPlayer_GroundSlide::Ground_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_pPlayer->Get_IsSlide())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_GROUND_SLIDESOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}
		RELEASE_INSTANCE(CGameInstance);

		Reset_Anim(pPlayer_AnimState);

		pPlayer_AnimState->SetUp_State(CPlayer_StopAnim::Get_Instance());
		CPlayer_StopAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
		return true;
	}

	return false;
}

void CPlayer_GroundSlide::Effect_Create()
{
	if (nullptr == m_pEffect)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_SlidingEffect"),
			SCENE_STATIC, TEXT("Effect_Repeat_SlidingEffect"), &m_pEffect)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		Safe_AddRef(m_pEffect);

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, false, m_pTransform, TEXT("Layer_Repeat_SlidingEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer_GroundSlide::Free()
{
	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}

