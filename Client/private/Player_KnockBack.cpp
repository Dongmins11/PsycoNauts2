#include "stdafx.h"
#include "..\public\Player_KnockBack.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"

IMPLEMENT_SINGLETON(CPlayer_KnockBack)

CPlayer_KnockBack::CPlayer_KnockBack()
{
}

void CPlayer_KnockBack::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;

	First_KNockBackMotionCheck(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	End_to_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_KnockBack::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if(!m_pCharacterController->IsDown())
		m_pTransform->Add_Velocity(CTransform::STATE_UP, (_float)-(TimeDelta * 9.8));

	m_fMoveOffset -= (_float)(TimeDelta * 2.0);
	if (0.f >= m_fMoveOffset)
		m_fMoveOffset = 0.f;

	m_pTransform->Go_Backward(TimeDelta * m_lfMoveSpeed * (_double)(m_fMoveOffset), m_pCharacterController);

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_KnockBack::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_lfMoveSpeed = 1.3;

	m_fMoveOffset = 1.f;

	m_bFirstCheck = false;

	pPlayer_AnimState->Set_AllLerpCheckTrue();

	m_pPlayer->Set_IsKnockBack(false);
	m_pPlayer->Set_RimLightEnable(false);
	m_pPlayer->Set_RimPower(3.f);
	m_pPlayer->Set_RGBA(_float4(1.f, 1.f, 1.f, 1.f));
}


void CPlayer_KnockBack::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_lfMoveSpeed = 1.3;

	m_fMoveOffset = 1.f;

	m_bFirstCheck = false;

	m_pPlayer->Set_RimLightEnable(false);
	m_pPlayer->Set_RimPower(3.f);
	m_pPlayer->Set_RGBA(_float4(1.f, 1.f, 1.f, 1.f));
}

void CPlayer_KnockBack::First_KNockBackMotionCheck(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_HIT_KNOCKBACK_LOOP, 6.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_HIT_KNOCKBACK_LOOP, 6.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_HIT_KNOCKBACK_LOOP, 6.f);

		m_iUpperIndex = PLAYER_HIT_KNOCKBACK_LOOP;
		m_iBottomIndex = PLAYER_HIT_KNOCKBACK_LOOP;
		m_iRootIndex = PLAYER_HIT_KNOCKBACK_LOOP;

		m_lfMoveSpeed = 1.3;

		m_fMoveOffset = 1.f;

		m_bFirstCheck = true;

		m_pPlayer->Set_RimLightEnable(true);
		m_pPlayer->Set_RimPower(0.5f);
		m_pPlayer->Set_RGBA(_float4(1.f, 0.f, 0.f, 1.f));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		if (nullptr == pRenderer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PlayerKnockBack.wav"), CHANNELID::PLAYER_KNOCKBACK)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

_bool CPlayer_KnockBack::Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	return false;
}

_bool CPlayer_KnockBack::End_to_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (0.f >= m_fMoveOffset)
	{
		m_pPlayer->Set_RimPower(3.f);
		m_pPlayer->Set_RGBA(_float4(1.f, 1.f, 1.f, 1.f));

		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

void CPlayer_KnockBack::Free()
{
}

