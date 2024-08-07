#include "stdafx.h"
#include "..\public\Player_RailSlide.h"
#include "Player_StopAnim.h"
#include "Player_IdleAnim.h"
#include "Player_ComboAttack.h"
#include "Player_Pisblast.h"
#include "Player_Dash.h"
#include "Player_Ball.h"
#include "Player_GrabAttack.h"
#include "Player_GrabBack.h"
#include "Player_Jump.h"
#include "Player_Falling.h"
#include "Player_BrainStom.h"
#include "Player_Hit.h"
#include "Player_GroundSlide.h"
#include "Player_Landing.h"
#include "Camera_Fly.h"

IMPLEMENT_SINGLETON(CPlayer_RailSlide)

CPlayer_RailSlide::CPlayer_RailSlide()
{
}

void CPlayer_RailSlide::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//초기세팅
	First_State(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	//키 조작
	SetUp_Movement(pPlayer_AnimState, pAnimKeyDesc);

	Separation_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	//Set_BlendSpeed(TimeDelta);

	Effect_Create(pPlayer_AnimState, TimeDelta);

	Effect_Release(pPlayer_AnimState);

	//애니메이션 변환
	if (false == RailRayCast(pPlayer_AnimState,TimeDelta))/*충돌 객체 찾기*/
	{
		if (true == ChangeAnim(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
			return;
	}
	else
	{
		if (true == Jump(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
			return;
	}
	//변수 셋팅

	SetUp_Values(pPlayer_AnimState, TimeDelta);/*변수 셋팅*/

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);

	m_BackUpPos = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
}

void CPlayer_RailSlide::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bIsMoveOn = true;
	Move_Acceleration(TimeDelta);
	Handle_Movement(pAnimKeyDesc);

	//기본 움직임

	if (true == m_bIsChangeLook) /*기본 움직임*/
		Move(TimeDelta);
	else /*점프로 탈출 했을 때*/
	{
		SetUp_Look(TimeDelta);
		m_pTransform->Go_Straight(TimeDelta *m_dMoveSpeed*m_dNonFixJumpPower, m_pCharacterController);
	}


	m_pModel->Root_Update(TimeDelta);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);

}
void CPlayer_RailSlide::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_pModel->Set_UpperAnimBlendSpeed(PALYER_LANDING, 10.f);
	m_pModel->Set_RootAnimBlendSpeed(PALYER_LANDING, 10.f);
	m_pModel->Set_BottomAnimBlendSpeed(PALYER_LANDING, 10.f);

	m_fMoveOffset = 0.f;
	m_bMotionCheck = false;
	m_bStartCheck = false;
	m_bFirstCheck = false;
	m_bIsMoveOn = false;

	m_pBackUpRailObject = nullptr;
	m_bRailChangeSign = false;
	m_bAdjustDirRight = false;
	m_bAdjustStart = false;
	m_bStartJump = false;
	m_dStartJumpAcc = 0.0;

	m_dMoveSpeed = m_dStartSpeed;
	m_dDestSpeed = m_dDefaultSpeed;
	m_bLerpSpeed = true;
	m_dLerpSpeedRatio = 0.0;
	//m_dLerpSpeed_Speed = 1.0;

	m_vDestLook = _float3(0.f, 0.f, 0.f);
	m_bLerpLook = true;
	m_dLerpLookRatio = 0.0;
	//m_dLerpLook_Speed = 10.0;

	m_vAdjustPos = _float3(0.f, 0.f, 0.f);
	m_bLerpAdjust = true;
	m_dLerpAdjustRatio = 0.0;
	//m_dLerpAdjust_Speed = 1.;
	m_vDestAdjustPos = _float3(0.f, 0.f, 0.f);


	m_iCurKey = 0;

	m_bIsJump = false;
	m_bIsNotRailLand = false;
	m_bIsChangeLook = true;

	Effect_Release(pPlayer_AnimState);
}

