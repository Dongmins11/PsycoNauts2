#include "stdafx.h"
#include "..\public\Player_TrampolineJump.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Floting.h"
#include "Player_Slider.h"
#include "Player_Ledge.h"
#include "Player_DownAttack.h"
#include "Trampoline.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
IMPLEMENT_SINGLETON(CPlayer_TrampolineJump)

CPlayer_TrampolineJump::CPlayer_TrampolineJump()
{
}

void CPlayer_TrampolineJump::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool	bMoveCheck = false;

	m_bGravityCheck = true;

	First_JumpStateCheck();

	pPlayer_AnimState->PhysX_Slider();

	Start_Jump(pPlayer_AnimState, TimeDelta);

	Adjust_StartPos(pPlayer_AnimState, TimeDelta);
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bMoveCheck = true;
		}
		if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			if (0 == m_iJumpCount && false == pPlayer_AnimState->Get_JumpCheck())
			{
				SetUp_One_Jump(pPlayer_AnimState, TimeDelta);
				if (false == m_bBeforeJumping)
					SetUp_Double_Jump(pPlayer_AnimState, TimeDelta);
			}
			else if (1 == m_iJumpCount && false == pPlayer_AnimState->Get_DoubleJumpCheck())
			{
				SetUp_Double_Jump(pPlayer_AnimState, TimeDelta);

			}
		}
	}
	m_bIsMoveOn = bMoveCheck;

	if (true == RepeatJump(pPlayer_AnimState))
		return;

	Fall(pPlayer_AnimState, TimeDelta);

	Land(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == Jump_to_Ledge(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Jump_to_Slider(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
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

void CPlayer_TrampolineJump::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		Handle_Movement(pAnimKeyDesc);
		Player_Move_NonMoveOffset(TimeDelta);
	}
	if (PALYER_LANDING == m_iRootIndex)
	{
		m_pModel->Upper_Update(TimeDelta * 1.5);
		m_pModel->Root_Update(TimeDelta * 1.5);
		m_pModel->Bottom_Update(TimeDelta * 1.5);

	}
	else
	{
		m_pModel->Upper_Update(TimeDelta * 0.8);
		m_pModel->Root_Update(TimeDelta * 0.8);
		m_pModel->Bottom_Update(TimeDelta * 0.8);
	}

}

void CPlayer_TrampolineJump::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(1.f);
	pPlayer_AnimState->Set_FallSpeed(1.5f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
	pPlayer_AnimState->Set_TrampolineJumpCheck(false);
	pPlayer_AnimState->Set_CollisionObject(nullptr);
}

void CPlayer_TrampolineJump::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);

	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_iJumpCount = 0;
	m_bFirstCheck = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value_Default(1.f);
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.8f);
	RELEASE_INSTANCE(CGameInstance);
}


void CPlayer_TrampolineJump::Hold(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{

	//if (m_eJumpCheck == JUMP_LEFT)
	//{
	//	if (4 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_JUMP_LEFT_UP))
	//	{
	//		m_iUpperIndex = PLAYER_JUMP_LEFT_HOLD;
	//		m_iRootIndex = PLAYER_JUMP_LEFT_HOLD;
	//		m_iBottomIndex = PLAYER_JUMP_LEFT_HOLD;
	//	}
	//}
	//else if (m_eJumpCheck == JUMP_RIGHT)
	//{
	//	if (4 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_JUMP_RIGHT_UP))
	//	{
	//		m_iUpperIndex = PLAYER_JUMP_RIGHT_HOLD;
	//		m_iRootIndex = PLAYER_JUMP_RIGHT_HOLD;
	//		m_iBottomIndex = PLAYER_JUMP_RIGHT_HOLD;
	//	}
	//}
	//else if (m_eJumpCheck == JUMP_NOMAL)
	//{
	//	if (4 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_JUMP_UP))
	//	{
	//		m_iUpperIndex = PLAYER_JUMP_HOLD;
	//		m_iRootIndex = PLAYER_JUMP_HOLD;
	//		m_iBottomIndex = PLAYER_JUMP_HOLD;
	//	}
	//}
}

void CPlayer_TrampolineJump::Double_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (m_eJumpCheck == JUMP_LEFT)
	{
		m_iUpperIndex = PLAYER_DOUBLE_JUMP_LEFT;
		m_iRootIndex = PLAYER_DOUBLE_JUMP_LEFT;
		m_iBottomIndex = PLAYER_DOUBLE_JUMP_LEFT;
	}
	else if (m_eJumpCheck == JUMP_RIGHT)
	{
		m_iUpperIndex = PLAYER_DOUBLE_JUMP_RIGHT;
		m_iRootIndex = PLAYER_DOUBLE_JUMP_RIGHT;
		m_iBottomIndex = PLAYER_DOUBLE_JUMP_RIGHT;
	}
	else if (m_eJumpCheck == JUMP_NOMAL)
	{
		m_iUpperIndex = PLAYER_DOUBLE_JUMP;
		m_iRootIndex = PLAYER_DOUBLE_JUMP;
		m_iBottomIndex = PLAYER_DOUBLE_JUMP;
	}
}

