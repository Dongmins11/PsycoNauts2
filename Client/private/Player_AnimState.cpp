#include "stdafx.h"
#include "..\public\Player_AnimState.h"
#include "Player.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player_ParentAnim.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_StopAnim.h"
#include "Player_ComboAttack.h"
#include "Player_Pisblast.h"
#include "Player_Dash.h"
#include "Player_Dash_Attack.h"
#include "Player_Ball.h"
#include "Player_GrabAttack.h"
#include "Player_GrabBack.h"
#include "Player_Jump.h"
#include "Player_Walk.h"
#include "Player_Floting.h"
#include "Player_Landing.h"
#include "Player_Falling.h"
#include "Player_LeftHand.h"
#include "Player_RightHand.h"
#include "Player_Slider.h"
#include "Player_Dodge.h"
#include "Player_BrainStom.h"
#include "Player_DownAttack.h"
#include "Player_Hit.h"
#include "Player_BrainWashing.h"
#include "Player_Ledge.h"
#include "Player_LedgeJump.h"
#include "Player_LedgeMove.h"
#include "Player_WalkStop.h"
#include "Player_TrampolineJump.h"
#include "Player_GroundSlide.h"
#include "Player_RailSlide.h"
#include "Player_PullSwitch.h"
#include "Player_PullSwitch_Move.h"
#include "Player_Bouncer.h"
#include "Player_Blender_Climbing.h"
#include "Player_Blender_Climbing_Jump.h"
#include "Player_Blender_Climbing_JumpEnd.h"
#include "Player_Dead.h"
#include "Player_KitchenButton.h"
#include "Player_CloudAnim.h"
#include "Player_CloudAnim_End.h"
#include "Player_KnockBack.h"
#include "Player_Food_Throw.h"
#include "Player_Throw_Left.h"
#include "Player_Throw_Right.h"
#include "Player_Dish_Throw.h"
#include "Cloud.h"


CPlayer_AnimState::CPlayer_AnimState()
{
}

HRESULT CPlayer_AnimState::NativeConstruct(CPlayer * pPlayer, CTransform * pPlayerTrasnform, CModel * pPlayerModel, CSphereCollider * pPlayerColider, CNavigation * pPlayerNavigation, CCharacterController* pCharacterController, CPlayer_LeftHand* pLeftHand, CPlayer_RightHand* pRightHand, CPlayerTrigger_Parkour* pTriggerParkour, _uint iScene)
{
	if (nullptr == pPlayer || nullptr == pPlayerTrasnform || nullptr == pPlayerModel || nullptr == pCharacterController || nullptr == pLeftHand || 0 > iScene)
	{
		MSGBOX("null Player_AnimState");
		return E_FAIL;
	}



	m_pPlayer = pPlayer;
	m_pTransformCom = pPlayerTrasnform;
	m_pModel = pPlayerModel;
	m_pColliderCom = pPlayerColider;
	m_pNavigation = pPlayerNavigation;
	m_pCharacterController = pCharacterController;
	m_pTrigger_Parkour = pTriggerParkour;
	m_iScene = iScene;

	m_pLeftHand = pLeftHand;
	m_pRightHand = pRightHand;

	ZeroMemory(&m_tAnimDesc, sizeof(ANIMDESC));


	CPlayer_IdleAnim::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_RunAnim::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_StopAnim::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_ComboAttack::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Pisblast::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Dash::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Dash_Attack::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Ball::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_GrabAttack::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_GrabBack::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Jump::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Walk::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_WalkStop::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Floting::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Falling::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Landing::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Slider::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Dodge::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_BrainStom::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_DownAttack::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Hit::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_BrainWashing::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Ledge::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_LedgeJump::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_LedgeMove::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_TrampolineJump::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_GroundSlide::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_RailSlide::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_PullSwitch::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_PullSwitch_Move::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Bouncer::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Blender_Climbing::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Blender_Climbing_Jump::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Blender_Climbing_JumpEnd::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Dead::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_KitchenButton::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_CloudAnim::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_CloudAnim_End::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_KnockBack::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Dish_Throw::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Food_Throw::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Throw_Left::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);
	CPlayer_Throw_Right::Get_Instance()->Set_StateInfo(m_pPlayer, m_pTransformCom, m_pModel, m_pColliderCom, m_pNavigation, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTrigger_Parkour, m_iScene);



	m_pPlayer_AnimState = CPlayer_IdleAnim::Get_Instance();
	
	m_fGravitySpeedOffset = 0.5f;
	m_lfFallAcceleration = 0.6;
	m_fFloatingSpeed = 1.f;
	m_fMaxSpeed = 4.f;

	m_lfAttackMoveSpeed = 1.0;
	m_lfDashAttackMoveSpeed = 1.0;

	return S_OK;
}