void CPlayer_RailSlide::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);

	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_JumpPower(6.85f);
	pPlayer_AnimState->Set_RailSlideCheck(false);
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(1.f);
	pPlayer_AnimState->Set_CollisionObject(nullptr);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value_Default(1.f);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_RailSlide::First_State(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		if (false == m_bMoveOffsetCheck)
			m_fMoveOffset = 0.3f;

		m_lfMoveSpeed = 0.8;
		m_fJumpPower = 17.5f;
		m_lfFallSpeed = 1.0;

		//m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN_START, 9.f);
		//m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN_START, 9.f);
		//m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN_START, 9.f);


		//m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN, 7.f);
		//m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN, 7.f);
		//m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN, 7.f);

		m_pBackUpRailObject = nullptr;
		m_bRailChangeSign = false;
		m_bAdjustDirRight = false;
		m_bAdjustStart = false;
		m_bStartJump = false;
		m_dStartJumpAcc = 0.0;

		m_dMoveSpeed = m_dStartSpeed;
		m_dDestSpeed = m_dDefaultSpeed;
		m_bLerpSpeed = true;
		m_dLerpSpeedRatio = 0.0;
		//m_dLerpSpeed_Speed = 1.0;

		m_vDestLook = _float3(0.f, 0.f, 0.f);
		m_bLerpLook = true;
		m_dLerpLookRatio = 0.0;
		//m_dLerpLook_Speed = 20.0;

		m_vAdjustPos = _float3(0.f, 0.f, 0.f);
		m_bLerpAdjust = true;
		m_dLerpAdjustRatio = 0.0;
		//m_dLerpAdjust_Speed = 1.;
		m_vDestAdjustPos = _float3(0.f, 0.f, 0.f);
		m_vSourAdjustPos = _float3(0.f, 0.f, 0.f);

		m_iCurKey = 0;

		m_bIsJump = false;
		m_bIsNotRailLand = false;
		m_bIsChangeLook = true;
		m_bRay_ChangeRail = false;

		m_dRayTimeAcc = 0.0;
		m_dNonFixJumpPower = 1.5f;
		m_dStartSpeed = 0.025;
		m_dDefaultSpeed = 0.6;
		m_dFastSpeed = 1.3;
		m_dSlowSpeed = 0.3;
		m_dRailCollisionCheckTerm = 2.0;
		m_dJumpCorrection = 1.7;
		m_dStartJumpAccMax = 0.1;
		m_dRayTrans = 0.08;


		m_iUpperIndex = PLAYER_RAIL_SLIDE_ENTER;
		m_iRootIndex = PLAYER_RAIL_SLIDE_ENTER;
		m_iBottomIndex = PLAYER_RAIL_SLIDE_ENTER;

		m_BackUpPos = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
		m_bFirstCheck = true;
	}
}

void CPlayer_RailSlide::Separation_Motion(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
	//{
	//	if (true == m_pModel->Get_IsUpperAnimFinishied(PLAYER_GRAP_ATTACK_START))
	//	{
	//		pPlayer_AnimState->Set_UpperLerpCheck(true);

	//		if (false == m_bMotionCheck)
	//			m_iUpperIndex = PLAYER_RUN_START;
	//		else
	//			m_iUpperIndex = PLAYER_RUN;
	//	}
	//	else
	//		m_iUpperIndex = PLAYER_GRAP_ATTACK_START;
	//}
	//else
	//{
	//	if (false == m_bMotionCheck)
	//		m_iUpperIndex = PLAYER_RUN_START;
	//	else
	//		m_iUpperIndex = PLAYER_RUN;
	//}

	if (PLAYER_RAIL_SLIDE_ENTER == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_RAIL_SLIDE_ENTER))
	{
		m_iUpperIndex = PLAYER_RAIL_SLIDE;
		m_iRootIndex = PLAYER_RAIL_SLIDE;
		m_iBottomIndex = PLAYER_RAIL_SLIDE;
	}


}

_bool CPlayer_RailSlide::Set_GravityChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_GravityCheck())
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Falling::Get_Instance());
		CPlayer_Falling::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}

	return false;
}

_bool CPlayer_RailSlide::Hit_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == pPlayer_AnimState->Get_HitCheck())
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		//pPlayer_AnimState->Set_HitCheck(false);
		pPlayer_AnimState->SetUp_State(CPlayer_Hit::Get_Instance());
		CPlayer_Hit::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

_bool CPlayer_RailSlide::Run_To_Ground_Slide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_pPlayer->Get_IsSlide())
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_GroundSlide::Get_Instance());
		CPlayer_GroundSlide::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		return true;
	}

	return false;
}