void CPlayer_TrampolineJump::Fall(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{

	if (PLAYER_TRAMPOLINE_HIGH_LOOP == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_HIGH_LOOP))
	{
		m_iUpperIndex = PLAYER_TRAMPOLINE_HIGH_JUMP;
		m_iRootIndex = PLAYER_TRAMPOLINE_HIGH_JUMP;
		m_iBottomIndex = PLAYER_TRAMPOLINE_HIGH_JUMP;
	}
	else if (PLAYER_TRAMPOLINE_LOW_LOOP == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_LOW_LOOP))
	{
		m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_FALL_FRIST;
		m_iRootIndex = PLAYER_TRAMPOLINE_LOW_FALL_FRIST;
		m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_FALL_FRIST;
	}

	if (PLAYER_TRAMPOLINE_HIGH_JUMP == m_iUpperIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_HIGH_JUMP))
	{
		m_iUpperIndex = PLAYER_TRAMPOLINE_HIGH_FALL_FRIST;
		m_iRootIndex = PLAYER_TRAMPOLINE_HIGH_FALL_FRIST;
		m_iBottomIndex = PLAYER_TRAMPOLINE_HIGH_FALL_FRIST;

	}
	else if (PLAYER_TRAMPOLINE_LOW_FALL_FRIST == m_iUpperIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_LOW_FALL_FRIST))
	{
		m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_FALL_NEXT;
		m_iRootIndex = PLAYER_TRAMPOLINE_LOW_FALL_NEXT;
		m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_FALL_NEXT;
	}
	else if (PLAYER_TRAMPOLINE_HIGH_FALL_FRIST == m_iUpperIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_HIGH_FALL_FRIST))
	{
		m_iUpperIndex = PLAYER_TRAMPOLINE_HIGH_FALL_NEXT;
		m_iRootIndex = PLAYER_TRAMPOLINE_HIGH_FALL_NEXT;
		m_iBottomIndex = PLAYER_TRAMPOLINE_HIGH_FALL_NEXT;

	}
	else if (PLAYER_TRAMPOLINE_HIGH_FALL_NEXT == m_iUpperIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_HIGH_FALL_NEXT))
	{
		m_iUpperIndex = PLAYER_FALLING;
		m_iRootIndex = PLAYER_FALLING;
		m_iBottomIndex = PLAYER_FALLING;

	}
	else if (PLAYER_TRAMPOLINE_LOW_FALL_NEXT == m_iUpperIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TRAMPOLINE_LOW_FALL_NEXT))
	{
		m_iUpperIndex = PLAYER_FALLING;
		m_iRootIndex = PLAYER_FALLING;
		m_iBottomIndex = PLAYER_FALLING;

	}
	else if (PLAYER_DOUBLE_JUMP == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOUBLE_JUMP))
	{
		m_iUpperIndex = PLAYER_FALLING;
		m_iRootIndex = PLAYER_FALLING;
		m_iBottomIndex = PLAYER_FALLING;
	}

}

