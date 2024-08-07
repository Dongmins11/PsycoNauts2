#include "stdafx.h"
#include "..\public\Player_Floting.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"

IMPLEMENT_SINGLETON(CPlayer_Floting)

CPlayer_Floting::CPlayer_Floting()
{
}

void CPlayer_Floting::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_Check(pPlayer_AnimState);

	_uint iUpCount = 0;
	_uint iPreSingCount = 0;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_bIsMoveOn = true;
			++iPreSingCount;
		}

		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_UP ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_UP ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_UP ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_UP)
		{
			++iUpCount;
		}


		if ('R' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_FALLING != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
			{
				pPlayer_AnimState->Set_FloatingSpeed(0.007f);
				m_bLoopCheck = true;
			}
		}
	}

	if ('R' == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_UP)
		m_bLoopCheck = false;

	if (iPreSingCount == iUpCount)
		m_bIsMoveOn = false;


	Floating_LoopMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Floating_FallMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Floating_LandMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Effect_Release(pPlayer_AnimState);

	if (true == Floating_Land_to_idle_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Floating_Land_to_Run_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;



	if (PLAYER_FLOATING_START != m_iRootIndex)
	{
		if (m_iRootPreMotionIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreMotionIndex = m_iRootIndex;
		}
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Floting::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		Handle_Movement(pAnimKeyDesc);
		Player_Move_NonMoveOffset(TimeDelta);
	}

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Floting::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_FloatingSpeed(1.f);

	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;
	m_bLoopCheck = false;
	m_bIsMoveOn = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Floting::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_FloatingSpeed(1.f);

	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;
	m_bLoopCheck = false;
	m_bIsMoveOn = false;

	//if()//여기서 지우ㅗㅓ줘여ㅑ함 이펙ㅌ가있을시에

}

void CPlayer_Floting::First_Check(CPlayer_AnimState * pPlayer_AnimState)
{
	if (false == m_bFirstCheck)
	{

		m_iUpperIndex = PLAYER_FLOATING_START;
		m_iRootIndex = PLAYER_FLOATING_START;
		m_iBottomIndex = PLAYER_FLOATING_START;

		m_lfMoveSpeed = 0.5;
		m_bFirstCheck = true;

		Effect_Create(pPlayer_AnimState);
	}
}

void CPlayer_Floting::Floating_LoopMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_FLOATING_START == m_iRootIndex && true == m_bLoopCheck)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_FLOATING_START))
		{
			m_iUpperIndex = PLAYER_FLOATING_LOOP;
			m_iRootIndex = PLAYER_FLOATING_LOOP;
			m_iBottomIndex = PLAYER_FLOATING_LOOP;
		}
	}

}

void CPlayer_Floting::Floating_FallMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_FLOATING_LOOP == m_iRootIndex && true == m_bLoopCheck)
	{
		m_lfFlotingTimeAcc += TimeDelta;

		if (5.0 <= m_lfFlotingTimeAcc)
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;

			m_lfFlotingTimeAcc = 0.0;
			pPlayer_AnimState->Set_FloatingSpeed(1.f);
		}
	}

	if (PLAYER_FALLING != m_iRootIndex && false == m_bLoopCheck)
	{
		if (PALYER_LANDING != m_iRootIndex)
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;

			pPlayer_AnimState->Set_FloatingSpeed(1.f);


		}
	}
	
}

void CPlayer_Floting::Floating_LandMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING != m_iRootIndex && false == pPlayer_AnimState->Get_FloatingCheck())
	{
		m_iUpperIndex = PALYER_LANDING;
		m_iRootIndex = PALYER_LANDING;
		m_iBottomIndex = PALYER_LANDING;
	}
}

_bool CPlayer_Floting::Floating_Land_to_idle_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_Floting::Floating_Land_to_Run_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex)
	{
		if (true == m_bIsMoveOn)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

void CPlayer_Floting::Effect_Create(CPlayer_AnimState * pPlayer_AnimState)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == m_pEffect)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Balloon"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Balloon"), &m_pEffect, nullptr)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		Safe_AddRef(m_pEffect);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_Floting::Effect_Release(CPlayer_AnimState * pPlayer_AnimState)
{
	if (PLAYER_FLOATING_START != m_iRootIndex && PLAYER_FLOATING_LOOP != m_iRootIndex)
	{
		if (nullptr != m_pEffect)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_FLOOTING_END)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("BalloonEnd.wav"), CHANNELID::PLAYER_FLOOTING_END)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			RELEASE_INSTANCE(CGameInstance);


			m_pEffect->Set_IsInteraction(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
	}
}

void CPlayer_Floting::Free()
{
	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}

