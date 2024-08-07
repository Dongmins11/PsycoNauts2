#include "stdafx.h"
#include "..\public\Player_Dash.h"
#include "Player_Dash_Attack.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"

IMPLEMENT_SINGLETON(CPlayer_Dash)

CPlayer_Dash::CPlayer_Dash()
{
}

void CPlayer_Dash::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		if (pGameInstance->StopSound(CHANNELID::PLAYER_DASHSOUND))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
			

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Dash.wav"), CHANNELID::PLAYER_DASHSOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_bFirstCheck = true;
	}


	if (10 >= m_pModel->Get_AnimationCurrentFrame(PLAYER_DASH))
	{
		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstace->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, true);

		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstace->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);

		RELEASE_INSTANCE(CGameInstance);
	}



	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if (PLAYER_DASH == m_pModel->Get_AnimIndex())
		{
			if (pAnimKeyDesc.iKey[i] == VK_LBUTTON && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN && false == m_bDashAttackCheck)
			{
				m_bDashAttackCheck = true;
			}
		}
	}

	m_iAnimIndex = PLAYER_DASH;


	if (true == m_bDashAttackCheck)
	{
		if (11 <= m_pModel->Get_AnimationCurrentFrame(PLAYER_DASH))
		{
			m_bFirstCheck = false;
			Reset_Anim(pPlayer_AnimState);
			ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
			pPlayer_AnimState->SetUp_State(CPlayer_Dash_Attack::Get_Instance());
			CPlayer_Dash_Attack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

			CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstace->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
			pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);

			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}
	if (30 <= m_pModel->Get_AnimationCurrentFrame(PLAYER_DASH))
	{
		m_pModel->Set_ResetAnimation(PLAYER_DASH);
		m_bFirstCheck = false;
		m_bFirstDashCheck = false;
		m_bDashAttackCheck = false;
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->Walk_to_Run_SettingCheck(true);
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	m_pModel->Set_Animation(m_iAnimIndex);
}

void CPlayer_Dash::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (0 <= m_pModel->Get_AnimationCurrentFrame(PLAYER_DASH) && 12 >= m_pModel->Get_AnimationCurrentFrame(PLAYER_DASH))
		m_pTransform->Go_Straight(TimeDelta * 1.2, m_pCharacterController);

	m_pModel->Lerp_Update(TimeDelta* 1.2);

	m_bFirstDashCheck = true;
}

void CPlayer_Dash::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_bFirstDashCheck = false;
	m_bDashAttackCheck = false;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Dash::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_bFirstDashCheck = false;
	m_bDashAttackCheck = false;
	m_pModel->Set_ResetAnimation(PLAYER_DASH);
	CPlayer_IdleAnim::Get_Instance()->Walk_to_Run_SettingCheck(true);
}

void CPlayer_Dash::Free()
{
}