void CPlayer_AnimState::Set_AllLerpCheckTrue()
{
	m_bUpperAnimLerp = true;
	m_bRootAnimLerp = true;
	m_bBottomAnimLerp = true;
}

void CPlayer_AnimState::Set_RootBottomLerpCheckTrue()
{
	m_bRootAnimLerp = true;
	m_bBottomAnimLerp = true;
}

void CPlayer_AnimState::Set_OneLerpCheckTrue()
{
	m_bAnimLerp = true;
}

void CPlayer_AnimState::SetUp_State(CPlayer_ParentAnim * pAnimState)
{
	m_pPlayer_AnimState = pAnimState;
}

void CPlayer_AnimState::Update(_double TimeDelta)
{
	ZeroMemory(&m_tAnimDesc, sizeof(ANIMDESC));

	if (false == m_pPlayer->Get_MoveCheck())
		Key_Set(TimeDelta);

	Player_Dead(TimeDelta);

	m_pPlayer_AnimState->HandleInput(this, m_tAnimDesc, TimeDelta);

	Lerp_Check();

	Player_Jump(TimeDelta);

	Player_Gravity(TimeDelta);

	m_pPlayer_AnimState->Update(this, m_tAnimDesc, TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down('P'))
	{
		m_bWalkingCheck = true;
	}
	if (pGameInstance->Key_Down('O'))
		m_bWalkingCheck = false;

	Safe_Release(pGameInstance);

}

void CPlayer_AnimState::Player_Jump(_double TimeDelta)
{
	if (true == m_bJumpCheck)
	{
		if (true == m_bFallCheck)
			m_lfFallSpeed += TimeDelta * m_lfFallAcceleration;

		m_lfJumpTimeAcc += TimeDelta * 0.7 * m_lfFallSpeed;
		m_pTransformCom->Jump(TimeDelta, &m_bJumpCheck, &m_fBackUpY, &m_lfJumpTimeAcc, &m_fJumpPower, &m_fJumpTimeValue, &m_bFallCheck, &m_bLandCheck, m_pCharacterController);
	}
	if (true == m_bDoubleJumpCheck)
	{
		if (true == m_bFallCheck)
			m_lfFallSpeed += TimeDelta * m_lfFallAcceleration;

		m_lfJumpTimeAcc += TimeDelta * 0.7 * m_lfFallSpeed;
		m_pTransformCom->Double_Jump(TimeDelta, &m_bDoubleJumpCheck, &m_fBackUpY, nullptr, &m_lfJumpTimeAcc, &m_fJumpPower, &m_fJumpTimeValue, &m_bFallCheck, &m_bLandCheck, m_pCharacterController);
	}
	if (true == m_bDownAttackCheck)
	{
		if (true == m_bFallCheck)
			m_lfFallSpeed += TimeDelta * 30.0;

		m_lfJumpTimeAcc += TimeDelta * 1.2 * m_lfFallSpeed;
		m_pTransformCom->Jump(TimeDelta, &m_bDownAttackCheck, &m_fBackUpY, &m_lfJumpTimeAcc, &m_fJumpPower, &m_fJumpTimeValue, &m_bFallCheck, &m_bLandCheck, m_pCharacterController);
	}
	if (true == m_bLedgeJumpCheck)
	{
		if (true == m_bFallCheck)
			m_lfFallSpeed += TimeDelta * m_lfFallAcceleration;

		m_lfJumpTimeAcc += TimeDelta * 2.0 * m_lfFallSpeed;
		m_pTransformCom->Ledge_Jump(TimeDelta, &m_bLedgeJumpCheck, &m_fBackUpY, &m_lfJumpTimeAcc, &m_fJumpPower, &m_fJumpTimeValue, &m_bFallCheck, &m_bLandCheck, &m_bLedgeCheck, m_pCharacterController);
	}
	if (true == m_bTrampolineJumpCheck)
	{
		if (true == m_bFallCheck)
			m_lfFallSpeed += TimeDelta * m_lfFallAcceleration;

		m_lfJumpTimeAcc += TimeDelta * 0.7 * m_lfFallSpeed;
		m_pTransformCom->Jump(TimeDelta, &m_bTrampolineJumpCheck, &m_fBackUpY, &m_lfJumpTimeAcc, &m_fJumpPower, &m_fJumpTimeValue, &m_bFallCheck, &m_bLandCheck, m_pCharacterController);
	}
}

