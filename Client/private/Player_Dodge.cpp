#include "stdafx.h"
#include "..\public\Player_Dodge.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Slider.h"
#include "Player_Ledge.h"

IMPLEMENT_SINGLETON(CPlayer_Dodge)

CPlayer_Dodge::CPlayer_Dodge()
{
}

void CPlayer_Dodge::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_Check_AnimMotion(pPlayer_AnimState, TimeDelta);

	Dodge_Loop_MotionCheck(pPlayer_AnimState, TimeDelta);

	pPlayer_AnimState->PhysX_Slider();

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
		if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			if (false == pPlayer_AnimState->Get_JumpCheck())
			{
				Reverse_Look(TimeDelta);
				pPlayer_AnimState->Set_JumpPower(8.f);
				pPlayer_AnimState->Set_LandCheck(false);
				pPlayer_AnimState->Set_JumpCheck(true);
			}
		}
	}


	Falling_MotionCheck(pPlayer_AnimState, TimeDelta);

	Landing_MotionCheck(pPlayer_AnimState, TimeDelta);

	if (true == Dodge_to_Ledge_Grab(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Dodge_to_Slider(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Landing_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Landing_to_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;


	if (true == m_bFirstCheck)
	{
		if (m_iRootPreIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreIndex = m_iRootIndex;
		}
	}

	m_bIsMoveOn = bMoveCheck;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Dodge::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING != m_iRootIndex)
		Player_Look_Reverse_Move(TimeDelta * 2.0);

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Dodge::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_bGravityCheck = false;

	m_eCurrentLookState = LOOK_STATE::LOOK_END;

	pPlayer_AnimState->Set_AllLerpCheckTrue();

	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.8);
	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
}

void CPlayer_Dodge::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_Dodge::First_Check_AnimMotion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_DODGE_LEFT;
			m_iRootIndex = PLAYER_DODGE_LEFT;
			m_iBottomIndex = PLAYER_DODGE_LEFT;
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_DODGE_RIGHT;
			m_iRootIndex = PLAYER_DODGE_RIGHT;
			m_iBottomIndex = PLAYER_DODGE_RIGHT;
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_DODGE_LEFT;
			m_iRootIndex = PLAYER_DODGE_LEFT;
			m_iBottomIndex = PLAYER_DODGE_LEFT;
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_DODGE_RIGHT;
			m_iRootIndex = PLAYER_DODGE_RIGHT;
			m_iBottomIndex = PLAYER_DODGE_RIGHT;
		}


		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_DODGE_LOOP_RIGHT, 6.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_DODGE_LOOP_RIGHT, 6.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_DODGE_LOOP_RIGHT, 6.f);

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_DODGE_LOOP_RIGHT, 6.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_DODGE_LOOP_RIGHT, 6.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_DODGE_LOOP_RIGHT, 6.f);

		pPlayer_AnimState->Set_Fall_AccelerationSpeed(2.0);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_DODGE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerDodgeJump.wav"), CHANNELID::PLAYER_DODGE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_bGravityCheck = true;
		m_bFirstCheck = true;
	}
}

void CPlayer_Dodge::Dodge_Loop_MotionCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState || LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
	{
		if (m_iRootIndex == PLAYER_DODGE_LEFT && 8 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_DODGE_LEFT))
		{
			m_iUpperIndex = PLAYER_DODGE_LOOP_LEFT;
			m_iRootIndex = PLAYER_DODGE_LOOP_LEFT;
			m_iBottomIndex = PLAYER_DODGE_LOOP_LEFT;
		}
	}
	else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState || LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
	{
		if (m_iRootIndex == PLAYER_DODGE_RIGHT && 8 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_DODGE_RIGHT))
		{
			m_iUpperIndex = PLAYER_DODGE_LOOP_RIGHT;
			m_iRootIndex = PLAYER_DODGE_LOOP_RIGHT;
			m_iBottomIndex = PLAYER_DODGE_LOOP_RIGHT;
		}
	}
}

void CPlayer_Dodge::Falling_MotionCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DODGE_LOOP_RIGHT) || true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DODGE_LOOP_LEFT))
	{
		if (true == pPlayer_AnimState->Get_FallCheck() && false == pPlayer_AnimState->Get_LandCheck())
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;
		}
	}
}

void CPlayer_Dodge::Landing_MotionCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_LandCheck())
	{
		m_iUpperIndex = PALYER_LANDING;
		m_iRootIndex = PALYER_LANDING;
		m_iBottomIndex = PALYER_LANDING;

		pPlayer_AnimState->Set_LandCheck(false);
	}
}

_bool CPlayer_Dodge::Landing_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	if (PALYER_LANDING == m_iRootIndex && 3 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
	{
		if (true == m_bIsMoveOn)
		{
			m_iUpperIndex = 0;
			m_iRootIndex = 0;
			m_iBottomIndex = 0;

			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

_bool CPlayer_Dodge::Landing_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex)
	{

		if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
		{
			m_iUpperIndex = 0;
			m_iRootIndex = 0;
			m_iBottomIndex = 0;

			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}

	}

	return false;
}

_bool CPlayer_Dodge::Dodge_to_Slider(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_Slider())
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			Reset_Anim(pPlayer_AnimState);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_RIGHT);

			m_eCurrentLookState = LOOK_STATE::LOOK_END;

			pPlayer_AnimState->SetUp_State(CPlayer_Slider::Get_Instance());
			CPlayer_Slider::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			Reset_Anim(pPlayer_AnimState);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_BACK);

			m_eCurrentLookState = LOOK_STATE::LOOK_END;

			pPlayer_AnimState->SetUp_State(CPlayer_Slider::Get_Instance());
			CPlayer_Slider::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			Reset_Anim(pPlayer_AnimState);


			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_LEFT);

			m_eCurrentLookState = LOOK_STATE::LOOK_END;

			pPlayer_AnimState->SetUp_State(CPlayer_Slider::Get_Instance());
			CPlayer_Slider::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			Reset_Anim(pPlayer_AnimState);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_FRONT);

			m_eCurrentLookState = LOOK_STATE::LOOK_END;

			pPlayer_AnimState->SetUp_State(CPlayer_Slider::Get_Instance());
			CPlayer_Slider::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
	}

	return false;
}

_bool CPlayer_Dodge::Dodge_to_Double_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	return _bool();
}

_bool CPlayer_Dodge::Dodge_to_Ledge_Grab(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_DODGE_LOOP_LEFT == m_iRootIndex || PLAYER_DODGE_LOOP_RIGHT == m_iRootIndex)
	{
		if (true == m_pPlayer->Get_IsParkour())
		{
			if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
			{
				Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

				return true;
			}
			else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
			{
				Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

				return true;
			}
			else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
			{
				Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

				return true;
			}
			else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
			{
				Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

				return true;
			}
		}
	}

	return false;
}

void CPlayer_Dodge::Ledge_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	Reset_Anim(pPlayer_AnimState);

	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->SetUp_State(CPlayer_Ledge::Get_Instance());
	CPlayer_Ledge::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
}


void CPlayer_Dodge::Free()
{
}

