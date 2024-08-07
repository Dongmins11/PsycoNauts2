#include "stdafx.h"
#include "..\public\Player_Slider.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Dodge.h"
#include "Player_Falling.h"

IMPLEMENT_SINGLETON(CPlayer_Slider)

CPlayer_Slider::CPlayer_Slider()
{
}

void CPlayer_Slider::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_Check_AnimMotion(pPlayer_AnimState, TimeDelta);

	Slide_Loop_MotionChange(pPlayer_AnimState, TimeDelta);

 	pPlayer_AnimState->PhysX_Slider();

	if (true == m_bSlideCheck)
	{
		for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
		{
			if (LOOK_LEFT == m_eCurrentLookState || LOOK_FRONT == m_eCurrentLookState)
			{
				if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"));
					if (0 < iSize)
					{
						if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), 0)))
						{
							RELEASE_INSTANCE(CGameInstance);
							return;
						}
					}
					
					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DonutRing"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DodgeJumpEffect"), &m_pTransform->Get_WorldMatrix())))
						return;

					/* For. Layer_DoubleJumpEffect 积己 */
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DodgeJumpEffect"),
						SCENE_STATIC, TEXT("Effect_DoubleJump"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					/* For. Layer_DoubleJumpEffect 惑加 */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_DodgeJumpEffect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}

					RELEASE_INSTANCE(CGameInstance);
					Reset_Anim(pPlayer_AnimState);
					CPlayer_Dodge::Get_Instance()->Set_LookState(LOOK_LEFT);
					pPlayer_AnimState->SetUp_State(CPlayer_Dodge::Get_Instance());
					CPlayer_Dodge::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
					return;
				}
			}
			else if (LOOK_RIGHT == m_eCurrentLookState || LOOK_BACK == m_eCurrentLookState)
			{
				if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"));
					if (0 < iSize)
					{
						if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), 0)))
						{
							RELEASE_INSTANCE(CGameInstance);
							return;
						}
					}

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DonutRing"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DodgeJumpEffect"), &m_pTransform->Get_WorldMatrix())))
						return;

					/* For. Layer_DoubleJumpEffect 积己 */
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DodgeJumpEffect"),
						SCENE_STATIC, TEXT("Effect_DoubleJump"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					/* For. Layer_DoubleJumpEffect 惑加 */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransform, TEXT("Layer_DodgeJumpEffect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}

					RELEASE_INSTANCE(CGameInstance);
					Reset_Anim(pPlayer_AnimState);
					CPlayer_Dodge::Get_Instance()->Set_LookState(LOOK_RIGHT);
					pPlayer_AnimState->SetUp_State(CPlayer_Dodge::Get_Instance());
					CPlayer_Dodge::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
					return;
				}
			}
			
		}

	}

	if (m_pCharacterController->IsDown())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"));
		if (0 < iSize)
		{
			if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), 0)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Slider::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

void CPlayer_Slider::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_eCurrentLookState = LOOK_END;
	m_bSlideCheck = false;
	m_bFirstCheck = false;
	pPlayer_AnimState->Set_Slider(false);
	pPlayer_AnimState->Set_FloatingSpeed(1.f);
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Slider::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_eCurrentLookState = LOOK_END;
	m_bSlideCheck = false;
	m_bFirstCheck = false;
	pPlayer_AnimState->Set_Slider(false);
	pPlayer_AnimState->Set_FloatingSpeed(1.f);
	pPlayer_AnimState->Set_AllLerpCheckTrue();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"));
	if (0 < iSize)
	{
		if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), 0)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_Slider::First_Check_AnimMotion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"));
		EFFECT_TYPE	eType;
		if (LOOK_STATE::LOOK_LEFT == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_SLIDER_LEFT;
			m_iRootIndex = PLAYER_SLIDER_LEFT;
			m_iBottomIndex = PLAYER_SLIDER_LEFT;

			if (0 == iSize)
			{
				eType = TYPE_LEFT;
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_SlidingMesh"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), &eType)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
		}
		else if (LOOK_STATE::LOOK_RIGHT == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_SLIDER_RIGHT;
			m_iRootIndex = PLAYER_SLIDER_RIGHT;
			m_iBottomIndex = PLAYER_SLIDER_RIGHT;

			if (0 == iSize)
			{
				eType = TYPE_RIGHT;
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_SlidingMesh"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), &eType)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
		}
		else if (LOOK_STATE::LOOK_FRONT == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_SLIDER_LEFT;
			m_iRootIndex = PLAYER_SLIDER_LEFT;
			m_iBottomIndex = PLAYER_SLIDER_LEFT;

			if (0 == iSize)
			{
				eType = TYPE_LEFT;
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_SlidingMesh"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), &eType)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
		}
		else if (LOOK_STATE::LOOK_BACK == m_eCurrentLookState)
		{
			m_iUpperIndex = PLAYER_SLIDER_RIGHT ;
			m_iRootIndex = PLAYER_SLIDER_RIGHT;
			m_iBottomIndex = PLAYER_SLIDER_RIGHT;

			if (0 == iSize)
			{
				eType = TYPE_RIGHT;
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_SlidingMesh"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SlidingMesh"), &eType)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);

		pPlayer_AnimState->Set_FloatingSpeed(0.05f);
		
		m_pModel->Set_UpperAnimSpeed(PLAYER_SLIDER_LEFT, 1.5f);
		m_pModel->Set_RootAnimSpeed(PLAYER_SLIDER_LEFT, 1.5f);
		m_pModel->Set_BottomAnimSpeed(PLAYER_SLIDER_LEFT, 1.5f);

		m_pModel->Set_UpperAnimSpeed(PLAYER_SLIDER_RIGHT, 1.5f);
		m_pModel->Set_RootAnimSpeed(PLAYER_SLIDER_RIGHT, 1.5f);
		m_pModel->Set_BottomAnimSpeed(PLAYER_SLIDER_RIGHT, 1.5f);

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_SLIDER_LEFT, 7.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_SLIDER_LEFT, 7.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_SLIDER_LEFT, 7.f);

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_SLIDER_RIGHT, 7.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_SLIDER_RIGHT, 7.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_SLIDER_RIGHT, 7.f);


		m_bFirstCheck = true;
	}



}

void CPlayer_Slider::Slide_Loop_MotionChange(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (m_iRootIndex == PLAYER_SLIDER_LEFT && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_SLIDER_LEFT))
	{
		m_iUpperIndex = PLAYER_SLIDER_LOOP_LEFT;
		m_iRootIndex = PLAYER_SLIDER_LOOP_LEFT;
		m_iBottomIndex = PLAYER_SLIDER_LOOP_LEFT;

		m_bSlideCheck = true;
	}
	else if (m_iRootIndex == PLAYER_SLIDER_RIGHT && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_SLIDER_RIGHT))
	{
		m_iUpperIndex = PLAYER_SLIDER_LOOP_RIGHT;
		m_iRootIndex = PLAYER_SLIDER_LOOP_RIGHT;
		m_iBottomIndex = PLAYER_SLIDER_LOOP_RIGHT;

		m_bSlideCheck = true;
	}


}

void CPlayer_Slider::Free()
{
}