void CPlayer_RailSlide::Effect_Create(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if ((PLAYER_RAIL_SLIDE == m_iRootIndex ||
		PLAYER_RAIL_SLIDE_BACK == m_iRootIndex) && !m_isEffectCreate)
	{
		m_isEffectCreate = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_SkateEffect"),
			SCENE_STATIC, TEXT("Effect_Repeat_SkateEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransform, TEXT("Layer_Repeat_SkateEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

}

void CPlayer_RailSlide::Effect_Release(CPlayer_AnimState * pPlayer_AnimState)
{
	if (true == m_isEffectCreate)
	{
		if (true == pPlayer_AnimState->Get_JumpCheck() || true == pPlayer_AnimState->Get_DoubleJumpCheck()
			|| (false == m_bIsJump && true == pPlayer_AnimState->Get_GravityCheck()))
		{
			//지워준다~

			m_isEffectCreate = false;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_SkateEffect"));

			if (0 < iSize)
			{
				if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_SkateEffect"), 0)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

_bool CPlayer_RailSlide::ChangeAnim(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == Fall(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return true;

	if (true == Jump(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return true;

	return false;
}

_bool CPlayer_RailSlide::Fall(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//점프안하고 레일 끝나서 떨어질 때 
	if (false == m_bIsJump && true == pPlayer_AnimState->Get_GravityCheck())
	{
		Reset_Anim(pPlayer_AnimState);

		pPlayer_AnimState->Set_RailSlideCheck(false);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->Set_JumpPower(6.85f);
		pPlayer_AnimState->Set_Fall_AccelerationSpeed(1.f);
		pPlayer_AnimState->Set_FallSpeed(1.f);
		pPlayer_AnimState->Set_BackUpY(0.f);
		pPlayer_AnimState->Set_JumpTimeAcc(0.f);
		pPlayer_AnimState->Set_JumpValue(0.f);
		pPlayer_AnimState->SetUp_State(CPlayer_Falling::Get_Instance());
		pPlayer_AnimState->Set_CollisionObject(nullptr);
		CPlayer_Falling::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.7f, -0.7f, 5.84f, 2.045f), 1.74f, 1.f);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(90.f);
		RELEASE_INSTANCE(CGameInstance);

		return true;
	}
	return false;
}

_bool CPlayer_RailSlide::Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//점프하자마자 Land충돌되서 true되는 것 방지. ->m_bStartJump
	IsStartJump(TimeDelta);

	//점프중에 Rail에 착지했는지 안했는지 판단
	IsNotRailLand(pPlayer_AnimState);

	if (false == m_bStartJump && true == m_bIsNotRailLand)// 위 두 조건이 충족했을 때 Change
	{
		m_iUpperIndex = 0;
		m_iRootIndex = 0;
		m_iBottomIndex = 0;

		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_FallSpeed(1.f);
		pPlayer_AnimState->Set_BackUpY(0.f);
		pPlayer_AnimState->Set_JumpTimeAcc(0.f);
		pPlayer_AnimState->Set_JumpValue(0.f);
		pPlayer_AnimState->Set_JumpPower(6.85f);
		pPlayer_AnimState->Set_RailSlideCheck(false);
		pPlayer_AnimState->Set_CollisionObject(nullptr);
		pPlayer_AnimState->SetUp_State(CPlayer_Landing::Get_Instance());
		CPlayer_Landing::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.7f, -0.7f, 5.84f, 2.045f), 1.74f, 1.f);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(90.f);
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}


	//플레이어가 계속 RailSlider와 충돌하고 있는지 판단. 충돌을 안하고 있을 때, Animstate의 충돌변수를 false해준다.
	SetUp_IsCollideRailSlide(pPlayer_AnimState);
	return false;
}

void CPlayer_RailSlide::SetUp_Movement(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc)
{
	_uint iCount = 0;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			//속도 Up
			SetUp_Speed('W', m_dFastSpeed);
			for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
			{
				if ('D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
				{
					//애니메이션 변경

					for (_uint k = 0; k < pAnimKeyDesc.iKeyCount; ++k)
					{
						if (false == m_bIsJump)
						{
							if (VK_SPACE == pAnimKeyDesc.iKey[k] && pAnimKeyDesc.eKeyState[k] == CPlayer_AnimState::KEY_DOWN)
							{
								//점프(더블점프모션)

								m_iUpperIndex = PLAYER_DOUBLE_JUMP;
								m_iRootIndex = PLAYER_DOUBLE_JUMP;
								m_iBottomIndex = PLAYER_DOUBLE_JUMP;

								SetUp_Double_Jump_Escape(pPlayer_AnimState);
								++m_iCurAnimKey;
							}
						}
					}
				}
				else if ('A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
				{
					//애니메이션 변경

					for (_uint k = 0; k < pAnimKeyDesc.iKeyCount; ++k)
					{
						if (false == m_bIsJump)
						{
							if (VK_SPACE == pAnimKeyDesc.iKey[k] && pAnimKeyDesc.eKeyState[k] == CPlayer_AnimState::KEY_DOWN)
							{
								//점프(더블점프모션)

								m_iUpperIndex = PLAYER_DOUBLE_JUMP;
								m_iRootIndex = PLAYER_DOUBLE_JUMP;
								m_iBottomIndex = PLAYER_DOUBLE_JUMP;

								SetUp_Double_Jump_Escape(pPlayer_AnimState);
								++m_iCurAnimKey;
							}
						}
					}
				}
			}


		}
		else if ('S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			//속도 Down
			if (PLAYER_RAIL_SLIDE == m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_RAIL_SLIDE_BACK;
				m_iRootIndex = PLAYER_RAIL_SLIDE_BACK;
				m_iBottomIndex = PLAYER_RAIL_SLIDE_BACK;
			}

			SetUp_Speed('S', m_dSlowSpeed);
		}
		else
		{
			//가만히 있을 때의 속도변경
			if (PLAYER_RAIL_SLIDE_BACK == m_iRootIndex)
			{
				m_iUpperIndex = PLAYER_RAIL_SLIDE;
				m_iRootIndex = PLAYER_RAIL_SLIDE;
				m_iBottomIndex = PLAYER_RAIL_SLIDE;
			}
			SetUp_Speed(0, m_dDefaultSpeed);
		}

		if ('D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			//애니메이션 변경
			for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
			{
				if (false == m_bIsJump)
				{
					if (VK_SPACE == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_DOWN)
					{
						//점프(더블점프모션)
						m_iUpperIndex = PLAYER_DOUBLE_JUMP;
						m_iRootIndex = PLAYER_DOUBLE_JUMP;
						m_iBottomIndex = PLAYER_DOUBLE_JUMP;

						SetUp_Double_Jump_Escape(pPlayer_AnimState);
						++m_iCurAnimKey;
					}
				}
			}
		}
		else if ('A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			//애니메이션 변경
			for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
			{
				if (false == m_bIsJump)
				{
					if (VK_SPACE == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_DOWN)
					{
						//점프(더블점프모션)
						m_iUpperIndex = PLAYER_DOUBLE_JUMP;
						m_iRootIndex = PLAYER_DOUBLE_JUMP;
						m_iBottomIndex = PLAYER_DOUBLE_JUMP;

						SetUp_Double_Jump_Escape(pPlayer_AnimState);
						++m_iCurAnimKey;
					}
				}
			}

		}
		else if (/*0 == m_iCurAnimKey && */VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			//점프(더블점프모션)
			m_iUpperIndex = PLAYER_DOUBLE_JUMP;
			m_iRootIndex = PLAYER_DOUBLE_JUMP;
			m_iBottomIndex = PLAYER_DOUBLE_JUMP;

			SetUp_Double_Jump_NonEscape(pPlayer_AnimState);
		}
	}

	m_iCurAnimKey = 0;
}

void CPlayer_RailSlide::SetUp_Double_Jump_Escape(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bStartJump = true;
	m_bIsJump = true;

	pPlayer_AnimState->Set_JumpCheck(true);
	pPlayer_AnimState->Set_JumpPower(m_fJumpPower*(_float)m_dJumpCorrection);
	pPlayer_AnimState->Set_Fall_AccelerationSpeed(1.f);
	pPlayer_AnimState->Set_FallSpeed(m_lfFallSpeed);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	//Look을 Rail의 Up으로 바꿔준다.
	m_bIsChangeLook = false;

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
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_RailSlide::SetUp_Double_Jump_NonEscape(CPlayer_AnimState * pPlayer_AnimState)
{
	if (false == m_bIsJump)
	{
		m_bStartJump = true;
		m_bIsJump = true;

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
		pPlayer_AnimState->Set_JumpCheck(true);
		pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
		pPlayer_AnimState->Set_Fall_AccelerationSpeed(1.f);
		pPlayer_AnimState->Set_FallSpeed(m_lfFallSpeed);
		pPlayer_AnimState->Set_BackUpY(0.f);
		pPlayer_AnimState->Set_JumpTimeAcc(0.f);
		pPlayer_AnimState->Set_JumpValue(0.f);
		//Look을 Rail의 Up으로 바꿔준다.
		m_bIsChangeLook = true;

	}
}

void CPlayer_RailSlide::SetUp_IsCollideRailSlide(CPlayer_AnimState * pPlayer_AnimState)
{
	if (false == m_pCharacterController->IsDown())
	{
		pPlayer_AnimState->Set_RailSlideCheck(false);
		OutputDebugString(TEXT("Rail과 충돌이 아닌 상태"));
	}
}

void CPlayer_RailSlide::Move(_double TimeDelta)
{

	Lerp_Look(TimeDelta);
	Lerp_Speed(TimeDelta);
	Lerp_AdjustPos(TimeDelta);

	m_pTransform->Set_MyLook(XMLoadFloat3(&m_vLook));
	//Rail 가운데로 위치를 조정해줌
	Adjust_To_Center(TimeDelta);
	if (false == m_bAdjustStart)
		m_pTransform->Go_Straight(TimeDelta*m_dMoveSpeed, m_pCharacterController);

}
void CPlayer_RailSlide::Adjust_To_Center(_double TimeDelta)
{
	if (true == m_bAdjustStart)
	{
		_float3 vAdjustPos = m_vAdjustPos;
		if (true == m_bIsJump)
		{
			vAdjustPos.y = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION).y;
		}
		m_pTransform->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
		m_pCharacterController->Set_Position(vAdjustPos);
		m_pCharacterController->Set_FootPosition(vAdjustPos);
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vAdjustPos));
	}
}

void CPlayer_RailSlide::IsStartJump(_double TimeDelta)
{
	if (true == m_bStartJump)
	{
		m_dStartJumpAcc += TimeDelta;

		if (m_dStartJumpAccMax < m_dStartJumpAcc)
		{
			m_dStartJumpAcc = 0.0;
			m_bStartJump = false;
		}
	}
}

void CPlayer_RailSlide::Lerp_Speed(_double TimeDelta)
{
	if (true == m_bLerpSpeed)
	{
		m_dLerpSpeedRatio += TimeDelta*m_dLerpSpeed_Speed;

		if (1.0 < m_dLerpSpeedRatio)
		{
			m_dLerpSpeedRatio = 0.0;
			m_bLerpSpeed = false;

			m_dMoveSpeed = m_dDestSpeed;
			return;
		}
		m_dMoveSpeed = (1.0 - m_dLerpSpeedRatio)*m_dMoveSpeed + m_dLerpSpeedRatio*m_dDestSpeed;
	}
}

void CPlayer_RailSlide::Lerp_Look(_double TimeDelta)
{
	if (true == m_bLerpLook)
	{
		m_dLerpLookRatio += TimeDelta*m_dLerpLook_Speed;

		if (1.0 < m_dLerpLookRatio)
		{
			m_dLerpLookRatio = 0.0;
			m_bLerpLook = false;

			m_vLook = m_vDestLook;
			return;
		}

		m_vLook = STOREFLOAT3((1.f - (_float)m_dLerpLookRatio)*XMLoadFloat3(&m_vSourLook) + (_float)m_dLerpLookRatio*XMLoadFloat3(&m_vDestLook));
	}
}

void CPlayer_RailSlide::Lerp_AdjustPos(_double TimeDelta)
{
	if (true == m_bLerpAdjust)
	{
		//m_dLerpAdjustRatio += TimeDelta*m_dLerpAdjust_Speed*((m_dFastSpeed*2.0)/m_dMoveSpeed);
		m_dLerpAdjustRatio += 2.0;/*보간해도 어짜피 곡선이 아닌데 곡선처럼 움직이는거라서 보간 안할꺼임*/


		//m_vDestAdjustPos = STOREFLOAT3(XMLoadFloat3(&m_vDestAdjustPos) + XMVector3Normalize(XMLoadFloat3(&m_vDestLook))*TimeDelta*m_dMoveSpeed);
		if (1.0 < m_dLerpAdjustRatio)
		{
			m_dLerpAdjustRatio = 0.0;
			m_bLerpAdjust = false;
			m_bAdjustStart = false;

			m_vAdjustPos = m_vSourAdjustPos = m_vDestAdjustPos;

			_float3 vAdjustPos = m_vAdjustPos;
			if (true == m_bIsJump)
			{
				vAdjustPos.y = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION).y;
			}
			m_pCharacterController->Set_FootPosition(vAdjustPos);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vAdjustPos));
			return;
		}
		m_vAdjustPos = STOREFLOAT3((1.f - (_float)m_dLerpAdjustRatio)*XMLoadFloat3(&m_vSourAdjustPos) + (_float)m_dLerpAdjustRatio*XMLoadFloat3(&m_vDestAdjustPos));
	}
}

