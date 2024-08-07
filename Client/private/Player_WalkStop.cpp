#include "stdafx.h"
#include "..\public\Player_WalkStop.h"
#include "Player_IdleAnim.h"
#include "Player_Walk.h"


IMPLEMENT_SINGLETON(CPlayer_WalkStop)

CPlayer_WalkStop::CPlayer_WalkStop()
{
}

void CPlayer_WalkStop::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iAnimIndex = PLAYER_WALK_STOP;

		Setting_Blend_Speed();

		m_bFirstCheck = true;
	}

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_WALK_STOP == m_iAnimIndex && 5 <= m_pModel->Get_AnimationCurrentFrame(PLAYER_WALK_STOP))
			{
				m_bFirstCheck = false;
				pPlayer_AnimState->Set_OneLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Walk::Get_Instance());
				CPlayer_Walk::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return;
			}
		}
	}

	if (true == m_pModel->Get_IsAnimFinishied(m_iAnimIndex))
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	m_pModel->Set_Animation(m_iAnimIndex);
}

void CPlayer_WalkStop::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Lerp_Update(TimeDelta);
}

void CPlayer_WalkStop::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_WalkStop::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_WalkStop::Setting_Blend_Speed()
{
	m_pModel->Set_AnimBlendSpeed(PLAYER_WALK_STOP, 6.0);
}

void CPlayer_WalkStop::Free()
{
}

