#include "stdafx.h"
#include "..\public\Player_Hit.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_BrainStom.h"
#include "Player_Pisblast.h"
#include "Player_Dash.h"
#include "Player_ComboAttack.h"
#include "Player_Jump.h"
#include "Player_Ball.h"

IMPLEMENT_SINGLETON(CPlayer_Hit)

CPlayer_Hit::CPlayer_Hit()
{
}

void CPlayer_Hit::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool bMoveCheck = false;


	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_iUpperIndex = PLAYER_HIT_BACK;
			m_iRootIndex = PLAYER_RUN;
			m_iBottomIndex = PLAYER_RUN;
			bMoveCheck = true;
		}
	}


	if (true == Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;


	if (0 == pAnimKeyDesc.iKeyCount && false == m_pModel->Get_IsUpperLerp())
	{
		m_iUpperIndex = PLAYER_HIT_BACK;
		m_iRootIndex = PLAYER_HIT_BACK;
		m_iBottomIndex = PLAYER_HIT_BACK;
	}

	m_bIsMoveOn = bMoveCheck;


	if (true == End_to_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;



	if (m_iPreUpperIndex != m_iUpperIndex)
	{
		pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
		m_iPreUpperIndex = m_iUpperIndex;
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Hit::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		Move_Acceleration(TimeDelta * 2.f);
		Handle_Movement(pAnimKeyDesc);
		Player_Move(TimeDelta);
	}

	m_pModel->Root_Update(TimeDelta);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Hit::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	//pPlayer_AnimState->Set_HitCheck(false);
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}


void CPlayer_Hit::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_Hit::First_HitMotionCheck(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
}

_bool CPlayer_Hit::Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if (false == m_pModel->Get_IsUpperLerp())
		{
			if (true == m_bIsMoveOn)
			{
				if (VK_SHIFT == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
				{
					pPlayer_AnimState->Set_OneLerpCheckTrue();
					pPlayer_AnimState->SetUp_State(CPlayer_Dash::Get_Instance());
					CPlayer_Dash::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
					return true;
				}

			}

			if ('E' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->SetUp_State(CPlayer_Pisblast::Get_Instance());
				CPlayer_Pisblast::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;

			}
			else if ('Q' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->SetUp_State(CPlayer_BrainStom::Get_Instance());
				CPlayer_BrainStom::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
			else if (pAnimKeyDesc.iKey[i] == 'R' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->SetUp_State(CPlayer_Ball::Get_Instance());
				CPlayer_Ball::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
			else if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
				CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
			else if (VK_LBUTTON == pAnimKeyDesc.iOnceKey)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->SetUp_State(CPlayer_ComboAttack::Get_Instance());
				CPlayer_ComboAttack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
	}

	return false;
}

_bool CPlayer_Hit::End_to_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_iUpperIndex == PLAYER_HIT_BACK && true == m_pModel->Get_IsUpperAnimFinishied(PLAYER_HIT_BACK))
	{
		if (true == m_bIsMoveOn)
		{

			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->Set_MoveOffset_ChangeCheck(true);
			CPlayer_RunAnim::Get_Instance()->Set_PlayerMoveAccelecrtaion(1.f);
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
		else
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

void CPlayer_Hit::Free()
{
}