void CPlayer_RailSlide::IsNotRailLand(CPlayer_AnimState * pPlayer_AnimState)
{
	if (true == m_bIsJump)//만약 점프중에
	{
		if (m_iRootIndex == PLAYER_DOUBLE_JUMP && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOUBLE_JUMP)) //더블점프애니메이션끝
		{
			if (PALYER_LANDING != m_iRootIndex) //랜딩자세가아니라면 
			{
				m_iUpperIndex = PLAYER_FALLING;
				m_iRootIndex = PLAYER_FALLING;
				m_iBottomIndex = PLAYER_FALLING;
			}
		}

		if (false == m_bStartJump) // 점프 초반에 down 방지
		{
			if (true == m_pCharacterController->IsDown())//착지했는데
			{
				if (false == pPlayer_AnimState->Get_RailSlideCheck())//그 때 Rail콜리전에 닿았는지 안닿았는지 판단
				{
					m_bIsNotRailLand = true;
				}
				else
				{
					m_iUpperIndex = PLAYER_RAIL_SLIDE_ENTER;
					m_iRootIndex = PLAYER_RAIL_SLIDE_ENTER;
					m_iBottomIndex = PLAYER_RAIL_SLIDE_ENTER;

					m_bIsNotRailLand = false;
					m_bIsChangeLook = true;
				}
				m_bIsJump = false;
			}
		}
	}
}

