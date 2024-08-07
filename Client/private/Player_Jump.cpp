#include "stdafx.h"
#include "..\public\Player_Jump.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Floting.h"
#include "Player_Slider.h"
#include "Player_Ledge.h"
#include "Player_DownAttack.h"
#include "Player_TrampolineJump.h"
#include "Player_GroundSlide.h"
#include "Player_RailSlide.h"
#include "Player_Blender_Climbing.h"
#include "Player_Bouncer.h"
#include "Player_KitchenButton.h"
#include "Camera_Fly.h"
#include "Player_CloudAnim.h"

IMPLEMENT_SINGLETON(CPlayer_Jump)

CPlayer_Jump::CPlayer_Jump()
{
}

void CPlayer_Jump::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool	bMoveCheck = false;

 	m_bGravityCheck = true;

	First_JumpStateCheck();

	pPlayer_AnimState->PhysX_Slider();

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
				m_eJumpState = ONE_JUMP_STATE;
				pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
				pPlayer_AnimState->Set_LandCheck(false);
				pPlayer_AnimState->Set_JumpCheck(true);

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(CHANNELID::PLAYER_JUMP_ONE);

				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("DoubleJump.wav"), CHANNELID::PLAYER_JUMP_ONE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);

				++m_iJumpCount;
			}
			else if (1 == m_iJumpCount && PALYER_LANDING == m_iRootIndex)
			{
				Reset_Anim(pPlayer_AnimState);
				m_iJumpCount = 0;

				m_eJumpCheck = JUMP_NOMAL;
				m_eJumpState = ONE_JUMP_STATE;

				m_iUpperIndex = PLAYER_JUMP_UP;
				m_iRootIndex = PLAYER_JUMP_UP;
				m_iBottomIndex = PLAYER_JUMP_UP;

				++m_iJumpCount;

				pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
				pPlayer_AnimState->Set_JumpCheck(true);

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->StopSound(CHANNELID::PLAYER_JUMP_ONE);

				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("JumpOne.wav"), CHANNELID::PLAYER_JUMP_ONE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (1 == m_iJumpCount && false == pPlayer_AnimState->Get_DoubleJumpCheck())
			{
				Reset_Anim(pPlayer_AnimState);
				Double_Jump(pPlayer_AnimState, TimeDelta);

				++m_iJumpCount;

				_float4 vPos;
				XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
				pPlayer_AnimState->Set_BackUpY(vPos.y);
				m_eJumpState = DOUBLE_JUMP_STATE;
				pPlayer_AnimState->Set_DoubleJumpCheck(true);
				pPlayer_AnimState->Set_JumpPower(m_fJumpPower);

				//이단점프생성위치
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DonutRing"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DoubleJumpEffect"), &m_pTransform->Get_WorldMatrix())))
					return;

				/* For. Layer_DoubleJumpEffect 생성 */
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DoubleJumpEffect"),
					SCENE_STATIC, TEXT("Effect_DoubleJump"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				/* For. Layer_DoubleJumpEffect 상속 */
				if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_DoubleJumpEffect"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				pGameInstance->StopSound(CHANNELID::PLAYER_JUMP_TWO);

				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("JumpTwo.wav"), CHANNELID::PLAYER_JUMP_TWO)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (2 == m_iJumpCount && PALYER_LANDING == m_iRootIndex && 2 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
			{
				Reset_Anim(pPlayer_AnimState);
				m_iJumpCount = 0;

				m_eJumpCheck = JUMP_NOMAL;
				m_eJumpState = ONE_JUMP_STATE;

				m_iUpperIndex = PLAYER_JUMP_UP;
				m_iRootIndex = PLAYER_JUMP_UP;
				m_iBottomIndex = PLAYER_JUMP_UP;

				++m_iJumpCount;

				pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
				pPlayer_AnimState->Set_JumpCheck(true);


				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->StopSound(CHANNELID::PLAYER_JUMP_ONE);

				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("JumpOne.wav"), CHANNELID::PLAYER_JUMP_ONE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		if ('R' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (true == Fall_While_Jumping(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
				return;
		}
		if (VK_LBUTTON == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			if (true == Jump_to_DownAttack(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
				return;
		}
	}

	m_bIsMoveOn = bMoveCheck;

	Hold(pPlayer_AnimState, TimeDelta);

	Fall(pPlayer_AnimState, TimeDelta);

	Land(pPlayer_AnimState, TimeDelta);

	if (true == Jump_to_Ledge(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Jump_to_Slider(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_Trampoline(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_RailSlide(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Jump_to_Blender(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Jump_to_Bouncer(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Jump_to_KitchenButton(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Jump_to_Cloud(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
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

void CPlayer_Jump::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn && false == m_bGroundSlide)
	{
		Handle_Movement(pAnimKeyDesc);

		if (true == m_bBackUpCheck)
			Player_Look_Reverse_Move(TimeDelta);
		else
			Player_Move_NonMoveOffset(TimeDelta);

	}
	else if (true == m_bGroundSlide)
	{
		if (true == m_bIsMoveOn)
		{
			Handle_Movement(pAnimKeyDesc);
			Player_Move_NonMoveOffset(TimeDelta * 2);
		}
		else
			Player_Look_Reverse_Move(TimeDelta * 2);
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

void CPlayer_Jump::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iRootIndex = 0;
	m_iUpperIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
	//준표
	pPlayer_AnimState->Set_TrampolineJumpCheck(false);

	//혹시나 내가 블랜드 점프에서 온녀석이 체크가되어있따?
	if (true == m_bBlendJump)
		m_bBlendJump = false;

	if (true == m_bGroundSlide)
		m_bGroundSlide = false;
}

void CPlayer_Jump::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);

	m_iJumpCount = 0;

	m_bFirstCheck = false;
}

void CPlayer_Jump::Hold(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
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

void CPlayer_Jump::Double_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
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

void CPlayer_Jump::Fall(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	//if (true == pPlayer_AnimState->Get_FallCheck() && false == pPlayer_AnimState->Get_LandCheck())
	//{
	if (m_eJumpState == DOUBLE_JUMP_STATE)
	{
		if (m_eJumpCheck == JUMP_LEFT && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOUBLE_JUMP_LEFT))
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;
		}
		else if (m_eJumpCheck == JUMP_RIGHT && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOUBLE_JUMP_RIGHT))
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;
		}
		else if (m_eJumpCheck == JUMP_NOMAL && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOUBLE_JUMP))
		{
			m_iUpperIndex = PLAYER_FALLING;
			m_iRootIndex = PLAYER_FALLING;
			m_iBottomIndex = PLAYER_FALLING;
		}
	}
	else if (m_eJumpState == ONE_JUMP_STATE)
	{
		if (m_eJumpCheck == JUMP_LEFT && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_JUMP_LEFT_UP))
		{
			if (PLAYER_JUMP_LEFT_DOWN != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_JUMP_LEFT_DOWN;
				m_iRootIndex = PLAYER_JUMP_LEFT_DOWN;
				m_iBottomIndex = PLAYER_JUMP_LEFT_DOWN;
			}
		}
		else if (m_eJumpCheck == JUMP_RIGHT && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_JUMP_RIGHT_UP))
		{
			if (PLAYER_JUMP_RIGHT_DOWN != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_JUMP_RIGHT_DOWN;
				m_iRootIndex = PLAYER_JUMP_RIGHT_DOWN;
				m_iBottomIndex = PLAYER_JUMP_RIGHT_DOWN;
			}
		}
		else if (m_eJumpCheck == JUMP_NOMAL && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_JUMP_UP))
		{
			if (PLAYER_JUMP_DOWN != m_iRootIndex && PALYER_LANDING != m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_JUMP_DOWN;
				m_iRootIndex = PLAYER_JUMP_DOWN;
				m_iBottomIndex = PLAYER_JUMP_DOWN;
			}

		}

		if (PALYER_LANDING != m_iRootIndex)
		{
			if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_JUMP_LEFT_DOWN) ||
				true == m_pModel->Get_IsRootAnimFinishied(PLAYER_JUMP_RIGHT_DOWN) ||
				true == m_pModel->Get_IsRootAnimFinishied(PLAYER_JUMP_DOWN))
			{
				m_iUpperIndex = PLAYER_FALLING;
				m_iRootIndex = PLAYER_FALLING;
				m_iBottomIndex = PLAYER_FALLING;
			}
		}
	}

}

void CPlayer_Jump::Land(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_LandCheck())
	{
		if (m_eJumpCheck == JUMP_LEFT)
		{
			m_iUpperIndex = PALYER_LANDING;
			m_iRootIndex = PALYER_LANDING;
			m_iBottomIndex = PALYER_LANDING;
		}
		else if (m_eJumpCheck == JUMP_RIGHT)
		{
			m_iUpperIndex = PALYER_LANDING;
			m_iRootIndex = PALYER_LANDING;
			m_iBottomIndex = PALYER_LANDING;
		}
		else if (m_eJumpCheck == JUMP_NOMAL)
		{
			m_iUpperIndex = PALYER_LANDING;
			m_iRootIndex = PALYER_LANDING;
			m_iBottomIndex = PALYER_LANDING;
		}
		pPlayer_AnimState->Set_LandCheck(false);

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

		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_LANDING)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerLand.wav"), CHANNELID::PLAYER_LANDING)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

}

_bool CPlayer_Jump::Jump_to_Ledge(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_pPlayer->Get_IsParkour() && true == m_bIsMoveOn)
	{
		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_LEFT);

			Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_FRONT);

			Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_RIGHT);

			Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			CPlayer_Ledge::Get_Instance()->Set_LookState(LOOK_BACK);

			Ledge_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
	}

	return false;
}

