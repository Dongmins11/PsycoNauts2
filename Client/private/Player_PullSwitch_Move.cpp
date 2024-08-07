#include "stdafx.h"
#include "..\public\Player_PullSwitch_Move.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_PullSwitch.h"

IMPLEMENT_SINGLETON(CPlayer_PullSwitch_Move)

CPlayer_PullSwitch_Move::CPlayer_PullSwitch_Move()
{
}

void CPlayer_PullSwitch_Move::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == Key_Check_ChangeMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_PullSwitch_Move::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_PullSwitch_Move::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_PullSwitch_Move::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_PullSwitch_Move::Set_MoveDirection(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (PLAYER_PULL_SWITCH_FORWARD == m_iRootIndex)
		m_pTransform->Go_Straight(TimeDelta * 0.4, m_pCharacterController);
	else if (PLAYER_PULL_SWITCH_BACKWARD == m_iRootIndex)
		m_pTransform->Go_Backward(TimeDelta * 0.4, m_pCharacterController);

}

_bool CPlayer_PullSwitch_Move::Key_Check_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_PULL_SWITCH_BACKWARD == m_iRootIndex && 5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_PULL_SWITCH_BACKWARD))
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();

				m_pPlayer->Set_PullSwitchMove(true, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
				m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

				m_iUpperIndex = PLAYER_PULL_SWITCH_FORWARD;
				m_iRootIndex = PLAYER_PULL_SWITCH_FORWARD;
				m_iBottomIndex = PLAYER_PULL_SWITCH_FORWARD;
			}
			else if (PLAYER_PULL_SWITCH_BACKWARD != m_iRootIndex)
			{
				m_pPlayer->Set_PullSwitchMove(true, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
				m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

				m_iUpperIndex = PLAYER_PULL_SWITCH_FORWARD;
				m_iRootIndex = PLAYER_PULL_SWITCH_FORWARD;
				m_iBottomIndex = PLAYER_PULL_SWITCH_FORWARD;
			}

		}
		else if ('S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_PULL_SWITCH_FORWARD == m_iRootIndex && 5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_PULL_SWITCH_FORWARD))
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();

				m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
				m_pPlayer->Set_PullSwitchMove(true, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

				m_iUpperIndex = PLAYER_PULL_SWITCH_BACKWARD;
				m_iRootIndex = PLAYER_PULL_SWITCH_BACKWARD;
				m_iBottomIndex = PLAYER_PULL_SWITCH_BACKWARD;
			}
			else if (PLAYER_PULL_SWITCH_FORWARD != m_iRootIndex)
			{
				m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
				m_pPlayer->Set_PullSwitchMove(true, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

				m_iUpperIndex = PLAYER_PULL_SWITCH_BACKWARD;
				m_iRootIndex = PLAYER_PULL_SWITCH_BACKWARD;
				m_iBottomIndex = PLAYER_PULL_SWITCH_BACKWARD;
			}
		}
	}


	Set_MoveDirection(pPlayer_AnimState, TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PullSwitching.wav"), CHANNELID::PLAYER_PULLSWITCING)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	RELEASE_INSTANCE(CGameInstance);



	if (pAnimKeyDesc.iKeyCount == 0 && false == m_pModel->Get_IsRootLerp())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->StopSound(CHANNELID::PLAYER_PULLSWITCING);
		RELEASE_INSTANCE(CGameInstance);

		m_pPlayer->Set_PullSwitchMove(true, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
		m_pPlayer->Set_PullSwitchMove(true, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_PullSwitch::Get_Instance());
		CPlayer_PullSwitch::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}



	if (true == m_pPlayer->Get_PullSwitchingEnd())
	{
		Reset_Anim(pPlayer_AnimState);

		m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
		m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

		CPlayer_PullSwitch::Get_Instance()->Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		return true;
	}

	return false;
}

void CPlayer_PullSwitch_Move::Free()
{
}