void CPlayer_RailSlide::SetUp_Speed(_uint iKey, _double dSpeed)
{
	if (iKey != m_iCurKey)
	{
		m_dLerpSpeedRatio = 0.0;
		m_bLerpSpeed = true;
		m_dDestSpeed = dSpeed;
	}

	m_iCurKey = iKey;
}

_bool CPlayer_RailSlide::RailRayCast(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (true == m_bRay_ChangeRail)
	{
		m_dRayTimeAcc += TimeDelta;
		if (0.5f < m_dRayTimeAcc)
		{
			m_bRay_ChangeRail = false;
			m_dRayTimeAcc = 0.0;
		}
	}
	if (true == m_bIsChangeLook )
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;

		//원위치에서 검사
		tDescIn.vOrigin = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
		XMStoreFloat3(&tDescIn.vUnitDir, XMVectorSet(0.f, -1.f, 0.f, 0.f));
		tDescIn.fDistance = 10.f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_RAIL);
		CGameObject* pHitObject = nullptr;
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (m_pBackUpRailObject != pHitObject && false ==m_bRay_ChangeRail )
			{
				pPlayer_AnimState->Set_CollisionObject(pHitObject);
				m_bRay_ChangeRail = true;
			}
			RELEASE_INSTANCE(CGameInstance);
			return true;

		}

		//오른쪽에서 검사
		m_pTransform->Go_Right(m_dRayTrans);
		tDescIn.vOrigin = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
		XMStoreFloat3(&tDescIn.vUnitDir, XMVectorSet(0.f, -1.f, 0.f, 0.f));
		tDescIn.fDistance = 10.f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_RAIL);
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (m_pBackUpRailObject != pHitObject && false == m_bRay_ChangeRail)
			{
				pPlayer_AnimState->Set_CollisionObject(pHitObject);
				m_bRay_ChangeRail = true;
			}
			RELEASE_INSTANCE(CGameInstance);
			m_pTransform->Go_Left(m_dRayTrans);
			return true;

		}
		m_pTransform->Go_Left(m_dRayTrans);

		//왼쪽에서 검사
		m_pTransform->Go_Left(m_dRayTrans);
		tDescIn.vOrigin = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
		XMStoreFloat3(&tDescIn.vUnitDir, XMVectorSet(0.f, -1.f, 0.f, 0.f));
		tDescIn.fDistance = 10.f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_RAIL);
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (m_pBackUpRailObject != pHitObject && false == m_bRay_ChangeRail)
			{
				pPlayer_AnimState->Set_CollisionObject(pHitObject);
				m_bRay_ChangeRail = true;

			}
			RELEASE_INSTANCE(CGameInstance);
			m_pTransform->Go_Right(m_dRayTrans);
			return true;

		}
		m_pTransform->Go_Right(m_dRayTrans);
		RELEASE_INSTANCE(CGameInstance);
	}

	return false;
}

