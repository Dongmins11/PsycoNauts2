#include "stdafx.h"
#include "..\public\Player_GrabBack.h"
#include "Player_IdleAnim.h"

IMPLEMENT_SINGLETON(CPlayer_GrabBack)

CPlayer_GrabBack::CPlayer_GrabBack()
{
}

void CPlayer_GrabBack::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iUpperIndex = PLAYER_GRAP_ATTACK_DRAW;
		m_iRootIndex = PLAYER_GRAP_ATTACK_DRAW;
		m_iBottomIndex = PLAYER_GRAP_ATTACK_DRAW;

		m_bFirstCheck = true;
	}

	if (true == m_pModel->Get_IsUpperAnimFinishied(PLAYER_GRAP_ATTACK_DRAW))
	{
		m_iUpperIndex = PLAYER_GRAP_ATTACK_DRAW_LOOP_STATIC;
		m_iRootIndex = PLAYER_GRAP_ATTACK_DRAW_LOOP_STATIC;
		m_iBottomIndex = PLAYER_GRAP_ATTACK_DRAW_LOOP_STATIC;
	}

	if (true == pPlayer_AnimState->Get_GrabSuccessCheck() && false == m_bEndCheck)
	{
		m_iUpperIndex = PLAYER_GRAP_ATTACK_DRAW_OUT;
		m_iRootIndex = PLAYER_GRAP_ATTACK_DRAW_OUT;
		m_iBottomIndex = PLAYER_GRAP_ATTACK_DRAW_OUT;

		m_bEndCheck = true;
	}

	if (PLAYER_GRAP_ATTACK_DRAW_OUT == m_iRootIndex)
	{
		if (m_pModel->Get_IsRootAnimFinishied(PLAYER_GRAP_ATTACK_DRAW_OUT))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_GrabBack::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//m_bIsMoveOn = true;
	//Move_Acceleration(TimeDelta * 2.f);
	//Handle_Movement(pAnimKeyDesc);
	//Player_Move(TimeDelta);

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_GrabBack::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_GrabCheck(false);
	pPlayer_AnimState->Set_GrabBack(false);
	pPlayer_AnimState->Set_GrabSuccess(false);
	m_bEndCheck = false;
	m_bFirstCheck = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_GrabBack::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_GrabCheck(false);
	pPlayer_AnimState->Set_GrabBack(false);
	pPlayer_AnimState->Set_GrabSuccess(false);
	m_bEndCheck = false;
	m_bFirstCheck = false;
}

void CPlayer_GrabBack::Free()
{
}

