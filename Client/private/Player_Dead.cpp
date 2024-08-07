#include "stdafx.h"
#include "..\public\Player_Dead.h"
#include "Player_IdleAnim.h"
#include "Camera_Fly.h"
#include "FadeInOut.h"

IMPLEMENT_SINGLETON(CPlayer_Dead)

CPlayer_Dead::CPlayer_Dead()
{
}

void CPlayer_Dead::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_DeadMotionCheck(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Camera_Back_State(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == End_to_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Dead::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Dead::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bFirstCheck = false;
	m_bFadeInCheck = false;
}


void CPlayer_Dead::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_Dead::First_DeadMotionCheck(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iUpperIndex = PLAYER_RESPAWN;
		m_iRootIndex = PLAYER_RESPAWN;
		m_iBottomIndex = PLAYER_RESPAWN;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);


		if(SCENE_STAGE03 == pGameInstance->Get_CurrentSceneIndex())
		{
			m_pCharacterController->Set_Position(_float3(236.62f, 3.15f, 177.274f));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(236.62f, 3.15f, 177.274f)), 1.f));
			m_pTransform->Set_Velocity(XMVectorZero());
		}

		Safe_Release(pGameInstance);
		m_bFirstCheck = true;
	}

}

void CPlayer_Dead::Camera_Back_State(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (10 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_RESPAWN))
	{
		if (false == m_bFadeInCheck)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);

			Safe_Release(pGameInstance);

			m_bFadeInCheck = true;
		}
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);

		Safe_Release(pGameInstance);
	}

}

_bool CPlayer_Dead::End_to_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_iUpperIndex == PLAYER_RESPAWN && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_RESPAWN))
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

void CPlayer_Dead::Free()
{
}

