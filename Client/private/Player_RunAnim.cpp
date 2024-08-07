#include "stdafx.h"
#include "..\public\Player_RunAnim.h"
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
#include "Player_Bouncer.h"
#include "Player_Blender_Climbing.h"
#include "Camera_Fly.h"
#include "Player_CloudAnim.h"

IMPLEMENT_SINGLETON(CPlayer_RunAnim)

CPlayer_RunAnim::CPlayer_RunAnim()
{
}

void CPlayer_RunAnim::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == Set_GravityChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
		if (nullptr != pWalkSmoke)
			pWalkSmoke->Set_IsInteraction(true);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (true == Hit_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
		if (nullptr != pWalkSmoke)
			pWalkSmoke->Set_IsInteraction(true);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (true == Run_To_Ground_Slide(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
		if (nullptr != pWalkSmoke)
			pWalkSmoke->Set_IsInteraction(true);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (true == Run_To_Bouncer(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Run_To_Blender(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Run_To_Cloud(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	Run_to_Effect_Create(TimeDelta);

	if (false == m_bFirstCheck)
	{
		First_State(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		Effect_Create();


		if (false == m_bMoveOffsetCheck)
			m_fMoveOffset = 0.3f;

		m_lfMoveSpeed = 0.8;
		m_bFirstCheck = true;
	}

	pPlayer_AnimState->Grab_RayCast_Loop(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
	if(nullptr != pWalkSmoke)
		pWalkSmoke->Set_IsInteraction(false);
	RELEASE_INSTANCE(CGameInstance);

	_uint iCount = 0;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (false == m_bMotionCheck)
			{
				m_iRootIndex = PLAYER_RUN_START;
				m_iBottomIndex = PLAYER_RUN_START;

				if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_RUN_START))
				{
					if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
						pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
					else
						pPlayer_AnimState->Set_AllLerpCheckTrue();

					m_iRootIndex = PLAYER_RUN;
					m_iBottomIndex = PLAYER_RUN;

					m_bMotionCheck = true;
				}
			}
			else
			{
				m_iRootIndex = PLAYER_RUN;
				m_iBottomIndex = PLAYER_RUN;
			}

		}

		if (VK_RBUTTON == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			if (PLAYER_GRAP_ATTACK_START != m_pModel->Get_UpperAnimIndex())
			{
				if (false == pPlayer_AnimState->Get_GrabCheck())
				{
					pPlayer_AnimState->Grab_RayCast_Ready(TimeDelta);

					if (true == pPlayer_AnimState->Get_GrabBackCheck())
					{
						Reset_Anim(pPlayer_AnimState);

						m_pRightHand->Set_ChangeAnimCheck(true);
						m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB_PULLING);

						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->Set_GrabBack(false);
						pPlayer_AnimState->SetUp_State(CPlayer_GrabBack::Get_Instance());
						CPlayer_GrabBack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return;
					}
					else
					{
						pPlayer_AnimState->Set_UpperLerpCheck(true);

						m_iUpperIndex = PLAYER_GRAP_ATTACK_START;
						m_pModel->Set_UpperAnimation(PLAYER_GRAP_ATTACK_START);
					}
				}
				else
				{
					if (true == pPlayer_AnimState->Get_GrabAttackCheck())
					{
						pPlayer_AnimState->Grab_RayCasting();

						Reset_Anim(pPlayer_AnimState);

						m_pRightHand->Set_ChangeAnimCheck(true);
						m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB_ATTACK);

						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_GrabAttack::Get_Instance());
						CPlayer_GrabAttack::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return;
					}
				}
			}

		}

		if (PLAYER_GRAP_ATTACK_START != m_pModel->Get_UpperAnimIndex() && false == m_pModel->Get_IsRootLerp())
		{
			if ('E' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Pisblast::Get_Instance());
				CPlayer_Pisblast::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);
				RELEASE_INSTANCE(CGameInstance);
				return;

			}
			//else if ('R' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			//{
			//	Reset_Anim(pPlayer_AnimState);
			//	pPlayer_AnimState->Set_AllLerpCheckTrue();
			//	pPlayer_AnimState->SetUp_State(CPlayer_Ball::Get_Instance());
			//	CPlayer_Ball::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			//	CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
			//	if (nullptr != pWalkSmoke)
			//		pWalkSmoke->Set_IsInteraction(true);
			//	RELEASE_INSTANCE(CGameInstance);
			//	return;
			//}
			else if ('Q' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_BrainStom::Get_Instance());
				CPlayer_BrainStom::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			else if (VK_SHIFT == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->Set_OneLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Dash::Get_Instance());
				CPlayer_Dash::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);
				RELEASE_INSTANCE(CGameInstance);
				return;

			}
			else if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
				CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			else if (VK_LBUTTON == pAnimKeyDesc.iOnceKey)
			{
				Reset_Anim(pPlayer_AnimState);

				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_ComboAttack::Get_Instance());
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		if (pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_UP)
			++iCount;
	}

	if (true == pPlayer_AnimState->Get_GravityCheck())
	{
		Reset_Anim(pPlayer_AnimState);

		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Falling::Get_Instance());
		CPlayer_Falling::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
		if (nullptr != pWalkSmoke)
			pWalkSmoke->Set_IsInteraction(true);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	Separation_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Set_BlendSpeed(TimeDelta);

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);


	if (0 == pAnimKeyDesc.iKeyCount && false == m_pModel->Get_IsRootLerp())
	{
		if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
		{
			Reset_Anim(pPlayer_AnimState);

			pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->Set_PlayerMoveAccelecrtaion(m_fMoveOffset);
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
			if (nullptr != pWalkSmoke)
				pWalkSmoke->Set_IsInteraction(true);
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		else if (PLAYER_RUN_START == m_iRootIndex)
		{
			Reset_Anim(pPlayer_AnimState);

			if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
				pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
			else
				pPlayer_AnimState->Set_AllLerpCheckTrue();

			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->Set_PlayerMoveAccelecrtaion(m_fMoveOffset);
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
			if (nullptr != pWalkSmoke)
				pWalkSmoke->Set_IsInteraction(true);
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		else
		{
			Reset_Anim(pPlayer_AnimState);

			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_StopAnim::Get_Instance());
			CPlayer_StopAnim::Get_Instance()->Set_PlayerMoveAccelecrtaion(m_fMoveOffset);
			CPlayer_StopAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
			if (nullptr != pWalkSmoke)
				pWalkSmoke->Set_IsInteraction(true);
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}

}

void CPlayer_RunAnim::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bIsMoveOn = true;
	Move_Acceleration(TimeDelta);
	Handle_Movement(pAnimKeyDesc);
	Player_Move(TimeDelta);

	m_pModel->Root_Update(TimeDelta);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_RunAnim::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_pModel->Set_UpperAnimBlendSpeed(PALYER_LANDING, 10.f);
	m_pModel->Set_RootAnimBlendSpeed(PALYER_LANDING, 10.f);
	m_pModel->Set_BottomAnimBlendSpeed(PALYER_LANDING, 10.f);

	m_fMoveOffset = 0.f;
	m_bMotionCheck = false;
	m_bStartCheck = false;
	m_bFirstCheck = false;
	m_bIsMoveOn = false;
}