_bool CPlayer_Jump::Jump_to_Slider(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_Slider() && true == m_bIsMoveOn)
	{
		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_LEFT);

			Slider_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_FRONT);

			Slider_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_RIGHT);

			Slider_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			if (true == m_pPlayer->Get_IsParkour())
				m_pPlayer->Set_IsParkour(false);

			CPlayer_Slider::Get_Instance()->Set_LookState(LOOK_BACK);

			Slider_Reset(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			return true;
		}
	}

	return false;
}

_bool CPlayer_Jump::Jump_to_DownAttack(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING != m_iRootIndex)
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->SetUp_State(CPlayer_DownAttack::Get_Instance());
		CPlayer_DownAttack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

_bool CPlayer_Jump::Fall_While_Jumping(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

_bool CPlayer_Jump::Land_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
		{
			m_iUpperIndex = 0;
			m_iRootIndex = 0;
			m_iBottomIndex = 0;

			Reset_Anim(pPlayer_AnimState);
			m_iJumpCount = 0;
			m_bFirstCheck = false;
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_Jump::Land_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex && 3 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING))
	{
		if (true == m_bIsMoveOn)
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
			return true;
		}
	}

	return false;
}

_bool CPlayer_Jump::Fall_to_GroundSlide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_LandCheck() && true == m_pPlayer->Get_IsSlide())
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_GroundSlide::Get_Instance());
		CPlayer_GroundSlide::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		pPlayer_AnimState->Set_LandCheck(false);

		//이펙트추가 
		// 이펙트 생성지점 
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_LandingSmoke"),
			SCENE_STATIC, TEXT("Effect_Smoke"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_LandingSmoke"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}
		RELEASE_INSTANCE(CGameInstance);

		return true;
	}

	return false;
}


