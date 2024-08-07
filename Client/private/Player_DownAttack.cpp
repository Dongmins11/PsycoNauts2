#include "stdafx.h"
#include "..\public\Player_DownAttack.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"

IMPLEMENT_SINGLETON(CPlayer_DownAttack)

CPlayer_DownAttack::CPlayer_DownAttack()
{
}

void CPlayer_DownAttack::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;

	First_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Start_Motion_Move(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Loop_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	JumpOffset_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	JumpOffset_Falling(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	JumpOffset_Land(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	KeyCheck_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Colider_Switch(pPlayer_AnimState, TimeDelta);

	if (true == Land_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Land_to_Run(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;



	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_DownAttack::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_iRootIndex == PLAYER_DOWN_ATTACK_START)
	{
		m_pModel->Upper_Update(TimeDelta * 2.0);
		m_pModel->Root_Update(TimeDelta* 2.0);
		m_pModel->Bottom_Update(TimeDelta* 2.0);
	}
	else
	{
		m_pModel->Upper_Update(TimeDelta * 1.5);
		m_pModel->Root_Update(TimeDelta* 1.5);
		m_pModel->Bottom_Update(TimeDelta* 1.5);
	}
}

void CPlayer_DownAttack::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;


	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.8);
	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_DoubleJumpCheck(false);
	pPlayer_AnimState->Set_DownAttackCheck(false);
	pPlayer_AnimState->Set_Coliderenable(false);

	m_pModel->Set_UpperAnimSpeed(PLAYER_DOWN_ATTACK_END, 1.f);
	m_pModel->Set_RootAnimSpeed(PLAYER_DOWN_ATTACK_END, 1.f);
	m_pModel->Set_BottomAnimSpeed(PLAYER_DOWN_ATTACK_END, 1.f);

	m_bDownAttackGravityCheck = false;
	m_bMoveCheck = false;
	m_bIsJump = false;
	m_bIsDownAttack = false;
	m_bKeyCheck = false;
}

void CPlayer_DownAttack::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);
}

void CPlayer_DownAttack::Set_Reset_Jump(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(0.8);
	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
}

void CPlayer_DownAttack::Colider_Switch(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (PLAYER_DOWN_ATTACK_START == m_iRootIndex || PLAYER_DOWN_ATTACK_LOOP == m_iRootIndex)
		pPlayer_AnimState->Set_Coliderenable(true);
	else
		pPlayer_AnimState->Set_Coliderenable(false);
}

void CPlayer_DownAttack::First_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bKeyCheck)
	{

		m_iUpperIndex = PLAYER_DOWN_ATTACK_START;
		m_iRootIndex = PLAYER_DOWN_ATTACK_START;
		m_iBottomIndex = PLAYER_DOWN_ATTACK_START;

		//pPlayer_AnimState->Set_JumpPower(9.f);
		//pPlayer_AnimState->Set_LandCheck(false);
		//pPlayer_AnimState->Set_FallCheck(false);
		//pPlayer_AnimState->Set_DownAttackCheck(true);

		m_bIsDownAttack = true;
		m_bKeyCheck = true;
		//첫번째 다운어택모션생성위치//

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ChargeSingleWave"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChargeSingleWave"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransform, TEXT("Layer_ChargeSingleWave"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		if (nullptr == pRenderer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, true);


		if (pGameInstance->StopSound(CHANNELID::PLAYER_DOWNATT))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("DawnAttack.wav"), CHANNELID::PLAYER_DOWNATT)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer_DownAttack::Start_Motion_Move(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_iRootIndex == PLAYER_DOWN_ATTACK_START && false == m_bMoveCheck)
	{
		if (6 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_DOWN_ATTACK_START))
			m_pTransform->Add_Velocity(CTransform::STATE_UP, 0.001f);
		else if (6 < m_pModel->Get_RootAnimationCurrentFrame(PLAYER_DOWN_ATTACK_START) || true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOWN_ATTACK_START))
			m_bMoveCheck = true;
	}

	if (true == m_bMoveCheck && false == m_bDownAttackGravityCheck)
	{
		if (!m_pCharacterController->IsDown())
			m_pTransform->Add_Velocity(CTransform::STATE_UP, (_float)-(TimeDelta * 15.0));
		else
			m_bDownAttackGravityCheck = true;
	}
}

