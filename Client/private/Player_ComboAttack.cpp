#include "stdafx.h"
#include "..\public\Player_ComboAttack.h"
#include "Player_IdleAnim.h"
#include "Player_LeftHand.h"
#include "Player_RunAnim.h"

IMPLEMENT_SINGLETON(CPlayer_ComboAttack)

CPlayer_ComboAttack::CPlayer_ComboAttack()
{
}

void CPlayer_ComboAttack::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_lfAnimSpeed = 1.0;
		m_bFirstCheck = true;
	}

	SetUp_RadianBlur(12);
	m_bAttackCheck = true;


	if (VK_LBUTTON == pAnimKeyDesc.iOnceKey)
	{
		if (false == m_bComboOne)
		{
			if (false == m_pLeftHand->Get_RenderCheck())
			{
				m_pLeftHand->Set_ChangeAnimCheck(true);
				m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_CHOPA);
			}
			m_iUpperIndex = PLAYER_ATTACK_COMBO_ONE;
			m_iRootIndex = PLAYER_ATTACK_COMBO_ONE;
			m_iBottomIndex = PLAYER_ATTACK_COMBO_ONE;
			m_bComboOne = true;

			//첫번째 공격//
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopA"));
			if (0 == iSize)
			{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ChopA"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopA"))))
					return;
			}

			if (pGameInstance->StopSound(CHANNELID::PLAYER_CHOP_A))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("ChopA.wav"), CHANNELID::PLAYER_CHOP_A)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (true == m_bComboOne && false == m_bComboTwo)
		{
			if (5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_ONE) && 10 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_ONE))
			{
				if (false == m_bComboTwo)
				{
					m_bComboTwo = true;
				}
			}
		}
		else if (true == m_bComboTwo && false == m_bComboThree)
		{
			if (5 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_TWO) && 10 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_TWO))
			{
				if (false == m_bComboThree)
				{
					m_bComboThree = true;
				}
			}
		}
	}


	if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_ATTACK_COMBO_ONE) && 0 == m_iAttackCount)
	{
		if (true == m_bComboTwo)
		{

			if (false == m_bRightHandSetCheck)
			{
				m_pRightHand->Set_ChangeAnimCheck(true);
				m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_CHOPB);
				m_bRightHandSetCheck = true;
			}

			++m_iAttackCount;
			m_iUpperIndex = PLAYER_ATTACK_COMBO_TWO;
			m_iRootIndex = PLAYER_ATTACK_COMBO_TWO;
			m_iBottomIndex = PLAYER_ATTACK_COMBO_TWO;

			//두번째 공격//
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ChopB"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopB"))))
				return;

			if (pGameInstance->StopSound(CHANNELID::PLAYER_CHOP_B))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("ChopB.wav"), CHANNELID::PLAYER_CHOP_B)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			Reset_Anim(pPlayer_AnimState);
			ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}
	else if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_ATTACK_COMBO_TWO) && 1 == m_iAttackCount)
	{
		if (true == m_bComboThree)
		{
			m_bActiveRadianBlur = true;
			if (false == m_bLeftHandSetCheck)
			{
				m_pLeftHand->Set_ChangeAnimCheck(true);
				m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_CHOPC);
				m_bLeftHandSetCheck = true;
			}
			++m_iAttackCount;
			m_iUpperIndex = PLAYER_ATTACK_COMBO_THREE;
			m_iRootIndex = PLAYER_ATTACK_COMBO_THREE;
			m_iBottomIndex = PLAYER_ATTACK_COMBO_THREE;

			//세번째 공격//
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ChopC"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopC"))))
				return;

			/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCCharge3"),
				SCENE_STATIC, TEXT("Effect_ChopCCharge3"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, false, m_pTransform, TEXT("Layer_ChopCCharge3"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCCharge3"), TEXT("Com_Option"), 2.f);
			_float3 vPos = _float3(0.f, 0.7f, 0.f);
			pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCCharge3"), TEXT("Com_Option"), XMLoadFloat3(&vPos));

			if (pGameInstance->StopSound(CHANNELID::PLAYER_CHOP_C))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("DawnAttack.wav"), CHANNELID::PLAYER_CHOP_C)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			Reset_Anim(pPlayer_AnimState);
			ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
			if (nullptr == pRenderer)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}
	else if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_ATTACK_COMBO_THREE) && 2 == m_iAttackCount)
	{
		Reset_Anim(pPlayer_AnimState);
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		if (nullptr == pRenderer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_ComboAttack::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_ATTACK_COMBO_ONE == m_iUpperIndex)
	{
		if (0 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_ONE) && 3 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_ONE))
		{
			m_pTransform->Go_Straight(TimeDelta * 1.3f * pPlayer_AnimState->Get_AttackMoveSpeed(), m_pCharacterController);
		}
	}
	else if (PLAYER_ATTACK_COMBO_TWO == m_iUpperIndex)
	{
		if (2 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_TWO) && 4 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_TWO))
		{
			m_pTransform->Go_Straight(TimeDelta * 1.5f * pPlayer_AnimState->Get_AttackMoveSpeed(), m_pCharacterController);
		}
	}
	else if (PLAYER_ATTACK_COMBO_THREE == m_iUpperIndex)
	{
		if (2 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_THREE) && 6 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_ATTACK_COMBO_THREE))
		{
			m_pTransform->Go_Straight(TimeDelta * 1.6f * pPlayer_AnimState->Get_AttackMoveSpeed(), m_pCharacterController);
		}
	}


	m_pModel->Upper_Update(TimeDelta * 1.3 * m_lfAnimSpeed);
	m_pModel->Root_Update(TimeDelta * 1.3 * m_lfAnimSpeed);
	m_pModel->Bottom_Update(TimeDelta * 1.3 * m_lfAnimSpeed);
}

void CPlayer_ComboAttack::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iAttackCount = 0;
	
	m_fMoveOffset = 0.f;
	
	m_lfAnimSpeed = 1.0;
	
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bFirstCheck = false;

	m_bComboOne = false;
	m_bComboTwo = false;
	m_bComboThree = false;

	m_bRightHandSetCheck = false;
	m_bLeftHandSetCheck = false;
}

void CPlayer_ComboAttack::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iAttackCount = 0;

	m_fMoveOffset = 0.f;

	m_lfAnimSpeed = 1.0;

	m_bFirstCheck = false;

	m_bComboOne = false;
	m_bComboTwo = false;
	m_bComboThree = false;

	m_bRightHandSetCheck = false;
	m_bLeftHandSetCheck = false;
}

void CPlayer_ComboAttack::SetUp_RadianBlur(_uint iMaxCount)
{
	if (true == m_bActiveRadianBlur)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		if (nullptr == pRenderer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, true);
		RELEASE_INSTANCE(CGameInstance);

		++m_iRadianCount;
		if (iMaxCount <= m_iRadianCount)
		{
			m_bActiveRadianBlur = false;
			m_iRadianCount = 0;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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



void CPlayer_ComboAttack::Free()
{
}
