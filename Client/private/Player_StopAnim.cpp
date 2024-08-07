#include "stdafx.h"
#include "..\public\Player_StopAnim.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Player_TrampolineJump.h"
IMPLEMENT_SINGLETON(CPlayer_StopAnim)

CPlayer_StopAnim::CPlayer_StopAnim()
{
}

void CPlayer_StopAnim::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_IsFixY(false);
	RELEASE_INSTANCE(CGameInstance);

	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_STOP)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerStop.wav"), CHANNELID::PLAYER_STOP)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
		m_bFirstCheck = true;
	}

	m_lfMoveSpeed = 0.5f;

	if (true == Key_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;


	if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
	{
		if (true == m_pModel->Get_IsUpperAnimFinishied(PLAYER_GRAP_ATTACK_START))
		{
			if (false == m_pModel->Get_IsUpperLerp())
			{
				m_bHandCheck = true;
				pPlayer_AnimState->Set_UpperLerpCheck(true);
				m_iUpperIndex = PLAYER_IDLE;
			}

		}
		else
			m_iUpperIndex = PLAYER_GRAP_ATTACK_START;
	}
	else
		m_iUpperIndex = PLAYER_RUN_STOP;

	m_iRootIndex = PLAYER_RUN_STOP;
	m_iBottomIndex = PLAYER_RUN_STOP;


	if (FAILED(Land_to_Trampoline(pPlayer_AnimState, pAnimKeyDesc, TimeDelta)))
		return;

	if (true == m_pModel->Get_IsBottomAnimFinishied(m_iBottomIndex))
	{
		//if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
		//	pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
		if (true == m_bHandCheck)
		{
			pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
			m_bHandCheck = false;
		}
		else
			pPlayer_AnimState->Set_AllLerpCheckTrue();
		m_bFirstCheck = false;
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_StopAnim::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bIsMoveOn = false;
	Move_Acceleration(TimeDelta * 1.5f);
	m_pTransform->Go_Straight(TimeDelta * m_lfMoveSpeed * m_fMoveOffset,m_pCharacterController);

	m_pModel->Upper_Update(TimeDelta * 1.5);

	if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
		m_pModel->Root_Update(TimeDelta * 1.5, true);
	else
		m_pModel->Root_Update(TimeDelta * 1.5);

	m_pModel->Bottom_Update(TimeDelta * 1.5);
}

void CPlayer_StopAnim::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_StopAnim::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

_bool CPlayer_StopAnim::Key_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_RUN_STOP == m_iRootIndex && 5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_STOP))
			{
				m_bFirstCheck = false;
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
				CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
		else if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			if (PLAYER_RUN_STOP == m_iRootIndex && 3 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_STOP))
			{
				m_bFirstCheck = false;
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
				CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_IsFixY(true);
				RELEASE_INSTANCE(CGameInstance);
				return true;
			}
		}
	}

	return false;
}

_bool CPlayer_StopAnim::Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_TrampolineTriggerCheck(false);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_TrampolineJump::Get_Instance());
		CPlayer_TrampolineJump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.8f, 0.1f, 8.f, 9.f), 0.2f, 1.f);
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	return false;
}

void CPlayer_StopAnim::Free()
{
}