void CPlayer_TrampolineJump::Land(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bBeforeJumping && false == m_bAdjustStartPos)
	{
		if (true == pPlayer_AnimState->Get_LandCheck())
		{
			m_iUpperIndex = PALYER_LANDING;
			m_iRootIndex = PALYER_LANDING;
			m_iBottomIndex = PALYER_LANDING;

			ChangeAnim(CHANGE_ANIM_JUMP, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			//이펙트추가 
			// 이펙트 생성지점 
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_LandingSmoke"),
				SCENE_STATIC, TEXT("Effect_Smoke"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_LandingSmoke"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

_bool CPlayer_TrampolineJump::RepeatJump(CPlayer_AnimState * pPlayer_AnimState)
{
	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
	{
		m_bFirstCheck = false;
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_TrampolineTriggerCheck(false);
		return true;
	}
	return false;
}

_bool CPlayer_TrampolineJump::Jump_to_Ledge(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_pPlayer->Get_IsParkour() && true == m_bIsMoveOn)
	{
		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_LEFT);

			ChangeAnim(CHANGE_ANIM_LEDGE, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_FRONT);

			ChangeAnim(CHANGE_ANIM_LEDGE, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);


			return true;
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_RIGHT);

			ChangeAnim(CHANGE_ANIM_LEDGE, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);


			return true;
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_BACK);

			ChangeAnim(CHANGE_ANIM_LEDGE, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

_bool CPlayer_TrampolineJump::Jump_to_Slider(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_Slider() && true == m_bIsMoveOn)
	{
		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_LEFT);

			ChangeAnim(CHANGE_ANIM_SLIDER, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_FRONT);
			ChangeAnim(CHANGE_ANIM_SLIDER, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);


			return true;
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_RIGHT);
			ChangeAnim(CHANGE_ANIM_SLIDER, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_BACK);
			ChangeAnim(CHANGE_ANIM_SLIDER, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
	}

	return false;
}

_bool CPlayer_TrampolineJump::Jump_to_DownAttack(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING != m_iRootIndex)
	{
		ChangeAnim(CHANGE_ANIM_DOWNATTACK, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

_bool CPlayer_TrampolineJump::Fall_While_Jumping(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_FallCheck() && false == m_pModel->Get_IsRootLerp())
	{
		Reset_Anim(pPlayer_AnimState);

		m_bFirstCheck = false;
		m_iJumpCount = 0;
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		pPlayer_AnimState->SetUp_State(CPlayer_Floting::Get_Instance());
		return true;
	}


	return false;
}

_bool CPlayer_TrampolineJump::Land_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
		{
			ChangeAnim(CHANGE_ANIM_IDLE, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_TrampolineJump::Land_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bFallCheck && PALYER_LANDING == m_iRootIndex && 3 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
	{
		if (true == m_bIsMoveOn)
		{
			ChangeAnim(CHANGE_ANIM_RUN, pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

void CPlayer_TrampolineJump::Slider_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	Reset_Anim(pPlayer_AnimState);

	m_bFirstCheck = false;
	m_iJumpCount = 0;
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->Set_AllLerpCheckTrue();

	pPlayer_AnimState->SetUp_State(CPlayer_Slider::Get_Instance());
	CPlayer_Slider::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

}

void CPlayer_TrampolineJump::Ledge_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	Reset_Anim(pPlayer_AnimState);

	m_bFirstCheck = false;
	m_iJumpCount = 0;
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->Set_AllLerpCheckTrue();

	pPlayer_AnimState->SetUp_State(CPlayer_Ledge::Get_Instance());
	CPlayer_Ledge::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
}

void CPlayer_TrampolineJump::Set_BlendSpeed()
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_RIGHT_UP, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_RIGHT_UP, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_RIGHT_UP, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_LEFT_UP, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_LEFT_UP, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_LEFT_UP, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_UP, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_UP, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_UP, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_RIGHT_HOLD, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_RIGHT_HOLD, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_RIGHT_HOLD, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_LEFT_HOLD, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_LEFT_HOLD, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_LEFT_HOLD, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_HOLD, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_HOLD, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_HOLD, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_DOUBLE_JUMP_LEFT, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_DOUBLE_JUMP_LEFT, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_DOUBLE_JUMP_LEFT, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_DOUBLE_JUMP_RIGHT, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_DOUBLE_JUMP_RIGHT, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_DOUBLE_JUMP_RIGHT, 5.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_DOUBLE_JUMP, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_DOUBLE_JUMP, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_DOUBLE_JUMP, 5.f);
}

void CPlayer_TrampolineJump::First_JumpStateCheck()
{
	if (false == m_bFirstCheck)
	{
		m_iJumpCount = 0;
		m_bBeforeJumping = true;
		m_bAdjustStartPos = true;
		m_bFallCheck = false;
		m_fJumpPower = 6.85f;
		m_iRandomState = rand() % 3;
		Set_BlendSpeed();

		m_iUpperIndex = PLAYER_TRAMPOLINE_ENTER;
		m_iRootIndex = PLAYER_TRAMPOLINE_ENTER;
		m_iBottomIndex = PLAYER_TRAMPOLINE_ENTER;

		m_iRootPreIndex = PLAYER_TRAMPOLINE_ENTER;

		m_bFirstCheck = true;
	}
}

void CPlayer_TrampolineJump::Random_JumpStateCheck()
{
	if (m_iBackUpJumpState == m_iRandomState)
	{
		if (0 == m_iRandomState)
			m_iRandomState = 2;
		else if (1 == m_iRandomState)
			m_iRandomState = 0;
		else if (2 == m_iRandomState)
			m_iRandomState = 1;
	}
}

void CPlayer_TrampolineJump::Start_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bBeforeJumping)
	{

		CTrampoline* pTrampoline = dynamic_cast<CTrampoline*>(pPlayer_AnimState->Get_CollisionObject());
		if (nullptr == pTrampoline)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (true == pTrampoline->Get_StartJump())
		{
			m_iUpperIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
			m_iRootIndex = PLAYER_TRAMPOLINE_LOW_LOOP;
			m_iBottomIndex = PLAYER_TRAMPOLINE_LOW_LOOP;

			if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_TRAMPOLINE)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("TrampolineOne.wav"), CHANNELID::PLAYER_TRAMPOLINE)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			

			SetUp_One_Jump(pPlayer_AnimState, TimeDelta);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_TrampolineJump::Adjust_StartPos(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTrampoline* pTrampoline = dynamic_cast<CTrampoline*>(pPlayer_AnimState->Get_CollisionObject());
	if (nullptr == pTrampoline)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	if (true == m_bAdjustStartPos)

	{

		m_dStartLandTimeAcc += TimeDelta;

		if (false == pTrampoline->Get_IsAdjustStartPos())
		{
			m_bAdjustStartPos = false;
			m_dStartLandTimeAcc = 0.0;
		}


		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		// raycasting 의 시작지점 ( 지금 player의 발쪽에 아마 되어있을 가능성이 높음 아마 몸통부분으로 수정해줘야할듯 )
		//_float3 vPos = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);

		//_vector vTramPos = pTrampoline->Get_MiddlePos();
		_vector vTramPos = pTrampoline->Get_Pos();
		_float3 vTramPosFloat3 = STOREFLOAT3(vTramPos);

		tDescIn.vOrigin = vTramPosFloat3;// +XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))*0.35f);// ;
										 // raycasting 방향 ( 플레이어의 look방향이 기본(벽탈때 보통 플레이어 기준 앞쪽을 바라보기 때문)
		XMStoreFloat3(&tDescIn.vUnitDir, XMVectorSet(0.f, -1.f, 0.f, 0.f));
		// raycast 길이
		tDescIn.fDistance = 10.f;
		// 어떤 오브젝트와 부딪힐 것인지에 대한 정보 (전부, 그리고 static만 부딪힌다(현재 네비메쉬만 static으로 되어있음)
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		// 어떤 레이어오브젝트랑만 충돌할 것인지 ( 네비메쉬의 layerType을 Plane으로 설정해서 Plane만 체킹해줌)
		// 어떤레이어로 설정하고 싶으면 Set_ShapeLayer() 함수 사용
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
		// 부딪힌 오브젝트를 받아오기 위한 변수
		CGameObject* pHitObject = nullptr;
		// Out에 받아오기 때문에 주소값 던져줌
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			_float3 vMyPos = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
			_vector vMyPosVector = XMLoadFloat3(&vMyPos);

			_vector vDir = XMVector3Normalize(XMLoadFloat3(&tDescOut.vHitPos) - XMLoadFloat3(&vMyPos));
			_double dAdjustSpeed = ADJUST_SPEED;
			if (false == m_bBeforeJumping)
				dAdjustSpeed -= 0.01f;

			_float3 vVelocity = STOREFLOAT3(vDir*(_float)dAdjustSpeed);

			m_pCharacterController->Move(TimeDelta, vVelocity);
		}


	}
	RELEASE_INSTANCE(CGameInstance);

}

void CPlayer_TrampolineJump::SetUp_Double_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == m_bAdjustStartPos)
	{
		Reset_Anim(pPlayer_AnimState);
		m_fJumpPower = BASIC_JUMP_POWER;

		m_iUpperIndex = PLAYER_DOUBLE_JUMP;
		m_iRootIndex = PLAYER_DOUBLE_JUMP;
		m_iBottomIndex = PLAYER_DOUBLE_JUMP;
	}
	else
	{
		if (false == m_bBeforeJumping)
			Reset_Anim(pPlayer_AnimState);

		m_iUpperIndex = PLAYER_TRAMPOLINE_HIGH_LOOP;
		m_iRootIndex = PLAYER_TRAMPOLINE_HIGH_LOOP;
		m_iBottomIndex = PLAYER_TRAMPOLINE_HIGH_LOOP;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_TRAMPOLINE_TWO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("TrampolineOne.wav"), CHANNELID::PLAYER_TRAMPOLINE_TWO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		m_fJumpPower = SUPER_JUMP_POWER;
	}
	m_bFallCheck = true;
	//Double_Jump(pPlayer_AnimState, TimeDelta);

	++m_iJumpCount;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
	pPlayer_AnimState->Set_BackUpY(vPos.y);
	m_eJumpState = DOUBLE_JUMP_STATE;
	pPlayer_AnimState->Set_DoubleJumpCheck(true);
	pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
}

