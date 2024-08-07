#include "stdafx.h"
#include "..\public\Player_Landing.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"

IMPLEMENT_SINGLETON(CPlayer_Landing)

CPlayer_Landing::CPlayer_Landing()
{
}

void CPlayer_Landing::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_Effect_Create();

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_bFirstCheck = false;
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}

	m_iUpperIndex = PALYER_LANDING;
	m_iRootIndex = PALYER_LANDING;
	m_iBottomIndex = PALYER_LANDING;

	if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
	{
		m_bFirstCheck = false;
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Landing::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_Landing::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Landing::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
}

void CPlayer_Landing::First_Effect_Create()
{
	if (false == m_bFirstCheck)
	{
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


		m_bFirstCheck = true;
	}

}

void CPlayer_Landing::Free()
{
}

