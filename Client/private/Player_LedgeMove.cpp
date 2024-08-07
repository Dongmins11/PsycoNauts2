#include "stdafx.h"
#include "..\public\Player_LedgeMove.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Dodge.h"
#include "Player_Falling.h"
#include "Player_LedgeJump.h"
#include "Player_Ledge.h"

IMPLEMENT_SINGLETON(CPlayer_LedgeMove)

CPlayer_LedgeMove::CPlayer_LedgeMove()
{
}

void CPlayer_LedgeMove::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;
	pPlayer_AnimState->Set_LedgeCheck(true);

	Set_FirstCheck(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == Set_Key_AnimationChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_LedgeMove::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_LedgeMove::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bRightMoveCheck = false;
	m_bLeftMoveCheck = false;
	m_bFirstCheck = false;

	m_iUpperIndex = PLAYER_LEDGEGRAB_DOWN;
	m_iRootIndex = PLAYER_LEDGEGRAB_DOWN;
	m_iBottomIndex = PLAYER_LEDGEGRAB_DOWN;

	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_LedgeMove::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_LedgeMove::Set_FirstCheck(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_LEDGEGRAB_LEFT_MOVE, 6.f);
		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_LEDGEGRAB_LEFT_MOVE, 6.f);
		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_LEDGEGRAB_LEFT_MOVE, 6.f);

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_LEDGEGRAB_RIGHT_MOVE, 6.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_LEDGEGRAB_RIGHT_MOVE, 6.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_LEDGEGRAB_RIGHT_MOVE, 6.f);

		m_bFirstCheck = true;
	}


}

_bool CPlayer_LedgeMove::Set_Key_AnimationChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{

		if ('A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_LEDGEGRAB_RIGHT_MOVE == m_iRootIndex && 5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_LEDGEGRAB_RIGHT_MOVE))
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();

				m_iUpperIndex = PLAYER_LEDGEGRAB_LEFT_MOVE;
				m_iRootIndex = PLAYER_LEDGEGRAB_LEFT_MOVE;
				m_iBottomIndex = PLAYER_LEDGEGRAB_LEFT_MOVE;
			}
			else if (PLAYER_LEDGEGRAB_RIGHT_MOVE != m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_LEDGEGRAB_LEFT_MOVE;
				m_iRootIndex = PLAYER_LEDGEGRAB_LEFT_MOVE;
				m_iBottomIndex = PLAYER_LEDGEGRAB_LEFT_MOVE;
			}

		}
		else if ('D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_LEDGEGRAB_LEFT_MOVE == m_iRootIndex && 5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_LEDGEGRAB_LEFT_MOVE))
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();

				m_iUpperIndex = PLAYER_LEDGEGRAB_RIGHT_MOVE;
				m_iRootIndex = PLAYER_LEDGEGRAB_RIGHT_MOVE;
				m_iBottomIndex = PLAYER_LEDGEGRAB_RIGHT_MOVE;
			}
			else if (PLAYER_LEDGEGRAB_LEFT_MOVE != m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_LEDGEGRAB_RIGHT_MOVE;
				m_iRootIndex = PLAYER_LEDGEGRAB_RIGHT_MOVE;
				m_iBottomIndex = PLAYER_LEDGEGRAB_RIGHT_MOVE;
			}
		}
	}


	Set_MoveDirection(pPlayer_AnimState,pAnimKeyDesc,TimeDelta);


	if (pAnimKeyDesc.iKeyCount == 0 && false == m_pModel->Get_IsRootLerp())
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_Ledge::Get_Instance());
		CPlayer_Ledge::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}

	return false;
}


void CPlayer_LedgeMove::Set_Move_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

}

void CPlayer_LedgeMove::Set_MoveDirection(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (nullptr == m_pTrigger_Parkour)
		return;

	_vector vPostion = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_vector vStartPos = XMLoadFloat4(&m_pTrigger_Parkour->Get_Trigger_StartPosition());
	_vector vEndPos = XMLoadFloat4(&m_pTrigger_Parkour->Get_Trigger_EndPosition());
	_double TransformSpeed = m_pTransform->Get_TransFormDescSpeed();


	if (PLAYER_LEDGEGRAB_LEFT_MOVE == m_iRootIndex)
		vPostion -= XMVector3Normalize(vRight) * _float(TimeDelta);
	else if (PLAYER_LEDGEGRAB_RIGHT_MOVE == m_iRootIndex)
		vPostion += XMVector3Normalize(vRight) * _float(TimeDelta);



	if (true == m_pTrigger_Parkour->Get_MoveChecking(CPlayerTrigger_Parkour::TRIGGER_LOOK_STATE::LOOK_LEFT))
	{
		if (XMVectorGetX(vPostion) <= XMVectorGetX(vStartPos) && XMVectorGetX(vPostion) >= XMVectorGetX(vEndPos))
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
			m_bLeftMoveCheck = true;
		}
		else
		{
			if (true == m_bLeftMoveCheck)
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				m_bLeftMoveCheck = false;
			}

			m_iUpperIndex = PLAYER_LEDGEGRAB_LOOP;
			m_iRootIndex = PLAYER_LEDGEGRAB_LOOP;
			m_iBottomIndex = PLAYER_LEDGEGRAB_LOOP;

		}
	}
	else if (true == m_pTrigger_Parkour->Get_MoveChecking(CPlayerTrigger_Parkour::TRIGGER_LOOK_STATE::LOOK_RIGHT))
	{
		if (XMVectorGetX(vPostion) <= XMVectorGetX(vEndPos) && XMVectorGetX(vPostion) >= XMVectorGetX(vStartPos))
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
			m_bRightMoveCheck = true;
		}
		else
		{
			if (true == m_bLeftMoveCheck)
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				m_bRightMoveCheck = false;
			}

			m_iUpperIndex = PLAYER_LEDGEGRAB_LOOP;
			m_iRootIndex = PLAYER_LEDGEGRAB_LOOP;
			m_iBottomIndex = PLAYER_LEDGEGRAB_LOOP;

		}

	}

}

void CPlayer_LedgeMove::Free()
{
}

