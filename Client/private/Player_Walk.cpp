#include "stdafx.h"
#include "..\public\Player_Walk.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_WalkStop.h"


IMPLEMENT_SINGLETON(CPlayer_Walk)

CPlayer_Walk::CPlayer_Walk()
{
}

void CPlayer_Walk::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		Setting_Blend_Speed();

		m_fMoveOffset = 0.5f;
		m_bFirstCheck = true;
	}

	_bool bMoveCheck = false;
	_uint iCount = 0;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (true == pPlayer_AnimState->Get_WalkingCheck())
			{
				m_iAnimIndex = PLAYER_WALK;
			}
			else
			{
				Reset_Anim(pPlayer_AnimState);
				ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));

				CPlayer_IdleAnim::Get_Instance()->Walk_to_Run_SettingCheck(true);
				pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
				CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return;
			}

			bMoveCheck = true;
		}
	}

	if (0 == pAnimKeyDesc.iKeyCount && false == m_pModel->Get_IsAnimLerp())
	{
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_OneLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_WalkStop::Get_Instance());
		CPlayer_WalkStop::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	m_pModel->Set_Animation(m_iAnimIndex);

	m_bIsMoveOn = bMoveCheck;
}

void CPlayer_Walk::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		Handle_Movement(pAnimKeyDesc);
		Player_Move_NonMoveOffset(TimeDelta * 0.6);
	}

	m_pModel->Lerp_Update(TimeDelta);
}

void CPlayer_Walk::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	//pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Walk::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_Walk::Walk_to_Effect_Create(_double TimeDelta)
{
	if (PLAYER_WALK == m_iRootIndex)
	{
		if (5 <= m_pModel->Get_AnimationCurrentFrame(PLAYER_WALK) && 7 >= m_pModel->Get_AnimationCurrentFrame(PLAYER_WALK))
		{
			//이펙트생성위치
			//오른발
			_uint i = 0;
		}
		else if (18 <= m_pModel->Get_AnimationCurrentFrame(PLAYER_WALK) && 20 >= m_pModel->Get_AnimationCurrentFrame(PLAYER_WALK))
		{
			//이펙트생성위치
			//왼발
			_uint i = 0;
		}
	}
}

void CPlayer_Walk::Setting_Blend_Speed()
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_WALK, 6.0);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_WALK, 6.0);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_WALK, 6.0);
	m_pModel->Set_AnimBlendSpeed(PLAYER_WALK, 6.0);

}

void CPlayer_Walk::Free()
{
}