void CPlayer_TrampolineJump::SetUp_One_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	Reset_Anim(pPlayer_AnimState);
	m_fJumpPower = BASIC_JUMP_POWER;
	m_eJumpState = ONE_JUMP_STATE;

	//m_iUpperIndex = PLAYER_JUMP_UP;
	//m_iRootIndex = PLAYER_JUMP_UP;
	//m_iBottomIndex = PLAYER_JUMP_UP;

	pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_TrampolineJumpCheck(true);

	m_bBeforeJumping = false;
	++m_iJumpCount;
}

void CPlayer_TrampolineJump::ChangeAnim(CHNAGE_ANIM eChangeAnim, CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.7f, -0.7f, 5.84f, 2.045f), 1.74f, 1.f);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(90.f);
	switch (eChangeAnim)
	{
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_IDLE:
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		break;
	}
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_RUN:
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
		CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		break;
	}
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_JUMP:
	{
		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_LandCheck(true);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
		CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		break;
	}
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_LEDGE:
		Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		break;
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_SLIDER:
		Slider_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		break;
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_DOWNATTACK:
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->SetUp_State(CPlayer_DownAttack::Get_Instance());
		CPlayer_DownAttack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		break;
	}
	case Client::CPlayer_TrampolineJump::CHANGE_ANIM_END:
		break;
	default:
		break;
	}

	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.8f);

	RELEASE_INSTANCE(CGameInstance);

}

void CPlayer_TrampolineJump::Free()
{
}

