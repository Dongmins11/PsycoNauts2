#include "stdafx.h"
#include "..\public\Player_Throw_Right.h"
#include "Player_IdleAnim.h"
#include "Player_Food_Throw.h"

IMPLEMENT_SINGLETON(CPlayer_Throw_Right)

CPlayer_Throw_Right::CPlayer_Throw_Right()
{
}

void CPlayer_Throw_Right::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	Set_First_State(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == Throw_Idle_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Throw_Right::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Throw_Right::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
}

void CPlayer_Throw_Right::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
}

void CPlayer_Throw_Right::Set_First_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iUpperIndex = 160;
		m_iRootIndex = 160;
		m_iBottomIndex = 160;

		m_bFirstCheck = true;
	}
}

_bool CPlayer_Throw_Right::Throw_Idle_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_pModel->Get_IsRootAnimFinishied(160))
	{
		Reset_Anim(pPlayer_AnimState);
		
		pPlayer_AnimState->SetUp_State(CPlayer_Food_Throw::Get_Instance());
		CPlayer_Food_Throw::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		return true;
	}
	return false;
}

void CPlayer_Throw_Right::Free()
{
}