_bool CPlayer_Jump::Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_TrampolineTriggerCheck())
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		//Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
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

_bool CPlayer_Jump::Land_to_RailSlide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_RailSlideCheck())
	{

		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		m_iJumpCount = 0;
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_RailSlide::Get_Instance());
		CPlayer_RailSlide::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.8f, 0.1f, 8.f, 9.f), 0.2f, 1.f);
		RELEASE_INSTANCE(CGameInstance);
		return true;

	}
	return false;
}

_bool CPlayer_Jump::Jump_to_Blender(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	if (true == m_pPlayer->Get_Climbing_TriggerCheck() && false == m_bBlendJump)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		_float3 vVelocity;
		_float4 vUpRayDirection;
		_float3 vPlayerPos;
		XMStoreFloat3(&vPlayerPos, m_pTransform->Get_State(CTransform::STATE_POSITION));

		XMStoreFloat3(&tDescIn.vOrigin, XMVectorSet(vPlayerPos.x, vPlayerPos.y + 0.9f, vPlayerPos.z, 1.f));
		XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK)));

		tDescIn.fDistance = 0.6f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_CLIMBINGPLANE);
		CGameObject* pHitObject = nullptr;
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (pHitObject)
			{
				((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(100.f, 1.f);
				m_bFirstCheck = false;
				m_pTransform->Set_State_Float3(CTransform::STATE_POSITION, _float3(tDescOut.vHitPos.x, tDescOut.vHitPos.y - 0.9f, tDescOut.vHitPos.z));
				Reset_Anim(pPlayer_AnimState);
				m_iJumpCount = 0;
				m_iUpperIndex = 0;
				m_iRootIndex = 0;
				m_iBottomIndex = 0;
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Blender_Climbing::Get_Instance());
				//m_pPlayer->Set_조건(false) //필요한경우에만 위의 조건문에 true로들어오고 false로자동으로 바뀌는게없으면 여기서처리하거나
				//blender FSM에서 RESET해줄때 FALSE해주거나 해주면됨
				CPlayer_Blender_Climbing::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				RELEASE_INSTANCE(CGameInstance);
				return true;
			}
			else
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}
	}
	return false;	
}