void CPlayer_RailSlide::Compute_AdjustDestPos(CRailSlide* pRailObject)
{
	//점 선택
	if (nullptr == pRailObject)
		return;
	CTransform* pTransform = dynamic_cast<CTransform*>(pRailObject->Get_Components(TEXT("Com_Transform")));
	if (nullptr == pTransform)
		return;

	_float3 vRight = pTransform->Get_State_Float3(CTransform::STATE_RIGHT);
	_float3 vLook = pTransform->Get_State_Float3(CTransform::STATE_LOOK);
	_float3 vUp = pTransform->Get_State_Float3(CTransform::STATE_UP);

	_float3 vSelectDir = vRight;
	if (vSelectDir.y <= -vRight.y)
		vSelectDir = STOREFLOAT3(-1 * XMLoadFloat3(&vRight));
	if (vSelectDir.y <= vLook.y)
		vSelectDir = STOREFLOAT3(XMLoadFloat3(&vLook));
	if (vSelectDir.y <= -vLook.y)
		vSelectDir = STOREFLOAT3(-1 * XMLoadFloat3(&vLook));

	_float3 vPointA = STOREFLOAT3(XMLoadFloat3(&m_vBackUpObjectPos) + XMLoadFloat3(&vSelectDir) / 2.f);
	_float3 vPointB = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);
	_vector vReverseRailDir = XMVector3Normalize(-XMLoadFloat3(&m_vDestLook));
	_vector vDir = XMLoadFloat3(&vPointB) - XMLoadFloat3(&vPointA);
	_float fLength = XMVectorGetX(XMVector3Dot(vDir, vReverseRailDir));

	m_vDestAdjustPos = STOREFLOAT3(XMLoadFloat3(&vPointA) + vReverseRailDir*fLength);
	if (1 == m_iCount)
		_uint i = 0;

	_float fDot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_vDestLook), vReverseRailDir));

	m_bLerpAdjust = true;
	m_dLerpAdjustRatio = 0.0;
	m_vAdjustPos = m_vSourAdjustPos = m_BackUpPos;
	if (nullptr == m_pBackUpRailObject)
		m_vAdjustPos = m_vSourAdjustPos = m_pTransform->Get_State_Float3(CTransform::STATE_POSITION);

	m_vDestAdjustPos.y = m_vAdjustPos.y;

}

