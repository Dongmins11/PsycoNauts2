#include "stdafx.h"
#include "..\public\Player_LedgeJump.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Dodge.h"
#include "Player_Falling.h"

IMPLEMENT_SINGLETON(CPlayer_LedgeJump)

CPlayer_LedgeJump::CPlayer_LedgeJump()
{
}

void CPlayer_LedgeJump::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	m_bGravityCheck = true;


	First_Check_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Key_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == Land_To_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_To_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;


	Jump_To_StopMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Jump_To_Landing(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Animation_BlendCheck(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_LedgeJump::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == pPlayer_AnimState->Get_LedgeCheck() && PLAYER_LEDGE_GRAB_JUMP == m_iRootIndex)
		m_pTransform->Go_Straight(TimeDelta * 0.5, m_pCharacterController);

		m_pModel->Upper_Update(TimeDelta);
		m_pModel->Root_Update(TimeDelta);
		m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_LedgeJump::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;

	m_bJumpEndCheck = false;

	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_LedgeJump::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

_bool CPlayer_LedgeJump::Land_To_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex && m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
	{
		Reset_Anim(pPlayer_AnimState);
		Jump_Reset(pPlayer_AnimState, TimeDelta);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}

	return false;
}

_bool CPlayer_LedgeJump::Land_To_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex && 2 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
	{
		if (true == m_bMoveCheck)
		{
			Reset_Anim(pPlayer_AnimState);
			Jump_Reset(pPlayer_AnimState, TimeDelta);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

void CPlayer_LedgeJump::Animation_BlendCheck(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bFirstCheck)
	{
		if (m_iRootPreIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreIndex = m_iRootIndex;
		}
	}
}

void CPlayer_LedgeJump::Jump_Reset(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
	pPlayer_AnimState->Set_LedgeJumpCheck(false);
}

void CPlayer_LedgeJump::First_Check_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_fJumpPower = 8.f;

		m_iUpperIndex = PLAYER_LEDGE_GRAB_JUMP;
		m_iRootIndex = PLAYER_LEDGE_GRAB_JUMP;
		m_iBottomIndex = PLAYER_LEDGE_GRAB_JUMP;

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_LEDGE_GRAB_JUMP, 8.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_LEDGE_GRAB_JUMP, 8.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_LEDGE_GRAB_JUMP, 8.f);

		m_pModel->Set_UpperAnimBlendSpeed(PALYER_LANDING, 7.f);
		m_pModel->Set_RootAnimBlendSpeed(PALYER_LANDING, 7.f);
		m_pModel->Set_BottomAnimBlendSpeed(PALYER_LANDING, 7.f);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_LEDGE_JUMP_SOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("DoubleJump.wav"), CHANNELID::PLAYER_LEDGE_JUMP_SOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);


		m_bFirstCheck = true;
	}
}

void CPlayer_LedgeJump::Key_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
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
		if (PALYER_LANDING != m_iRootIndex)
		{
			if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				if (false == pPlayer_AnimState->Get_LedgeJumpCheck())
				{
					pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
					pPlayer_AnimState->Set_LandCheck(false);
					pPlayer_AnimState->Set_LedgeJumpCheck(true);

					_float4 vPos;
					XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
					pPlayer_AnimState->Set_BackUpY(vPos.y);
				}
			}
		}
	}

	m_bMoveCheck = bMoveCheck;
}

void CPlayer_LedgeJump::Jump_To_StopMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_LEDGE_GRAB_JUMP == m_iRootIndex && 10 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_LEDGE_GRAB_JUMP))
		m_pModel->Set_AnimSpeed(PLAYER_LEDGE_GRAB_JUMP, 1.f);

}

void CPlayer_LedgeJump::Jump_To_Landing(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_LandCheck())
	{
		m_iUpperIndex = PALYER_LANDING;
		m_iRootIndex = PALYER_LANDING;
		m_iBottomIndex = PALYER_LANDING;

		pPlayer_AnimState->Set_LandCheck(false);
	}
}




void CPlayer_LedgeJump::Free()
{
}

