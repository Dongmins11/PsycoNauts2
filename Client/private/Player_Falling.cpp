#include "stdafx.h"
#include "..\public\Player_Falling.h"
#include "Player_Landing.h"
#include "Player_Floting.h"
#include "Player_RunAnim.h"
#include "Player_TrampolineJump.h"
#include "Camera_Fly.h"


IMPLEMENT_SINGLETON(CPlayer_Falling)

CPlayer_Falling::CPlayer_Falling()
{
}

void CPlayer_Falling::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iUpperIndex = PLAYER_FALLING;
		m_iRootIndex = PLAYER_FALLING;
		m_iBottomIndex = PLAYER_FALLING;
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
			bMoveCheck = true;
		}
	}

	m_bIsMoveOn = bMoveCheck;

	
	if (true == Land_to_Trampoline(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (false == pPlayer_AnimState->Get_GravityCheck())
	{
		Reset_Anim(pPlayer_AnimState);

		if (true == bMoveCheck)
		{
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
		else
		{
			pPlayer_AnimState->SetUp_State(CPlayer_Landing::Get_Instance());
			CPlayer_Landing::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);

}

void CPlayer_Falling::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_Falling::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();

	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
		pPlayer_AnimState->Set_TrampolineTriggerCheck(false);
}

void CPlayer_Falling::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;

	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
		pPlayer_AnimState->Set_TrampolineTriggerCheck(false);
}

_bool CPlayer_Falling::Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_TrampolineJump::Get_Instance());
		CPlayer_TrampolineJump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.8f, 0.1f, 8.f, 9.f), 0.2f, 1.f);
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	return false;
}

void CPlayer_Falling::Free()
{
}