void CPlayer_AnimState::Player_Gravity(_double TimeDelta)
{
	if (false == m_pPlayer_AnimState->Get_GravityCheck())
	{
		if (!m_pCharacterController->IsDown())
		{
			m_fGravitySpeed += (_float)(TimeDelta * m_fGravitySpeedOffset) * m_fFloatingSpeed;

			if (m_fMaxSpeed <= m_fGravitySpeed)
				m_fGravitySpeed = m_fMaxSpeed;

			m_bFloatingCheck = true;

			if (0.1f <= m_fGravitySpeed)
				m_bGravityCheck = true;

			m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -m_fGravitySpeed);
		}
		else
		{
			m_bFloatingCheck = false;
			m_bGravityCheck = false;
			m_fGravitySpeed = 0.f;
		}
	}
	else
	{
		m_bFloatingCheck = false;
		m_bGravityCheck = false;
		m_fGravitySpeed = 0.f;
	}
}

void CPlayer_AnimState::Player_Dead(_double TimeDelta)
{
	if (true == m_pPlayer->Get_IsDead())
	{
		m_pPlayer_AnimState->Dead_Reset(this);
		m_pPlayer_AnimState = CPlayer_Dead::Get_Instance();
		m_pPlayer->Set_IsDamage(true);
		m_pPlayer->Set_IsDead(false);
	}

}

void CPlayer_AnimState::Player_Hit(_bool* bHitCheck, _double TimeDelta)
{
	if (true == *bHitCheck && false == m_IsHiting)
	{
		m_IsHiting = true;

		if (true == m_pPlayer->Get_IsKnockBack())
		{
			m_pPlayer_AnimState->Dead_Reset(this);
			m_pPlayer_AnimState = CPlayer_KnockBack::Get_Instance();
			*bHitCheck = false;
			m_pPlayer->Set_IsDamage(true);
		}
		if (m_pPlayer_AnimState != CPlayer_KnockBack::Get_Instance() && false == m_bHitCheck)
		{
			m_pPlayer->Set_IsDamage(true);
			*bHitCheck = false;
			m_bHitCheck = true;
			m_IsRimLightEnable = true;
		}
		if (true == m_pPlayer->Get_CamShake())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CollisionShaking(true);
			RELEASE_INSTANCE(CGameInstance);
		}

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iRand = rand() % 3;

		if (0 == iRand)
		{
			if (false == pGameInstance->IsChannelPlaying(PLAYER_HIT_ONE))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerHitOne.wav"), CHANNELID::PLAYER_HIT_ONE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

			}
			else if (false == pGameInstance->IsChannelPlaying(PLAYER_HIT_TWO))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerHitTwo.wav"), CHANNELID::PLAYER_HIT_TWO)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
		}
		else if (1 == iRand)
		{
			if (false == pGameInstance->IsChannelPlaying(PLAYER_HIT_TWO))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerHitTwo.wav"), CHANNELID::PLAYER_HIT_TWO)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
			else if (false == pGameInstance->IsChannelPlaying(PLAYER_HIT_ONE))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerHitOne.wav"), CHANNELID::PLAYER_HIT_ONE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

			}
		}
		else if (2 == iRand)
		{
			if (false == pGameInstance->IsChannelPlaying(PLAYER_HIT_THREE))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerHitThree.wav"), CHANNELID::PLAYER_HIT_THREE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
			else if (false == pGameInstance->IsChannelPlaying(PLAYER_HIT_TWO))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerHitTwo.wav"), CHANNELID::PLAYER_HIT_TWO)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);

	}

	if (true == m_IsHiting)
	{
		m_lfHitResetTimeAcc += TimeDelta;

		Hiting_RimLight(TimeDelta);

		if (true == m_bHitCheck)
			++m_iHitCount;

		if (1 < m_iHitCount)
		{
			m_iHitCount = 0;
			m_bHitCheck = false;
		}

		if (2.0 <= m_lfHitResetTimeAcc)
		{
			m_IsHiting = false;
			m_lfHitResetTimeAcc = 0.0;

			m_IsRimLightEnable = false;
			m_lfRimLightTimeAcc = 0.0;
		}
	}

}