void CPlayer_DownAttack::Loop_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsDownAttack)
	{
		if (PLAYER_DOWN_ATTACK_START == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOWN_ATTACK_START))
		{
			m_iUpperIndex = PLAYER_DOWN_ATTACK_LOOP;
			m_iRootIndex = PLAYER_DOWN_ATTACK_LOOP;
			m_iBottomIndex = PLAYER_DOWN_ATTACK_LOOP;
		}
	}

}

void CPlayer_DownAttack::JumpOffset_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bIsJump && true == m_bIsDownAttack)
	{
		if (true == m_bDownAttackGravityCheck)
		{
			m_iUpperIndex = PLAYER_DOWN_ATTACK_END;
			m_iRootIndex = PLAYER_DOWN_ATTACK_END;
			m_iBottomIndex = PLAYER_DOWN_ATTACK_END;

			Set_Reset_Jump(pPlayer_AnimState);

			pPlayer_AnimState->Set_JumpPower(5.f);
			pPlayer_AnimState->Set_JumpCheck(true);

			m_bIsJump = true;

			////엔드위치 이펙트생성/////

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_Once"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JumpAtt3"),
				SCENE_STATIC, TEXT("Effect_JumpAtt3"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_JumpAtt3"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JumpAtt3"), TEXT("Com_Option"), 1.f);
			_float3 vPos = _float3(0.f, 0.02f, 0.f);
			pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JumpAtt3"), TEXT("Com_Option"), XMLoadFloat3(&vPos));

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_JumpAtt"),
				SCENE_STATIC, TEXT("Effect_Smoke_JumpAtt"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_Smoke_JumpAtt"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_JumpAtt"), TEXT("Com_Option"), 0.5f);

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JumpAttRact"),
				SCENE_STATIC, TEXT("Effect_JumpAttRact"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_JumpAttRact"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JumpAttRact"), TEXT("Com_Option"), 0.5f);


			CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
			if (nullptr == pRenderer)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);

			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CPlayer_DownAttack::JumpOffset_Falling(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsJump && 15 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_DOWN_ATTACK_END))
	{
		if (true == pPlayer_AnimState->Get_FallCheck() && false == pPlayer_AnimState->Get_LandCheck())
		{
			m_pModel->Set_UpperAnimSpeed(PLAYER_DOWN_ATTACK_END, 0.f);
			m_pModel->Set_RootAnimSpeed(PLAYER_DOWN_ATTACK_END, 0.f);
			m_pModel->Set_BottomAnimSpeed(PLAYER_DOWN_ATTACK_END, 0.f);
		}
	}

}

void CPlayer_DownAttack::JumpOffset_Land(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsJump)
	{
		if (true == pPlayer_AnimState->Get_LandCheck())
		{
			m_iUpperIndex = PALYER_LANDING;
			m_iRootIndex = PALYER_LANDING;
			m_iBottomIndex = PALYER_LANDING;

			pPlayer_AnimState->Set_LandCheck(false);
		}
	}


}

void CPlayer_DownAttack::KeyCheck_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool bMoveCheck = false;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bMoveCheck = true;
		}
		if (VK_NUMLOCK == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		}
	}

	m_bIsMoveOn = bMoveCheck;
}

_bool CPlayer_DownAttack::Land_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	if (PALYER_LANDING == m_iRootIndex && true == m_bIsJump)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_DownAttack::Land_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex && 3 <= m_pModel->Get_RootAnimationCurrentFrame(PALYER_LANDING) && true == m_bIsJump)
	{
		if (true == m_bIsMoveOn)
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

void CPlayer_DownAttack::Free()
{

}