void CPlayer_RunAnim::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
	if (nullptr != pWalkSmoke)
		pWalkSmoke->Set_IsInteraction(true);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_RunAnim::First_State(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN_START, 9.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN_START, 9.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN_START, 9.f);


	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN, 7.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN, 7.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN, 7.f);
}

void CPlayer_RunAnim::Effect_Create()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->SettingVolume(0.5f, PLAYER_RUNNING);

	_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"));
	if (0 < iSize)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"),
		SCENE_STATIC, TEXT("Effect_Repeat_WalkSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransform, TEXT("Layer_Repeat_WalkSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
	if (nullptr != pWalkSmoke)
		pWalkSmoke->Set_IsInteraction(false);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_RunAnim::Separation_Motion(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_GRAP_ATTACK_START == m_pModel->Get_UpperAnimIndex())
	{
		if (true == m_pModel->Get_IsUpperAnimFinishied(PLAYER_GRAP_ATTACK_START))
		{
			pPlayer_AnimState->Set_UpperLerpCheck(true);

			if (false == m_bMotionCheck)
				m_iUpperIndex = PLAYER_RUN_START;
			else
				m_iUpperIndex = PLAYER_RUN;
		}
		else
			m_iUpperIndex = PLAYER_GRAP_ATTACK_START;
	}
	else
	{
		if (false == m_bMotionCheck)
			m_iUpperIndex = PLAYER_RUN_START;
		else
			m_iUpperIndex = PLAYER_RUN;
	}
}

_bool CPlayer_RunAnim::Set_GravityChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

_bool CPlayer_RunAnim::Hit_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

_bool CPlayer_RunAnim::Run_To_Ground_Slide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
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