void CPlayer_AnimState::Hiting_RimLight(_double TimeDelta)
{
	if (true == m_IsRimLightEnable)
	{
		m_lfRimLightTimeAcc += TimeDelta;

		if (0.0 <= m_lfRimLightTimeAcc && 0.3 >= m_lfRimLightTimeAcc)
		{
			m_pPlayer->Set_RimLightEnable(true);
			m_pPlayer->Set_RimPower(0.5f);
			m_pPlayer->Set_RGBA(_float4(1.f, 0.f, 0.f, 1.f));
		}
		else if (0.3 < m_lfRimLightTimeAcc && 0.7 >= m_lfRimLightTimeAcc)
		{
			m_pPlayer->Set_RimLightEnable(false);
			m_pPlayer->Set_RimPower(3.f);
			m_pPlayer->Set_RGBA(_float4(1.f, 1.f, 1.f, 1.f));
		}
		else if (0.7 < m_lfRimLightTimeAcc && 1.0 >= m_lfRimLightTimeAcc)
		{
			m_pPlayer->Set_RimLightEnable(true);
			m_pPlayer->Set_RimPower(0.5f);
			m_pPlayer->Set_RGBA(_float4(1.f, 0.f, 0.f, 1.f));
		}
		else if (1.5 < m_lfRimLightTimeAcc)
		{
			m_pPlayer->Set_RimLightEnable(false);
			m_pPlayer->Set_RimPower(3.f);
			m_pPlayer->Set_RGBA(_float4(1.f, 1.f, 1.f, 1.f));
		}
	}
}

_bool CPlayer_AnimState::Get_AttackCheck() const
{
	if (nullptr == m_pPlayer_AnimState)
		return false;

	return m_pPlayer_AnimState->Get_AttackCheck();
}