void CPlayer_RailSlide::SetUp_Values(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{

	if (nullptr == pPlayer_AnimState->Get_CollisionObject())
	{
		OutputDebugString(TEXT("Rail 충돌 객체 nullptr"));
		return;
	}

	CRailSlide* pRailSlide = dynamic_cast<CRailSlide*>(pPlayer_AnimState->Get_CollisionObject());
	if (nullptr == pRailSlide)
	{
		OutputDebugString(TEXT("Rail 충돌 객체 nullptr"));
		return;
	}


	if (false == m_bRailChangeSign)
	{
		_vector vObjectPos = dynamic_cast<CRailSlide*>(pRailSlide)->Get_Pos();


		if (true == m_bIsChangeLook && m_pBackUpRailObject != pRailSlide)
		{

			_float3 vDir = STOREFLOAT3(XMVector3Normalize(XMLoadFloat3(&dynamic_cast<CRailSlide*>(pRailSlide)->Get_Up())));/*Rail의 Up*/

			//Rail의 Up을 Reverse할지 판단
			_vector vCenterDir = XMVector3Normalize(m_pTransform->Get_Position() - vObjectPos);
			if (0.f < XMVectorGetX(XMVector3Dot(XMLoadFloat3(&vDir), vCenterDir)))
				vDir = STOREFLOAT3(XMLoadFloat3(&vDir)*-1.f);

			//처음 점프 후 착지했을 때 Reverse할지 판단
			if (nullptr == m_pBackUpRailObject)
			{
				if (0.f > XMVectorGetX(XMVector3Dot(XMLoadFloat3(&vDir), XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK)))))
					vDir = STOREFLOAT3(XMLoadFloat3(&vDir)*-1.f);
			}

			//Look보간 및 값 설정
			m_vDestLook = vDir;
			m_vSourLook = m_pTransform->Get_State_Float3(CTransform::STATE_LOOK);
			m_bLerpLook = true;

			//가운데로 이동 보간 및 값 설정
			m_vBackUpObjectPos = STOREFLOAT3(vObjectPos);
			Compute_AdjustDestPos(pRailSlide);
			m_bAdjustStart = true;

			//Rail충돌체크 텀 설정
			m_bRailChangeSign = true;
			m_dRailTimeAcc = 0.0;
		}
		m_pBackUpRailObject = pRailSlide;
		m_vBackUpObjectPos = STOREFLOAT3(vObjectPos);

	}
	else
	{
		if (false == m_bAdjustStart)
		{
			m_dRailTimeAcc += TimeDelta*m_dRailCollisionCheckTerm;
			if (1.f < m_dRailTimeAcc)
			{
				m_bRailChangeSign = false;
				m_dRailTimeAcc = 0.0;
			}
		}
	}

}