_bool CPlayer_RunAnim::Run_To_Bouncer(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < 3; i++)
	{
		if (true == m_pPlayer->Get_IsBouncer(i))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_Bouncer::Get_Instance());
			CPlayer_Bouncer::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_RunAnim::Run_To_Blender(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_pPlayer->Get_Climbing_TriggerCheck())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		_float3 vVelocity;
		_float4 vUpRayDirection;
		_float3 vPlayerPos;
		XMStoreFloat3(&vPlayerPos, m_pTransform->Get_State(CTransform::STATE_POSITION));

		XMStoreFloat3(&tDescIn.vOrigin, XMVectorSet(vPlayerPos.x, vPlayerPos.y + 0.9f, vPlayerPos.z, 1.f));
		XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK)));

		tDescIn.fDistance = 0.6f;
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_CLIMBINGPLANE);
		CGameObject* pHitObject = nullptr;
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (pHitObject)
			{
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);	

				((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(100.f, 1.f);
				m_pTransform->Set_State_Float3(CTransform::STATE_POSITION, _float3(tDescOut.vHitPos.x, tDescOut.vHitPos.y - 0.9f, tDescOut.vHitPos.z));
				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_Blender_Climbing::Get_Instance());
				//m_pPlayer->Set_조건(false) //필요한경우에만 위의 조건문에 true로들어오고 false로자동으로 바뀌는게없으면 여기서처리하거나
				//blender FSM에서 RESET해줄때 FALSE해주거나 해주면됨
				CPlayer_Blender_Climbing::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				RELEASE_INSTANCE(CGameInstance);
				return true;
			}
			else
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}
	}
	return false;
}

_bool CPlayer_RunAnim::Run_To_Cloud(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if ('V' == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_DOWN)
	{
		if (true == pPlayer_AnimState->Cloud_RayCast_Ready(TimeDelta))
		{
			if (nullptr != pPlayer_AnimState)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CGameObject* pWalkSmoke = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_WalkSmoke"), 0);
				if (nullptr != pWalkSmoke)
					pWalkSmoke->Set_IsInteraction(true);
				RELEASE_INSTANCE(CGameInstance);

				Reset_Anim(pPlayer_AnimState);
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_CloudAnim::Get_Instance());
				CPlayer_CloudAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
		return false;
	}
	return false;
}


void CPlayer_RunAnim::Run_to_Effect_Create(_double TimeDelta)
{
	if (PLAYER_RUN_START == m_iRootIndex)
	{
		if (4 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START) && 7 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START))
		{
			if (false == m_bSoundRunCheck[0])
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(CHANNELID::PLAYER_RUNNING);
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Run.wav"), CHANNELID::PLAYER_RUNNING)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
				m_bSoundRunCheck[0] = true;
				m_bSoundRunCheck[1] = false;
				m_bSoundRunCheck[2] = false;
			}

		}
		else if (9 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START) && 11 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START))
		{
			//이펙트생성위치
			//오른발
			if (false == m_bSoundRunCheck[1])
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(CHANNELID::PLAYER_RUNNING);
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Run.wav"), CHANNELID::PLAYER_RUNNING)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
				m_bSoundRunCheck[1] = true;
				m_bSoundRunCheck[0] = false;
				m_bSoundRunCheck[2] = false;
			}

		}
		else if (15 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START) && 18 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN_START))
		{
			//이펙트생성위치
			//왼발
			if (false == m_bSoundRunCheck[2])
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(CHANNELID::PLAYER_RUNNING);
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Run.wav"), CHANNELID::PLAYER_RUNNING)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
				m_bSoundRunCheck[2] = true;
				m_bSoundRunCheck[0] = false;
				m_bSoundRunCheck[1] = false;
			}
		}
	}
	else if (PLAYER_RUN == m_iRootIndex)
	{
		if (3 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN) && 6 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN))
		{
			//이펙트생성위치
			//오른발
			if (false == m_bSoundRunCheck[3])
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(CHANNELID::PLAYER_RUNNING);
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Run.wav"), CHANNELID::PLAYER_RUNNING)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
				m_bSoundRunCheck[3] = true;
				m_bSoundRunCheck[4] = false;
			}
		}
		else if (10 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN) && 13 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RUN))
		{
			//이펙트생성위치
			//왼발
			if (false == m_bSoundRunCheck[4])
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(CHANNELID::PLAYER_RUNNING);
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Run.wav"), CHANNELID::PLAYER_RUNNING)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				RELEASE_INSTANCE(CGameInstance);
				m_bSoundRunCheck[4] = true;
				m_bSoundRunCheck[3] = false;
			}
		}

	}

}

void CPlayer_RunAnim::Set_BlendSpeed(_double TimeDelta)
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

void CPlayer_RunAnim::Free()
{
}