void CPlayer_AnimState::Key_Set(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_iKeyStack = 0;

	if (pGameInstance->Key_Pressing('W'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'W';
		m_tAnimDesc.iOnceKey = 'W';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (pGameInstance->Key_Up('W'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'W';
		m_tAnimDesc.iOnceKey = 'W';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (pGameInstance->Key_Pressing('S'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'S';
		m_tAnimDesc.iOnceKey = 'S';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (pGameInstance->Key_Up('S'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'S';
		m_tAnimDesc.iOnceKey = 'S';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (pGameInstance->Key_Pressing('A'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'A';
		m_tAnimDesc.iOnceKey = 'A';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (pGameInstance->Key_Up('A'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'A';
		m_tAnimDesc.iOnceKey = 'A';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Pressing('D'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'D';
		m_tAnimDesc.iOnceKey = 'D';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (pGameInstance->Key_Up('D'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'D';
		m_tAnimDesc.iOnceKey = 'D';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Down(VK_LBUTTON))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_LBUTTON;
		m_tAnimDesc.iOnceKey = VK_LBUTTON;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}
	if (true == Key_Up(VK_LBUTTON))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_LBUTTON;
		m_tAnimDesc.iOnceKey = VK_LBUTTON;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Down(VK_RBUTTON))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_RBUTTON;
		m_tAnimDesc.iOnceKey = VK_RBUTTON;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;

	}

	if (true == Key_Up(VK_RBUTTON))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_RBUTTON;
		m_tAnimDesc.iOnceKey = VK_RBUTTON;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Down(VK_SHIFT))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_SHIFT;
		m_tAnimDesc.iOnceKey = VK_SHIFT;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Pressing(VK_SPACE))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_SPACE;
		m_tAnimDesc.iOnceKey = VK_SPACE;
		m_tAnimDesc.eOneceKeyState = KEY_PRESING;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (true == pGameInstance->Key_Down(VK_SPACE))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_SPACE;
		m_tAnimDesc.iOnceKey = VK_SPACE;
		m_tAnimDesc.eOneceKeyState = KEY_DOWN;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}


	if (true == Key_Up(VK_SPACE))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_SPACE;
		m_tAnimDesc.iOnceKey = VK_SPACE;
		m_tAnimDesc.eOneceKeyState = KEY_UP;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}



	if (pGameInstance->Key_Pressing('E'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'E';
		m_tAnimDesc.iOnceKey = 'E';
		m_tAnimDesc.eOneceKeyState = KEY_PRESING;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (true == pGameInstance->Key_Down('E'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'E';
		m_tAnimDesc.iOnceKey = 'E';
		m_tAnimDesc.eOneceKeyState = KEY_DOWN;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}


	if (true == Key_Up('E'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'E';
		m_tAnimDesc.iOnceKey = 'E';
		m_tAnimDesc.eOneceKeyState = KEY_UP;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Pressing('R'))
	{
	m_tAnimDesc.iKey[m_iKeyStack] = 'R';
	m_tAnimDesc.iOnceKey = 'R';
	m_tAnimDesc.eOneceKeyState = KEY_PRESING;
	m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
	++m_tAnimDesc.iKeyCount;
	++m_iKeyStack;
	}

	if (true == pGameInstance->Key_Down('R'))
	{
	m_tAnimDesc.iKey[m_iKeyStack] = 'R';
	m_tAnimDesc.iOnceKey = 'R';
	m_tAnimDesc.eOneceKeyState = KEY_DOWN;
	m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
	++m_tAnimDesc.iKeyCount;
	++m_iKeyStack;
	}


	if (true == Key_Up('R'))
	{
	m_tAnimDesc.iKey[m_iKeyStack] = 'R';
	m_tAnimDesc.iOnceKey = 'R';
	m_tAnimDesc.eOneceKeyState = KEY_UP;
	m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
	++m_tAnimDesc.iKeyCount;
	++m_iKeyStack;
	}


	if (pGameInstance->Key_Pressing('Q'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'Q';
		m_tAnimDesc.iOnceKey = 'Q';
		m_tAnimDesc.eOneceKeyState = KEY_PRESING;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (true == pGameInstance->Key_Down('Q'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'Q';
		m_tAnimDesc.iOnceKey = 'Q';
		m_tAnimDesc.eOneceKeyState = KEY_DOWN;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}


	if (true == Key_Up('Q'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'Q';
		m_tAnimDesc.iOnceKey = 'Q';
		m_tAnimDesc.eOneceKeyState = KEY_UP;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}



	if (pGameInstance->Key_Pressing('T'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'T';
		m_tAnimDesc.iOnceKey = 'T';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Down('T'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'T';
		m_tAnimDesc.iOnceKey = 'T';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Pressing('Y'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'Y';
		m_tAnimDesc.iOnceKey = 'Y';
		m_tAnimDesc.eOneceKeyState = KEY_PRESING;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (true == pGameInstance->Key_Down('Y'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'Y';
		m_tAnimDesc.iOnceKey = 'Y';
		m_tAnimDesc.eOneceKeyState = KEY_DOWN;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}


	if (true == Key_Up('Q'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'Q';
		m_tAnimDesc.iOnceKey = 'Q';
		m_tAnimDesc.eOneceKeyState = KEY_UP;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_UP;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Down('I'))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'I';
		m_tAnimDesc.iOnceKey = 'I';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Down('V')) /* 성준 Cloud 구름 */
	{
		m_tAnimDesc.iKey[m_iKeyStack] = 'V';
		m_tAnimDesc.iOnceKey = 'V';
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_DOWN;
		m_tAnimDesc.eOneceKeyState = KEY_DOWN;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	if (pGameInstance->Key_Pressing(VK_NUMLOCK))
	{
		m_tAnimDesc.iKey[m_iKeyStack] = VK_NUMLOCK;
		m_tAnimDesc.iOnceKey = VK_NUMLOCK;
		m_tAnimDesc.eOneceKeyState = KEY_PRESING;
		m_tAnimDesc.eKeyState[m_iKeyStack] = KEY_PRESING;
		++m_tAnimDesc.iKeyCount;
		++m_iKeyStack;
	}

	Safe_Release(pGameInstance);
}

_bool CPlayer_AnimState::Key_Up(_uint _key)
{
	if (m_bKeyState[_key] && !(GetAsyncKeyState(_key) & 0x8000))
	{
		m_bKeyState[_key] = !m_bKeyState[_key];
		return true;
	}
	if (!m_bKeyState[_key] && (GetAsyncKeyState(_key) & 0x8000))
		m_bKeyState[_key] = !m_bKeyState[_key];

	return false;
}

void CPlayer_AnimState::Lerp_Check()
{
	_bool bUpperAnimLerp = false;
	_bool bRootAnimLerp = false;
	_bool bBottomAnimLerp = false;
	_bool bAnimLerp = false;

	if (true == m_bUpperAnimLerp)
	{
		bUpperAnimLerp = true;
		m_bUpperAnimLerp = false;
	}
	if (true == m_bRootAnimLerp)
	{
		bRootAnimLerp = true;
		m_bRootAnimLerp = false;
	}
	if (true == m_bBottomAnimLerp)
	{
		bBottomAnimLerp = true;
		m_bBottomAnimLerp = false;
	}
	if (true == m_bAnimLerp)
	{
		bAnimLerp = true;
		m_bAnimLerp = false;
	}

	if (true == bUpperAnimLerp)
		m_pModel->Set_IsUpperLefp(bUpperAnimLerp);
	if (true == bRootAnimLerp)
		m_pModel->Set_IsBottomLefp(bRootAnimLerp);
	if (true == bBottomAnimLerp)
		m_pModel->Set_IsRootLefp(bBottomAnimLerp);
	if (true == bAnimLerp)
		m_pModel->Set_IsAnimLefp(bAnimLerp);
}

void CPlayer_AnimState::PhysX_Slider()
{
	// #수정사항 : 피직스
	// m_pCharacterController->IsSides() : 피직스에서 플레이어의 4방향에 
	// 방해물이 부딪혔을때 true반환 함수
	if (m_pCharacterController->IsSides() && !m_pCharacterController->IsDown() && false == m_isSliding)
	{
		if (m_isSlidingCheck || m_isSlidingTriggerCheck)
			return;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		// raycasting 의 시작지점 ( 지금 player의 발쪽에 아마 되어있을 가능성이 높음 아마 몸통부분으로 수정해줘야할듯 )
		XMStoreFloat3(&tDescIn.vOrigin, m_pTransformCom->Get_Position() +XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))*0.35f);// ;
																		 // raycasting 방향 ( 플레이어의 look방향이 기본(벽탈때 보통 플레이어 기준 앞쪽을 바라보기 때문)
		XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
		// raycast 길이
		tDescIn.fDistance = 5.0f;
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
			if (pHitObject)
			{
				const _tchar* pName = pHitObject->Get_LayerName();
				// 점프상태 일때 checking 해서 sliding상태도 들어가면됌
				// 그 상태일때 중력값 바꿔서 슬라이딩 하는걸로 변경
				//if (Get_JumpCheck() || Get_DoubleJumpCheck())
				//{
				m_isSliding = true;
				//}
			}
		}
		else
		{
			// 사이드는 플레이어 기준 4방향으로 체킹해줌 벽을 보고있지 않을경우
			// 여기서 설정
			if (m_isSliding)
				m_isSliding = false;
		}
		RELEASE_INSTANCE(CGameInstance);

		if (m_isSliding)
		{
			OutputDebugStringW(TEXT("Sliding중 \n"));
		}
		else
		{
			OutputDebugStringW(TEXT("Sliding 아님!!\n"));
		}
	}
	else if (!m_pCharacterController->IsSides())// 플레이어기준 4방향으로 방해물이 없다고 판단될 경우 들어가는 else문
	{
		if (m_isSliding)
			m_isSliding = false;
	}
}

_bool CPlayer_AnimState::Check_JumpAnimstate()
{
	_bool bCheck = false;

	if (m_pPlayer_AnimState == CPlayer_Jump::Get_Instance())
		bCheck = true;

	return bCheck;
}

void CPlayer_AnimState::Set_CollisionObject(CGameObject * pGameObject)
{
	if (nullptr != pGameObject)
	{
		m_pCollisionObject = pGameObject;
	}
}

void CPlayer_AnimState::Set_CloudObject(CGameObject * pGameObject)
{
	m_pCloudHitObject = pGameObject;
}

void CPlayer_AnimState::Set_RailValues(_double dStartSpeed, _double dDefaultSpeed, _double dFastSpeed, _double dSlowSpeed, _double dNonFixJumpPower, _double dRailCollisionCheckTerm, _double dJumpCorrection, _double dStartJumpAccMax, _double dRayTrans, _double dJumpPower, _double dJumpFallSpeed, _double dLookSpeed, _double dMoveSpeed, _double dAdjustSpeed)
{
	if (CPlayer_RailSlide::Get_Instance() == m_pPlayer_AnimState)
		dynamic_cast<CPlayer_RailSlide*>(m_pPlayer_AnimState)->Set_RailValues(dStartSpeed, dDefaultSpeed, dFastSpeed, dSlowSpeed, dNonFixJumpPower, dRailCollisionCheckTerm, dJumpCorrection, dStartJumpAccMax, dRayTrans, dJumpPower, dJumpFallSpeed, dLookSpeed, dMoveSpeed, dAdjustSpeed);
}

void CPlayer_AnimState::Grab_RayCast_Ready(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr == m_pHitObject)
	{
		_vector vRayPos, vRayDir;
		vRayPos = pGameInstance->Get_RayPos();
		vRayDir = XMVector3Normalize(pGameInstance->Get_RayDir());
		const _tchar* pLayerTag;

		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		XMStoreFloat3(&tDescIn.vOrigin, vRayPos);
		XMStoreFloat3(&tDescIn.vUnitDir, vRayDir);
		tDescIn.fDistance = 50.f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PICK)| (1 << (_uint)LayerType::LAYERTYPE_MONSTER_FRUIT);
		tDescOut.ppHitObject = &m_pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (m_pHitObject)
			{
				//Setting_Throwing

				if (true == m_pHitObject->Get_IsPulling())
					m_bGrabBack = true;
				else
					m_bGrabAttack = true;

				m_bGrabCheck = true;
				pLayerTag = m_pHitObject->Get_LayerName();
				m_pHitObject->Set_IsPicking(true);
				Set_Position_RightHand(TimeDelta);

				if (true == m_bGrabBack)
					m_pHitObject = nullptr;
			}
		}

	}
	Safe_Release(pGameInstance);
}

void CPlayer_AnimState::Grab_RayCast_Loop(_double TimeDelta)
{
	if (true == m_bGrabAttack)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//Safe_AddRef(pGameInstance);

		if (nullptr == m_pThrowTargetObject)
		{
			_vector vRayPos, vRayDir;
			vRayPos = pGameInstance->Get_RayPos();
			vRayDir = XMVector3Normalize(pGameInstance->Get_RayDir());
			//const _tchar* pLayerTag;

			CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
			CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
			XMStoreFloat3(&tDescIn.vOrigin, vRayPos);
			XMStoreFloat3(&tDescIn.vUnitDir, vRayDir);
			tDescIn.fDistance = 50.f;
			tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC;
			tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_MONSTER) | (1 << (_uint)LayerType::LAYERTYPE_PLATINGPLATE);
			tDescOut.ppHitObject = &m_pThrowTargetObject;
			if (pGameInstance->Raycast(tDescIn, tDescOut))
			{
				if (m_pThrowTargetObject)
				{
					m_lfTargetTimeAcc = 0.0;

					m_isTargetingOn = true;
				}
			}
		}
		else
		{
			_vector vRayPos, vRayDir;
			vRayPos = pGameInstance->Get_RayPos();
			vRayDir = XMVector3Normalize(pGameInstance->Get_RayDir());
			//const _tchar* pLayerTag;

			CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
			CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
			XMStoreFloat3(&tDescIn.vOrigin, vRayPos);
			XMStoreFloat3(&tDescIn.vUnitDir, vRayDir);
			tDescIn.fDistance = 50.f;
			tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC;
			tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_MONSTER) | (1 << (_uint)LayerType::LAYERTYPE_PLATINGPLATE);
			tDescOut.ppHitObject = &m_pThrowTargetObject;
			if (!pGameInstance->Raycast(tDescIn, tDescOut))
			{
				m_lfTargetTimeAcc += TimeDelta;

				if (m_lfTargetTimeAcc > m_lfTargetTimeMax)
				{
					if (m_pThrowTargetObject)
					{
						m_pThrowTargetObject = nullptr;
						m_isTargetingOn = false;
						m_lfTargetTimeAcc = 0.0;
					}
				}
			}
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer_AnimState::Grab_RayCasting()
{
	if (nullptr != m_pHitObject && true == m_bGrabAttack)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		if (m_isTargetingOn)
			OutputDebugStringW(TEXT("Output_TargetOn\n"));
		else
			OutputDebugStringW(TEXT("Output_TargetOff\n"));

		if (m_isTargetingOn)
		{
			_vector vDir1 = ((CTransform*)m_pThrowTargetObject->Get_Components(TEXT("Com_Transform")))->Get_Position() - ((CTransform*)m_pHitObject->Get_Components(TEXT("Com_Transform")))->Get_Position();
			m_pHitObject->Setting_Throwing(XMVector3Normalize(vDir1));
			m_pHitObject->Set_IsPicking(false);
			m_bGrabSignalCheck = false;

			m_pHitObject = nullptr;
		}
		else
		{
			_vector vDir1 = pGameInstance->Get_CamPosition() - ((CTransform*)m_pHitObject->Get_Components(TEXT("Com_Transform")))->Get_Position();
			_vector vDir2 = XMVector3Normalize(pGameInstance->Get_RayDir())*30.f;
			m_pHitObject->Setting_Throwing(XMVector3Normalize(vDir1 + vDir2));
			m_pHitObject->Set_IsPicking(false);
			m_bGrabSignalCheck = false;
			m_pHitObject = nullptr;
		}

		if (true == m_bGrabBack)
			m_bGrabBack = false;

		if (true == m_bGrabAttack)
			m_bGrabAttack = false;

		if (m_pThrowTargetObject)
		{
			m_pThrowTargetObject = nullptr;
			m_isTargetingOn = false;
		}

		Safe_Release(pGameInstance);
	}
}

_bool CPlayer_AnimState::Cloud_RayCast_Ready(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == m_pCloudHitObject)
	{
		_vector vRayPos, vRayDir;
		vRayPos = pGameInstance->Get_RayPos();
		vRayDir = XMVector3Normalize(pGameInstance->Get_RayDir());

		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		XMStoreFloat3(&tDescIn.vOrigin, vRayPos);
		XMStoreFloat3(&tDescIn.vUnitDir, vRayDir);
		tDescIn.fDistance = 50.f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_CLOUD);
		tDescOut.ppHitObject = &m_pCloudHitObject;
		
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (m_pCloudHitObject /*&& false == static_cast<CCloud*>(m_pCloudHitObject)->Get_PlayerCastCheck()*/)
			{
				//static_cast<CCloud*>(m_pCloudHitObject)->Set_PlayerCastCheck(true);
				RELEASE_INSTANCE(CGameInstance);
				return true;
			}
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}

	}
	RELEASE_INSTANCE(CGameInstance);
	return false;
}

void CPlayer_AnimState::Set_Position_RightHand(_double TimeDelta)
{
	if (nullptr != m_pHitObject)
	{
		_vector HitObjectPos = ((CTransform*)m_pHitObject->Get_Components(TEXT("Com_Transform")))->Get_Position();

		CTransform* pRightHandTransform = (CTransform*)m_pRightHand->Get_Components(TEXT("Com_Transform"));
		if (nullptr == pRightHandTransform)
		{
			MSGBOX("Failed to Get_Component : Funtion : CPlayer_IdleAnim::Set_Position_RightHand ");
			return;
		}

		pRightHandTransform->Set_State(CTransform::STATE_POSITION, HitObjectPos);
		m_pRightHand->Set_ChangeAnimCheck(true);
		m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB);
	}
}
CPlayer_AnimState * CPlayer_AnimState::Create(CPlayer * pPlayer, CTransform * pPlayerTrasnform, CModel * pPlayerModel, CSphereCollider * pPlayerColider, CNavigation * pPlayerNavigation, CCharacterController* pCharacterController, CPlayer_LeftHand* pLeftHand, CPlayer_RightHand* pRightHand, CPlayerTrigger_Parkour* pTriggerParkour, _uint iMyLevel)
{
	CPlayer_AnimState* pInstance = new CPlayer_AnimState;
	if (FAILED(pInstance->NativeConstruct(pPlayer, pPlayerTrasnform, pPlayerModel, pPlayerColider, pPlayerNavigation, pCharacterController, pLeftHand, pRightHand, pTriggerParkour, iMyLevel)))
	{
		MSGBOX("Failed To Create PlayerAnim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_AnimState::Free()
{
	CPlayer_IdleAnim::Destroy_Instance();
	CPlayer_RunAnim::Destroy_Instance();
	CPlayer_StopAnim::Destroy_Instance();
	CPlayer_ComboAttack::Destroy_Instance();
	CPlayer_Pisblast::Destroy_Instance();
	CPlayer_Dash::Destroy_Instance();
	CPlayer_Dash_Attack::Destroy_Instance();
	CPlayer_Ball::Destroy_Instance();
	CPlayer_GrabAttack::Destroy_Instance();
	CPlayer_GrabBack::Destroy_Instance();
	CPlayer_Jump::Destroy_Instance();
	CPlayer_Walk::Destroy_Instance();
	CPlayer_WalkStop::Destroy_Instance();
	CPlayer_Floting::Destroy_Instance();
	CPlayer_Falling::Destroy_Instance();
	CPlayer_Landing::Destroy_Instance();
	CPlayer_Slider::Destroy_Instance();
	CPlayer_Dodge::Destroy_Instance();
	CPlayer_BrainStom::Destroy_Instance();
	CPlayer_DownAttack::Destroy_Instance();
	CPlayer_Hit::Destroy_Instance();
	CPlayer_BrainWashing::Destroy_Instance();
	CPlayer_Ledge::Destroy_Instance();
	CPlayer_LedgeJump::Destroy_Instance();
	CPlayer_LedgeMove::Destroy_Instance();
	CPlayer_TrampolineJump::Destroy_Instance();
	CPlayer_GroundSlide::Destroy_Instance();
	CPlayer_RailSlide::Destroy_Instance();
	CPlayer_PullSwitch::Destroy_Instance();
	CPlayer_PullSwitch_Move::Destroy_Instance();
	CPlayer_Bouncer::Destroy_Instance();
	CPlayer_Blender_Climbing::Destroy_Instance();
	CPlayer_Blender_Climbing_Jump::Destroy_Instance();
	CPlayer_Blender_Climbing_JumpEnd::Destroy_Instance();
	CPlayer_Dead::Destroy_Instance();
	CPlayer_KitchenButton::Destroy_Instance();
	CPlayer_CloudAnim::Destroy_Instance();
	CPlayer_CloudAnim_End::Destroy_Instance();
	CPlayer_KnockBack::Destroy_Instance();
	CPlayer_Dish_Throw::Destroy_Instance();
	CPlayer_Food_Throw::Destroy_Instance();
	CPlayer_Throw_Left::Destroy_Instance();
	CPlayer_Throw_Right::Destroy_Instance();
}

