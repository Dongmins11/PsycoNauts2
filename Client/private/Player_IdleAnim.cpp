#include "stdafx.h"
#include "..\public\Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_ComboAttack.h"
#include "Player_Pisblast.h"
#include "Player_Ball.h"
#include "Player_Jump.h"
#include "Player_Walk.h"
#include "Player_Falling.h"
#include "Player_GrabAttack.h"
#include "Player_GrabBack.h"
#include "Player_BrainStom.h"
#include "Player_Hit.h"
#include "Player_BrainWashing.h"
#include "Player_PullSwitch.h"
#include "Player_Bouncer.h"
#include "Camera_Fly.h"
#include "Player_TrampolineJump.h"
#include "Player_CloudAnim.h"
#include "Player_Food_Throw.h"

IMPLEMENT_SINGLETON(CPlayer_IdleAnim)

CPlayer_IdleAnim::CPlayer_IdleAnim()
{
}

void CPlayer_IdleAnim::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	if (true == Land_to_Trampoline(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Set_GravityChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Hit_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Idle_To_Bouncer(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Idle_To_Cloud(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;


	if (false == m_pPlayer->Get_KeyCheck())
	{
		if (true == Idle_to_StateChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
			return;
	}



	Grab_RayCastingLoop(pPlayer_AnimState, TimeDelta);

	if (false == m_bRandomChangeCheck)
	{
		if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
		{
			if (true == m_pModel->Get_IsUpperAnimFinishied(PLAYER_GRAP_ATTACK_START))
			{
				pPlayer_AnimState->Set_UpperLerpCheck(true);
				m_iUpperIndex = PLAYER_IDLE;
				m_pModel->Set_UpperAnimation(m_iUpperIndex);
				return;
			}
			else
				m_iUpperIndex = PLAYER_GRAP_ATTACK_START;
		}
		else
			m_iUpperIndex = PLAYER_IDLE;

		m_iRootIndex = PLAYER_IDLE;
		m_iBottomIndex = PLAYER_IDLE;
	}



	Set_RandomIdleMotion(pPlayer_AnimState, TimeDelta);



	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_IdleAnim::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bIsMoveOn = false;
	Move_Acceleration(TimeDelta);

	if (true == m_bSettingCheck)
	{
		m_pModel->Upper_Update(TimeDelta);
		m_pModel->Root_Update(TimeDelta);
		m_pModel->Bottom_Update(TimeDelta);

		m_bSettingCheck = false;
	}
	else
	{
		m_pModel->Root_Update(TimeDelta);
		m_pModel->Upper_Update(TimeDelta);
		m_pModel->Bottom_Update(TimeDelta);
	}
}

void CPlayer_IdleAnim::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_IdleAnim::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_lfChangeTime = 0.0;
	m_bRandomChangeCheck = false;
}

void CPlayer_IdleAnim::Reset_Check()
{
	m_lfChangeTime = 0.0;
	m_bRandomChangeCheck = false;
}

void CPlayer_IdleAnim::Set_BlendSpeed(_double TimeDelta)
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_IDLE, 6.2f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_IDLE, 6.2f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_IDLE, 6.2f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RANDOM_ONE_IDLE, 6.2f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RANDOM_ONE_IDLE, 6.2f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RANDOM_ONE_IDLE, 6.2f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RANDOM_TWO_IDLE, 6.2f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RANDOM_TWO_IDLE, 6.2f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RANDOM_TWO_IDLE, 6.2f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RANDOM_THREE_IDLE, 6.2f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RANDOM_THREE_IDLE, 6.2f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RANDOM_THREE_IDLE, 6.2f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RANDOM_FOR_IDLE, 6.2f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RANDOM_FOR_IDLE, 6.2f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RANDOM_FOR_IDLE, 6.2f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RANDOM_FIVE_IDLE, 6.2f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RANDOM_FIVE_IDLE, 6.2f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RANDOM_FIVE_IDLE, 6.2f);
}

void CPlayer_IdleAnim::Set_RandomIdleMotion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == m_bRandomChangeCheck)
	{
		m_lfChangeTime += TimeDelta;

		if (7.f <= m_lfChangeTime)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_lfChangeTime = 0.0;
			m_iRandomValue = rand() % 6;
			m_bRandomChangeCheck = true;
		}
	}


	if (true == m_bRandomChangeCheck)
	{
		switch (m_iRandomValue)
		{
		case 0:
			m_iUpperIndex = PLAYER_RANDOM_ONE_IDLE;
			m_iRootIndex = PLAYER_RANDOM_ONE_IDLE;
			m_iBottomIndex = PLAYER_RANDOM_ONE_IDLE;
			break;
		case 1:
			m_iUpperIndex = PLAYER_RANDOM_TWO_IDLE;
			m_iRootIndex = PLAYER_RANDOM_TWO_IDLE;
			m_iBottomIndex = PLAYER_RANDOM_TWO_IDLE;
			break;
		case 2:
			m_iUpperIndex = PLAYER_RANDOM_THREE_IDLE;
			m_iRootIndex = PLAYER_RANDOM_THREE_IDLE;
			m_iBottomIndex = PLAYER_RANDOM_THREE_IDLE;
			break;
		case 4:
			m_iUpperIndex = PLAYER_RANDOM_FOR_IDLE;
			m_iRootIndex = PLAYER_RANDOM_FOR_IDLE;
			m_iBottomIndex = PLAYER_RANDOM_FOR_IDLE;
			break;
		case 5:
			m_iUpperIndex = PLAYER_RANDOM_FIVE_IDLE;
			m_iRootIndex = PLAYER_RANDOM_FIVE_IDLE;
			m_iBottomIndex = PLAYER_RANDOM_FIVE_IDLE;
			break;
		default:
			m_iUpperIndex = PLAYER_IDLE;
			m_iRootIndex = PLAYER_IDLE;
			m_iBottomIndex = PLAYER_IDLE;
			break;
		}

		if (true == m_pModel->Get_IsRootAnimFinishied(m_iRootIndex))
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_bRandomChangeCheck = false;
		}
	}
}

