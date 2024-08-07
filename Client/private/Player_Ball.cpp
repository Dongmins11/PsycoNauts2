#include "stdafx.h"
#include "..\public\Player_Ball.h"
#include "Player_IdleAnim.h"

IMPLEMENT_SINGLETON(CPlayer_Ball)

CPlayer_Ball::CPlayer_Ball()
{
}

void CPlayer_Ball::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		Set_BlendSpeed();

		m_iUpperIndex = PLAYER_BALL_START;
		m_iRootIndex = PLAYER_BALL_START;
		m_iBottomIndex = PLAYER_BALL_START;

		m_bFirstCheck = true;
	}
	
	if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_BALL_START) &&false == m_bStartCheck)
		m_bStartCheck = true;

	if (PLAYER_BALL_END != m_iRootIndex)
	{
		if (FAILED(Ball_Interaction_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta)))
			return;

		if (FAILED(Ball_Idle_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta)))
			return;

		if (FAILED(Ball_Jump_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta)))
			return;
	}
	if (true == m_bStartCheck)
	{
		if (m_iRootPreIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreIndex = m_iRootIndex;
		}
	}

	if (true == m_bEndCheck && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_BALL_END))
	{
		Reset_Anim(pPlayer_AnimState);
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Ball::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Ball::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_bStartCheck = false;
	m_bEndCheck = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Ball::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_Ball::Set_BlendSpeed()
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_BALL_IDEL_TWO, 4.3);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_BALL_IDEL_TWO, 4.3);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_BALL_IDEL_TWO, 4.3);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_BALLJUMP_UP, 4.3);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_BALLJUMP_UP, 4.3);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_BALLJUMP_UP, 4.3);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_BALLJUMP_HOLD, 5.0);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_BALLJUMP_HOLD, 5.0);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_BALLJUMP_HOLD, 5.0);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_BALLJUMP_LAND, 7.0);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_BALLJUMP_LAND, 7.0);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_BALLJUMP_LAND, 7.0);

}

HRESULT CPlayer_Ball::Ball_Idle_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bStartCheck)
	{
		if (PLAYER_BALLJUMP_LAND != m_iRootIndex)
		{
			if (0 == pAnimKeyDesc.iKeyCount && false == m_bJumpCheck)
			{
				if (false == m_bIdleChangeCheck)
				{
					m_iUpperIndex = PLAYER_BALL_IDEL;
					m_iRootIndex = PLAYER_BALL_IDEL;
					m_iBottomIndex = PLAYER_BALL_IDEL;

					m_lfBallIdleChangeTimeAcc += TimeDelta;
					if (7.f <= m_lfBallIdleChangeTimeAcc)
					{
						m_lfBallIdleChangeTimeAcc = 0.0;
						m_bIdleChangeCheck = true;
					}
				}
				else
				{
					m_iUpperIndex = PLAYER_BALL_IDEL_TWO;
					m_iRootIndex = PLAYER_BALL_IDEL_TWO;
					m_iBottomIndex = PLAYER_BALL_IDEL_TWO;

					if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_BALL_IDEL_TWO))
						m_bIdleChangeCheck = false;

				}
			}
		}
		else
		{
			if (m_pModel->Get_IsRootAnimFinishied(PLAYER_BALLJUMP_LAND))
			{
				m_iUpperIndex = PLAYER_BALL_IDEL;
				m_iRootIndex = PLAYER_BALL_IDEL;
				m_iBottomIndex = PLAYER_BALL_IDEL;
			}
		}
	}
	return S_OK;
}

HRESULT CPlayer_Ball::Ball_Interaction_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bStartCheck && false == m_bJumpCheck && PLAYER_BALLJUMP_LAND != m_iRootIndex)
	{
		for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
		{
			if (pAnimKeyDesc.iKey[i] == 'W' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
				pAnimKeyDesc.iKey[i] == 'S' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
				pAnimKeyDesc.iKey[i] == 'A' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
				pAnimKeyDesc.iKey[i] == 'D' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				m_lfBallIdleChangeTimeAcc = 0.0;
				m_bIdleChangeCheck = false;
				m_iUpperIndex = PLAYER_BALL_RUN;
				m_iRootIndex = PLAYER_BALL_RUN;
				m_iBottomIndex = PLAYER_BALL_RUN;
			}
			else if (pAnimKeyDesc.iKey[i] == 'R' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN && false == m_bEndCheck)
			{
				if (true == m_bJumpCheck)
				{
					Reset_Anim(pPlayer_AnimState);
				}
				else
				{
					m_iUpperIndex = PLAYER_BALL_END;
					m_iRootIndex = PLAYER_BALL_END;
					m_iBottomIndex = PLAYER_BALL_END;
					m_bEndCheck = true;
				}
			}
			else if (pAnimKeyDesc.iKey[i] == VK_SPACE && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN && false == m_bJumpCheck)
			{
				m_bJumpCheck = true;
				m_fJumpPower = 10.f;

				m_iUpperIndex = PLAYER_BALLJUMP_UP;
				m_iRootIndex = PLAYER_BALLJUMP_UP;
				m_iBottomIndex = PLAYER_BALLJUMP_UP;

				if (true == m_bLandCheck)
					m_bLandCheck = false;

			}
		}
	}

	return S_OK;
}

HRESULT CPlayer_Ball::Ball_Jump_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if(true == m_bJumpCheck)
	{
		m_lfJumpTimeAcc += TimeDelta * 0.3;
		m_pTransform->Jump(TimeDelta, &m_bJumpCheck, &m_fBackUpY, &m_lfJumpTimeAcc, &m_fJumpPower, &m_fJumpValue, &m_bFallCheck,&m_bLandCheck, m_pCharacterController);
	}
	
	if (PLAYER_BALLJUMP_UP == m_iRootIndex && 6 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_BALLJUMP_UP))
	{
		m_iUpperIndex = PLAYER_BALLJUMP_HOLD;
		m_iRootIndex = PLAYER_BALLJUMP_HOLD;
		m_iBottomIndex = PLAYER_BALLJUMP_HOLD;
	}

	if (true == m_bFallCheck && false == m_bLandCheck)
	{
		m_iUpperIndex = PLAYER_BALLJUMP_DOWN;
		m_iRootIndex = PLAYER_BALLJUMP_DOWN;
		m_iBottomIndex = PLAYER_BALLJUMP_DOWN;
	}
	else if (true == m_bLandCheck)
	{
		m_iUpperIndex = PLAYER_BALLJUMP_LAND;
		m_iRootIndex = PLAYER_BALLJUMP_LAND;
		m_iBottomIndex = PLAYER_BALLJUMP_LAND;

		m_bLandCheck = false;
	}

	return S_OK;
}

void CPlayer_Ball::Free()
{
}