void CPlayer_RailSlide::Set_BlendSpeed(_double TimeDelta)
{
	if (PLAYER_RUN_START == m_pModel->Get_RootAnimIndex())
	{
		if (0 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START) && 5 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START))
		{
			m_pModel->Set_UpperAnimSpeed(PLAYER_RUN_START, 0.8f);
			m_pModel->Set_RootAnimSpeed(PLAYER_RUN_START, 0.8f);
			m_pModel->Set_BottomAnimSpeed(PLAYER_RUN_START, 0.8f);
		}
		if (7 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START))
		{
			m_pModel->Set_UpperAnimSpeed(PLAYER_RUN_START, 1.2f);
			m_pModel->Set_RootAnimSpeed(PLAYER_RUN_START, 1.2f);
			m_pModel->Set_BottomAnimSpeed(PLAYER_RUN_START, 1.2f);
		}
	}

}

void CPlayer_RailSlide::Set_RailValues(_double dStartSpeed, _double dDefaultSpeed, _double dFastSpeed, _double dSlowSpeed, _double dNonFixJumpPower, _double dRailCollisionCheckTerm, _double dJumpCorrection, _double dStartJumpAccMax, _double dRayTrans, _double dJumpPower, _double dJumpFallSpeed, _double dLookSpeed, _double dMoveSpeed, _double dAdjustSpeed)
{
	m_dStartSpeed = dStartSpeed;
	m_dDefaultSpeed = dDefaultSpeed;
	m_dFastSpeed = dFastSpeed;
	m_dSlowSpeed = dSlowSpeed;
	m_dNonFixJumpPower = dNonFixJumpPower;
	m_dRailCollisionCheckTerm = dRailCollisionCheckTerm;
	m_dJumpCorrection = dJumpCorrection;
	m_dStartJumpAccMax = dStartJumpAccMax;
	m_dRayTrans = dRayTrans;
	m_fJumpPower = (_float)dJumpPower;
	m_lfFallSpeed = dJumpFallSpeed;
	m_dLerpLook_Speed = dLookSpeed;
	m_dLerpSpeed_Speed = dMoveSpeed;
	m_dLerpAdjust_Speed = dAdjustSpeed;
}

void CPlayer_RailSlide::Free()
{
}

