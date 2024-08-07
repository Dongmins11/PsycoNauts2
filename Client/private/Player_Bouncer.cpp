#include "stdafx.h"
#include "..\public\Player_Bouncer.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Bouncer.h"

IMPLEMENT_SINGLETON(CPlayer_Bouncer)

CPlayer_Bouncer::CPlayer_Bouncer()
{
}

void CPlayer_Bouncer::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;

	First_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);	// FSM Handle Input이 불렸을 때 최초 한번만 불리는 함수

	if (0 == m_iJumpCount && false == pPlayer_AnimState->Get_JumpCheck())
	{
		//pPlayer_AnimState->Set_JumpAcceleration(m_fSetValue);
		pPlayer_AnimState->Set_LandCheck(false);
		pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
		pPlayer_AnimState->Set_JumpCheck(true);
		++m_iJumpCount;
	}

	//Key_Check_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Fall_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Land_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	//if (true == Bouncer_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	//	return;
	//
	//if (true == Bouncer_to_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	//	return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Bouncer::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pTransform->Go_Straight(TimeDelta * m_fJumpGage, m_pCharacterController);

	m_pModel->Upper_Update(TimeDelta * 3.0);
	m_pModel->Root_Update(TimeDelta * 3.0);
	m_pModel->Bottom_Update(TimeDelta * 3.0);	
}

void CPlayer_Bouncer::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_AllLerpCheckTrue();

	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;
	m_iJumpCount = 0;
	for (_uint i = 0; i < 3; i++)
		m_pPlayer->Set_IsBouncer(false, i);

	Jump_Initialize(pPlayer_AnimState);
}

void CPlayer_Bouncer::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstCheck = false;
	m_iJumpCount = 0;
	for (_uint i = 0; i < 3; i++)
		m_pPlayer->Set_IsBouncer(false, i);

	Jump_Initialize(pPlayer_AnimState);
}

void CPlayer_Bouncer::First_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		Jump_Initialize(pPlayer_AnimState);

		m_iUpperIndex = PLAYER_BOUNCER_JUMP;
		m_iRootIndex = PLAYER_BOUNCER_JUMP;
		m_iBottomIndex = PLAYER_BOUNCER_JUMP;

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_BOUNCER_ROLL, 6.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_BOUNCER_ROLL, 6.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_BOUNCER_ROLL, 6.f);

		_uint iLayerIndex = 0;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		for (_uint i = 0; i < 3; i++)
			if (true == m_pPlayer->Get_IsBouncer(i))
				iLayerIndex = i;

		switch (iLayerIndex)
		{
		case 0:
			vArrivePoint = XMVectorSet(279.01f, 16.14f, 145.02f, 1.f);	// 객체 0의 점프 도착 지점.
			m_fJumpGage = 1.9f;
			m_fJumpPower = 6.f;
			break;
		case 1:
			vArrivePoint = XMVectorSet(287.35f, 16.38f, 154.18f, 1.f);	// 객체 1의 점프 도착 지점.
			m_fJumpGage = 2.f;
			m_fJumpPower = 6.f;
			break;
		case 2:
			vArrivePoint = XMVectorSet(190.86f, 13.03f, 143.92f, 1.f);	// 객체 2의 점프 도착 지점.
			m_fJumpGage = 4.6f;
			m_fJumpPower = 12.f;
			break;

		}

		_vector vNewLook = XMVector3Normalize(vArrivePoint - m_pTransform->Get_State(CTransform::STATE_POSITION));
		vNewLook = XMVectorSet(XMVectorGetX(vNewLook), 0.f, XMVectorGetZ(vNewLook), 0.f);

		m_pTransform->Set_MyLook(vNewLook);


		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_BOUNCER)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Bouncer.wav"), CHANNELID::PLAYER_BOUNCER)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_bFirstCheck = true;
	}
}

void CPlayer_Bouncer::Key_Check_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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
	}

	m_bIsMoveOn = bMoveCheck;
}

void CPlayer_Bouncer::Fall_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_BOUNCER_JUMP == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_BOUNCER_JUMP))
	{
		if (PALYER_LANDING != m_iRootIndex)
		{
			m_iUpperIndex = PLAYER_BOUNCER_ROLL;
			m_iRootIndex = PLAYER_BOUNCER_ROLL;
			m_iBottomIndex = PLAYER_BOUNCER_ROLL;
		}
	}
}

void CPlayer_Bouncer::Land_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_LandCheck())
	{
		m_iUpperIndex = PALYER_LANDING;
		m_iRootIndex = PALYER_LANDING;
		m_iBottomIndex = PALYER_LANDING;

		pPlayer_AnimState->Set_LandCheck(false);

		// Bouncer->Idle
		if (PALYER_LANDING == m_iRootIndex && false == m_bIsMoveOn)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		}
		// Bouncer->Run
		else if (PALYER_LANDING == m_iRootIndex && true == m_bIsMoveOn)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		}

		//이펙트추가 
		// 이펙트 생성지점 
		//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//
		//if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		//	pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_LandingSmoke"),
		//	SCENE_STATIC, TEXT("Effect_Smoke"))))
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}
		//
		///* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		//if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT//("Layer_LandingSmoke"))))
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}
		//RELEASE_INSTANCE(CGameInstance);
	}
}

_bool CPlayer_Bouncer::Bouncer_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

_bool CPlayer_Bouncer::Bouncer_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_Bouncer::Update_Player_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
}

void CPlayer_Bouncer::Jump_Initialize(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.6);
	//pPlayer_AnimState->Set_JumpAcceleration(0.7);
	pPlayer_AnimState->Set_FallSpeed(1.0);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
}

void CPlayer_Bouncer::Free()
{
}
