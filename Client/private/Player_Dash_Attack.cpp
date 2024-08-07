#include "stdafx.h"
#include "..\public\Player_Dash_Attack.h"
#include "Player_IdleAnim.h"

IMPLEMENT_SINGLETON(CPlayer_Dash_Attack)

CPlayer_Dash_Attack::CPlayer_Dash_Attack()
{
}

void CPlayer_Dash_Attack::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iUpperIndex = PLAYER_TUMBLE_ATTACK_START;
		m_iRootIndex = PLAYER_TUMBLE_ATTACK_START;
		m_iBottomIndex = PLAYER_TUMBLE_ATTACK_START;
		++m_iDashAttackCount;
		m_bFirstCheck = true;
	}
	else if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TUMBLE_ATTACK_START) && 1 == m_iDashAttackCount)
	{
		++m_iDashAttackCount;
		m_iUpperIndex = PLAYER_TUMBLE_ATTACK_LOOP;
		m_iRootIndex = PLAYER_TUMBLE_ATTACK_LOOP;
		m_iBottomIndex = PLAYER_TUMBLE_ATTACK_LOOP;


		//점프해서 슈웅하는부분

	}
	else if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TUMBLE_ATTACK_LOOP) && 2 == m_iDashAttackCount)
	{
		++m_iDashAttackCount;
		m_iUpperIndex = PLAYER_TUMBLE_ATTACK_END;
		m_iRootIndex = PLAYER_TUMBLE_ATTACK_END;
		m_iBottomIndex = PLAYER_TUMBLE_ATTACK_END;

		//첫모션 한바퀴 도는끝에지점의 쿵
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_TumbleAttack"));
		if (0 == iSize)
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_TumbleAttack"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_TumbleAttack"), &m_pTransform->Get_WorldMatrix())))
				return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("DashAttack.wav"), CHANNELID::PLAYER_DASHATTACKSOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	if (PLAYER_TUMBLE_ATTACK_END == m_iUpperIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_TUMBLE_ATTACK_END))
	{
		Reset_Anim(pPlayer_AnimState);
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}


	Colider_Switch(pPlayer_AnimState, TimeDelta);

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

void CPlayer_Dash_Attack::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_TUMBLE_ATTACK_START == m_iRootIndex)
	{
		if (0 <= m_pModel->Get_BottomAnimationCurrentFrame(PLAYER_TUMBLE_ATTACK_START) && 3 >= m_pModel->Get_BottomAnimationCurrentFrame(PLAYER_TUMBLE_ATTACK_START))
			m_pTransform->Go_Straight(TimeDelta * 1.3, m_pCharacterController);
	}
	else if (PLAYER_TUMBLE_ATTACK_LOOP == m_iRootIndex)
		m_pTransform->Go_Straight(TimeDelta * pPlayer_AnimState->Get_DashAttackMoveSpeed(), m_pCharacterController);
	else if (PLAYER_TUMBLE_ATTACK_END == m_iRootIndex)
	{
		if (0 == m_pModel->Get_BottomAnimationCurrentFrame(PLAYER_TUMBLE_ATTACK_END))
			m_pTransform->Go_Straight(TimeDelta * 0.5 * pPlayer_AnimState->Get_DashAttackMoveSpeed(), m_pCharacterController);
		else if (1 == m_pModel->Get_BottomAnimationCurrentFrame(PLAYER_TUMBLE_ATTACK_END))
			m_pTransform->Go_Straight(TimeDelta * 0.5 * pPlayer_AnimState->Get_DashAttackMoveSpeed(), m_pCharacterController);
	}


	m_pModel->Upper_Update(TimeDelta * 1.8);
	m_pModel->Root_Update(TimeDelta * 1.8);
	m_pModel->Bottom_Update(TimeDelta * 1.8);
}

void CPlayer_Dash_Attack::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_iDashAttackCount = 0;
	pPlayer_AnimState->Set_Coliderenable(false);
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Dash_Attack::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_iDashAttackCount = 0;
	pPlayer_AnimState->Set_Coliderenable(false);
}

void CPlayer_Dash_Attack::Colider_Switch(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (PLAYER_TUMBLE_ATTACK_END == m_iRootIndex)
		pPlayer_AnimState->Set_Coliderenable(true);
}


void CPlayer_Dash_Attack::Free()
{
}