_bool CPlayer_Jump::Jump_to_Cloud(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if ('V' == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_DOWN)
	{
		if (true == pPlayer_AnimState->Cloud_RayCast_Ready(TimeDelta))
		{
			if (nullptr != pPlayer_AnimState)
			{
				Reset_Anim(pPlayer_AnimState);
				m_iJumpCount = 0;
				m_iUpperIndex = 0;
				m_iRootIndex = 0;
				m_iBottomIndex = 0;
				m_bFirstCheck = false;
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

_bool CPlayer_Jump::Jump_to_Bouncer(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < 3; i++)
	{
		if (true == m_pPlayer->Get_IsBouncer(i))
		{
			m_iUpperIndex = 0;
			m_iRootIndex = 0;
			m_iBottomIndex = 0;

			m_iJumpCount = 0;
			m_bFirstCheck = false;

			Reset_Anim(pPlayer_AnimState);

			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_Bouncer::Get_Instance());
			CPlayer_Bouncer::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_Jump::Jump_to_KitchenButton(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < 3; i++)
	{
		if (true == m_pPlayer->Get_IsKitchenButton(i))
		{
			m_iUpperIndex = 0;
			m_iRootIndex = 0;
			m_iBottomIndex = 0;

			m_iJumpCount = 0;
			m_bFirstCheck = false;

			Reset_Anim(pPlayer_AnimState);

			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_KitchenButton::Get_Instance());
			CPlayer_KitchenButton::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}


void CPlayer_Jump::Slider_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_Jump::Ledge_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_Jump::Set_BlendSpeed()
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

void CPlayer_Jump::First_JumpStateCheck()
{
	if (false == m_bFirstCheck)
	{
		m_fJumpPower = 6.85f;
		m_iRandomState = rand() % 3;
		Set_BlendSpeed();
		Random_JumpStateCheck();

		if (0 == m_iRandomState)
		{
			m_iUpperIndex = PLAYER_JUMP_LEFT_UP;
			m_iRootIndex = PLAYER_JUMP_LEFT_UP;
			m_iBottomIndex = PLAYER_JUMP_LEFT_UP;

			m_eJumpCheck = JUMP_LEFT;
		}
		else if (1 == m_iRandomState)
		{
			m_iUpperIndex = PLAYER_JUMP_RIGHT_UP;
			m_iRootIndex = PLAYER_JUMP_RIGHT_UP;
			m_iBottomIndex = PLAYER_JUMP_RIGHT_UP;

			m_eJumpCheck = JUMP_RIGHT;
		}
		else if (2 == m_iRandomState)
		{
			m_iUpperIndex = PLAYER_JUMP_UP;
			m_iRootIndex = PLAYER_JUMP_UP;
			m_iBottomIndex = PLAYER_JUMP_UP;

			m_eJumpCheck = JUMP_NOMAL;
		}

		m_iBackUpJumpState = m_iRandomState;
		m_bFirstCheck = true;
	}
}

void CPlayer_Jump::Random_JumpStateCheck()
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

void CPlayer_Jump::Free()
{
}

