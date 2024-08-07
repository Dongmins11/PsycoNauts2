#include "stdafx.h"
#include "..\public\Player_Blender_Climbing_JumpEnd.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Player_Blender_Climbing_JumpEnd.h"

IMPLEMENT_SINGLETON(CPlayer_Blender_Climbing_JumpEnd)

CPlayer_Blender_Climbing_JumpEnd::CPlayer_Blender_Climbing_JumpEnd()
{
}
void CPlayer_Blender_Climbing_JumpEnd::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_fJumpPower = 15.f;

		m_iUpperIndex = PLAYER_CLIMB_JUMP;
		m_iRootIndex = PLAYER_CLIMB_JUMP;
		m_iBottomIndex = PLAYER_CLIMB_JUMP;

		m_iRootPreIndex = PLAYER_CLIMB_JUMP;

		m_pModel->Set_UpperAnimBlendSpeed(PALYER_LANDING, 7.f);
		m_pModel->Set_RootAnimBlendSpeed(PALYER_LANDING, 7.f);
		m_pModel->Set_BottomAnimBlendSpeed(PALYER_LANDING, 7.f);

		m_bFirstCheck = true;
	}

	//true 중력없음
	//false 중력있음
	m_bGravityCheck = true;

	_bool bUpKeyCheck = false;
	_bool bMoveCheck = false;
	_bool bJump = false;
	_bool Test = false;
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bUpKeyCheck = true;
			bMoveCheck = true;
		}
		if ('S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bUpKeyCheck = true;
			bMoveCheck = true;
		}
		if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			bJump = true;
			if (false == pPlayer_AnimState->Get_LedgeJumpCheck() && PALYER_LANDING != m_iRootIndex)
			{
				pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
				pPlayer_AnimState->Set_LandCheck(false);
				pPlayer_AnimState->Set_LedgeJumpCheck(true);

				_float4 vPos;
				XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
				pPlayer_AnimState->Set_BackUpY(vPos.y);
			}

		}
		//if ('I' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		//	Test = true;
	}

	if (Test)
	{
		Reset_Anim(pPlayer_AnimState);
		m_pCharacterController->Set_Position(_float3(279.f, 6.f, 157.f));
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(287.5f, 19.4f, 195.3f)), 1.f));
		m_pTransform->Set_Velocity(XMVectorZero());
		m_pPlayer->Set_Climbing_TriggerCheck(false, -1);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}



	m_bMoveCheck = bMoveCheck;


	if (PLAYER_CLIMB_JUMP == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_CLIMB_JUMP))
	{
		if (m_iRootIndex != PALYER_LANDING)
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;
		}
	}

	//랜드조건 함수빼도돰
	if (true == pPlayer_AnimState->Get_LandCheck())
	{
		m_iUpperIndex = PALYER_LANDING;
		m_iRootIndex = PALYER_LANDING;
		m_iBottomIndex = PALYER_LANDING;

		pPlayer_AnimState->Set_LandCheck(false);
	}

	//아이들조건 함수빼도됨
	if (m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}


	//런조건 함수빼도됨
	if (2 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
	{
		if (true == m_bMoveCheck)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}

	//러프시키위한녀석
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

void CPlayer_Blender_Climbing_JumpEnd::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == pPlayer_AnimState->Get_LedgeCheck() && PALYER_LANDING != m_iRootIndex)
		m_pTransform->Go_Straight(TimeDelta * 0.4, m_pCharacterController);

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Blender_Climbing_JumpEnd::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iRootIndex = 0;

	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
	pPlayer_AnimState->Set_LedgeJumpCheck(false);
	pPlayer_AnimState->Set_LedgeJumpCheck(false);


	m_bFirstCheck = false;
}

void CPlayer_Blender_Climbing_JumpEnd::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);
}

void CPlayer_Blender_Climbing_JumpEnd::Free()
{
}