void CPlayer_IdleAnim::Grab_RayCastingLoop(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	pPlayer_AnimState->Grab_RayCast_Loop(TimeDelta);
}

_bool CPlayer_IdleAnim::Idle_to_StateChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			Reset_Check();

			if (true == pPlayer_AnimState->Get_WalkingCheck())
			{
				pPlayer_AnimState->Set_OneLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Walk::Get_Instance());
				CPlayer_Walk::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
			else
			{
				if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
					pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
				else
					pPlayer_AnimState->Set_AllLerpCheckTrue();

				pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
				CPlayer_RunAnim::Get_Instance()->Set_MoveOffset_ChangeCheck(false);
				CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}

		if (false == pPlayer_AnimState->Get_WalkingCheck())
		{
			if (VK_RBUTTON == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				if (PLAYER_GRAP_ATTACK_START != m_pModel->Get_UpperAnimIndex())
				{
					if (false == pPlayer_AnimState->Get_GrabCheck())
					{
						pPlayer_AnimState->Grab_RayCast_Ready(TimeDelta);

						if (true == pPlayer_AnimState->Get_GrabBackCheck())
						{
							Reset_Check();
							Reset_Anim(pPlayer_AnimState);
							m_pRightHand->Set_ChangeAnimCheck(true);
							pPlayer_AnimState->Set_GrabBack(false);
							m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB_PULLING);
							pPlayer_AnimState->SetUp_State(CPlayer_GrabBack::Get_Instance());
							CPlayer_GrabBack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
							return true;
						}
						else
						{
							pPlayer_AnimState->Set_UpperLerpCheck(true);
							m_iUpperIndex = PLAYER_GRAP_ATTACK_START;
							m_pModel->Set_UpperAnimation(PLAYER_GRAP_ATTACK_START);
						}
					}
					else
					{
						if (true == pPlayer_AnimState->Get_GrabAttackCheck())
						{
							pPlayer_AnimState->Grab_RayCasting();
							Reset_Check();

							m_pRightHand->Set_ChangeAnimCheck(true);
							m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB_ATTACK);
							Reset_Anim(pPlayer_AnimState);
							pPlayer_AnimState->SetUp_State(CPlayer_GrabAttack::Get_Instance());
							CPlayer_GrabAttack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
							return true;
						}
					}
				}

			}
			if (PLAYER_GRAP_ATTACK_START != m_pModel->Get_UpperAnimIndex())
			{
				if (false == m_pPlayer->Get_RenderCheck())
				{
					if ('E' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
					{
						Reset_Check();
						pPlayer_AnimState->SetUp_State(CPlayer_Pisblast::Get_Instance());
						CPlayer_Pisblast::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;

					}
					else if ('Q' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
					{
						Reset_Anim(pPlayer_AnimState);
						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_BrainStom::Get_Instance());
						CPlayer_BrainStom::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
					//else if (pAnimKeyDesc.iKey[i] == 'R' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
					//{
					//	Reset_Check();
					//	pPlayer_AnimState->Set_AllLerpCheckTrue();
					//	pPlayer_AnimState->SetUp_State(CPlayer_Ball::Get_Instance());
					//	CPlayer_Ball::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
					//	return true;
					//}
					else if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
					{
						Reset_Check();
						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
						CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
						((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_IsFixY(true);
						RELEASE_INSTANCE(CGameInstance);
						return true;
					}
					else if ('T' == pAnimKeyDesc.iKey[i] && CPlayer_AnimState::KEY_PRESING == pAnimKeyDesc.eKeyState[i])
					{
						Reset_Check();
						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_BrainWashing::Get_Instance());
						CPlayer_BrainWashing::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
					else if ('Y' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
					{
						if (true == m_pPlayer->Get_PullSwitch() && false == m_pPlayer->Get_PullSwitchingEnd())
						{
							Reset_Check();
							pPlayer_AnimState->Set_AllLerpCheckTrue();
							pPlayer_AnimState->SetUp_State(CPlayer_PullSwitch::Get_Instance());
							CPlayer_PullSwitch::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
							return true;
						}
						else if (true == m_pPlayer->Get_DishCheck())
						{
							Reset_Check();
							pPlayer_AnimState->Set_AllLerpCheckTrue();
							pPlayer_AnimState->SetUp_State(CPlayer_Food_Throw::Get_Instance());
							CPlayer_Food_Throw::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
							return true;
						}
					}
				}
			}
		}
	}


	if (PLAYER_GRAP_ATTACK_START != m_pModel->Get_UpperAnimIndex() && false == pPlayer_AnimState->Get_WalkingCheck())
	{
		/*For.Gara*/
		if (GetKeyState(VK_F9) & 0x8000)
			m_bGara = false;
		if (GetKeyState(VK_F10) & 0x8000)
			m_bGara = true;
		if (true == m_bGara)
		{
			if (VK_LBUTTON == pAnimKeyDesc.iOnceKey)
			{
				Reset_Check();
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_ComboAttack::Get_Instance());
				CPlayer_ComboAttack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
	}

	return false;
}

_bool CPlayer_IdleAnim::Set_GravityChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_GravityCheck())
	{
		Reset_Anim(pPlayer_AnimState);
		Reset_Check();
		pPlayer_AnimState->SetUp_State(CPlayer_Falling::Get_Instance());
		CPlayer_Falling::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}

	return false;
}

_bool CPlayer_IdleAnim::Hit_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_HitCheck())
	{
		Reset_Anim(pPlayer_AnimState);
		Reset_Check();
		pPlayer_AnimState->SetUp_State(CPlayer_Hit::Get_Instance());
		CPlayer_Hit::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

_bool CPlayer_IdleAnim::Idle_To_Bouncer(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < 3; i++)
	{
		if (true == m_pPlayer->Get_IsBouncer(i))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_Bouncer::Get_Instance());
			CPlayer_Bouncer::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

_bool CPlayer_IdleAnim::Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_TrampolineTriggerCheck(false);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_TrampolineJump::Get_Instance());
		CPlayer_TrampolineJump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.8f, 0.1f, 8.f, 9.f), 0.2f, 1.f);
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	return false;
}

_bool CPlayer_IdleAnim::Idle_To_Cloud(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if ('V' == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_DOWN)
	{
		if (true == pPlayer_AnimState->Cloud_RayCast_Ready(TimeDelta))
		{
			if (nullptr != pPlayer_AnimState)
			{
				m_iUpperIndex = 0;
				m_iRootIndex = 0;
				m_iBottomIndex = 0;

				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_CloudAnim::Get_Instance());
				CPlayer_CloudAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
		return false;
	}
	return false;
}


void CPlayer_IdleAnim::Free()
{
}

