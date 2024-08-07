#include "stdafx.h"
#include "..\public\Player_GrabAttack.h"
#include "Player_IdleAnim.h"

IMPLEMENT_SINGLETON(CPlayer_GrabAttack)
 
CPlayer_GrabAttack::CPlayer_GrabAttack()
{
}

void CPlayer_GrabAttack::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_bIsMoveOn = true;
		}
	}

	m_iUpperIndex = PLAYER_GRAP_ATTACK_CAST;
	m_iRootIndex = PLAYER_GRAP_ATTACK_CAST;
	m_iBottomIndex = PLAYER_GRAP_ATTACK_CAST;

	if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_GRAP_ATTACK_CAST))
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_GrabAttack::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		m_fMoveOffset = 1.f;
		Handle_Movement(pAnimKeyDesc);
		Player_Move(TimeDelta * 0.3f);
	}
	
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_GrabAttack::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_GrabCheck(false);
	pPlayer_AnimState->Set_GrabAttack(false);
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_GrabAttack::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_GrabCheck(false);
	pPlayer_AnimState->Set_GrabAttack(false);
}

void CPlayer_GrabAttack::Free()
{
}

