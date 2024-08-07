#include "stdafx.h"
#include "..\public\Player_KitchenButton.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"

IMPLEMENT_SINGLETON(CPlayer_KitchenButton)

CPlayer_KitchenButton::CPlayer_KitchenButton()
{
}

void CPlayer_KitchenButton::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;

	First_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);	// FSM Handle Input이 불렸을 때 최초 한번만 불리는 함수

	Start_Jump(pPlayer_AnimState, TimeDelta);

	Key_Check_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	LandTimeCheck(pPlayer_AnimState,TimeDelta);

	if (true == RepeatJump(pPlayer_AnimState))
		return;

	Fall_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Land_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == KitchenButton_to_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == KitchenButton_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == m_bFirstCheck)
	{
		if (m_iRootPreIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreIndex = m_iRootIndex;
		}
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_KitchenButton::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_KitchenButton::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bLandChecking = false;
	m_bFirstCheck = false;
	m_iJumpCount = 0;

	m_dLandTimeAcc = 0.0;

	for (_uint i = 0; i < 3; i++)
		m_pPlayer->Set_IsKitchenButton(false, i);

	Jump_Initialize(pPlayer_AnimState);
}

void CPlayer_KitchenButton::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);

	m_iJumpCount = 0;

	m_dLandTimeAcc = 0.0;

	m_bFirstCheck = false;

	for (_uint i = 0; i < 3; i++)
		m_pPlayer->Set_IsKitchenButton(false, i);

	Jump_Initialize(pPlayer_AnimState);
}

void CPlayer_KitchenButton::First_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iJumpCount = 0;
		m_bBeforeJumping = true;
		m_bLandChecking = false;
		m_dLandTimeAcc = 0.0;
		//m_fJumpPower = 6.85f;

		m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
		m_iRootIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
		m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_LOOP;

		m_iRootPreIndex = PLAYER_TRAMPOLINE_LOW_LOOP;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_TRAMPOLINE_TWO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("JumpOne.wav"), CHANNELID::PLAYER_TRAMPOLINE_TWO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);


		m_bFirstCheck = true;
	}
}

void CPlayer_KitchenButton::Key_Check_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool	bMoveCheck = false;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bMoveCheck = true;
		}
		//if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		//{
		//	//if (0 == m_iJumpCount && false == pPlayer_AnimState->Get_JumpCheck())
		//	//{
		//	//	SetUp_One_Jump(pPlayer_AnimState, TimeDelta);
		//	//	//if (false == m_bBeforeJumping)
		//	//	//	SetUp_Double_Jump(pPlayer_AnimState, TimeDelta);
		//	//}
		//}
	}

	m_bIsMoveOn = bMoveCheck;
}

void CPlayer_KitchenButton::Fall_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//if (PLAYER_TRAMPOLINE_ENTER == m_iRootIndex && 4 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_TRAMPOLINE_ENTER))
	//{
	//	if (PLAYER_TRAMPOLINE_HIGH_LOOP != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
	//	{
	//		m_iUpperIndex = PLAYER_TRAMPOLINE_HIGH_LOOP;
	//		m_iRootIndex = PLAYER_TRAMPOLINE_HIGH_LOOP;
	//		m_iBottomIndex = PLAYER_TRAMPOLINE_HIGH_LOOP;
	//	}
	//}
	//if (PLAYER_TRAMPOLINE_ENTER == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_ENTER))
	//{
	//	if (PLAYER_TRAMPOLINE_LOW_LOOP != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
	//	{
	//		m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
	//		m_iRootIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
	//		m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
	//	}

	//}
	if (PLAYER_TRAMPOLINE_LOW_LOOP == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_LOW_LOOP))
	{
		if (PLAYER_TRAMPOLINE_LOW_FALL_FRIST != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
		{
			m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_FALL_FRIST;
			m_iRootIndex = PLAYER_TRAMPOLINE_LOW_FALL_FRIST;
			m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_FALL_FRIST;
		}
	}
	else if (PLAYER_TRAMPOLINE_LOW_FALL_FRIST == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_LOW_FALL_FRIST))
	{
		if (PLAYER_TRAMPOLINE_LOW_FALL_NEXT != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
		{
			m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_FALL_NEXT;
			m_iRootIndex = PLAYER_TRAMPOLINE_LOW_FALL_NEXT;
			m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_FALL_NEXT;
		}
	}
	else if (PLAYER_TRAMPOLINE_LOW_FALL_NEXT == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_LOW_FALL_NEXT))
	{
		if (PLAYER_FALLING != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;
		}
	}
}

void CPlayer_KitchenButton::Land_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_LandCheck() && true == m_bLandChecking)
	{
		//m_iUpperIndex = PALYER_LANDING;
		//m_iRootIndex = PALYER_LANDING;
		//m_iBottomIndex = PALYER_LANDING;
		//
		//pPlayer_AnimState->Set_LandCheck(false);
		//m_dLandTimeAcc += TimeDelta;

		//if (0.35 <= m_dLandTimeAcc)
		//{
			m_iUpperIndex = PALYER_LANDING;
			m_iRootIndex = PALYER_LANDING;
			m_iBottomIndex = PALYER_LANDING;
			m_dLandTimeAcc = 0.0;
			//m_bFirstCheck = false;
			m_bLandChecking = false;
			pPlayer_AnimState->Set_LandCheck(false);
		//}
	//}
	}
}

void CPlayer_KitchenButton::Update_Player_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
}

_bool CPlayer_KitchenButton::RepeatJump(CPlayer_AnimState * pPlayer_AnimState)
{
	for (_uint i = 0; i < 3; i++)
	{
		if (true == m_pPlayer->Get_IsKitchenButton(i))
		{
			m_bFirstCheck = false;
			m_pPlayer->Set_IsKitchenButton(false, i);
			return true;
		}
	}

	return false;
}

void CPlayer_KitchenButton::Start_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (true == m_bBeforeJumping)
	{
		for (_uint i = 0; i < 3; i++)
		{
			if (true == m_pPlayer->Get_IsKitchenButton(i))
				SetUp_One_Jump(pPlayer_AnimState, TimeDelta);
		}
	}
}

void CPlayer_KitchenButton::SetUp_One_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	Reset_Anim(pPlayer_AnimState);
	m_fJumpPower = DEFAULT_JUMP_POWER;

	pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_JumpCheck(true);

	m_bLandChecking = false;
	m_bBeforeJumping = false;
	++m_iJumpCount;
}

_bool CPlayer_KitchenButton::KitchenButton_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

_bool CPlayer_KitchenButton::KitchenButton_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex && 3 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
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

void CPlayer_KitchenButton::LandTimeCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_JumpCheck() && false == m_bLandChecking)
	{
		m_dLandTimeAcc += TimeDelta;

		if (0.3 <= m_dLandTimeAcc)
		{
			m_dLandTimeAcc = 0.0;
			m_bLandChecking = true;
		}
	}

}

void CPlayer_KitchenButton::Jump_Initialize(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.6);
	pPlayer_AnimState->Set_FallSpeed(1.0);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
}

void CPlayer_KitchenButton::Free()
{
}
