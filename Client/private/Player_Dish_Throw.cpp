#include "stdafx.h"
#include "..\public\Player_Dish_Throw.h"
#include "Player_IdleAnim.h"
#include "Boss_Dish.h"
#include "Camera_Fly.h"

IMPLEMENT_SINGLETON(CPlayer_Dish_Throw)

CPlayer_Dish_Throw::CPlayer_Dish_Throw()
{
}

void CPlayer_Dish_Throw::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	Frist_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);


	if (CBoss_Dish::BOSS_FOODTYPE_SOUP == m_pPlayer->Get_DishType())
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(162))
		{

			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}
	else
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(161))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return;
		}
	}

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Dish_Throw::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Dish_Throw::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_pCharacterController->Set_Position(_float3(40.f, 0.095f, 25.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(40.f, 0.095f, 25.f)), 1.f));

	m_pPlayer->Set_Boss_DishCheck(false, CBoss_Dish::BOSS_FOODTYPE_END);
	m_bFirstCheck = false;
}

void CPlayer_Dish_Throw::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
}

void CPlayer_Dish_Throw::Frist_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DishCheckType_On();
		Safe_Release(pGameInstance);

		if (CBoss_Dish::BOSS_FOODTYPE_SOUP == m_pPlayer->Get_DishType())
		{
			m_iUpperIndex = 162;
			m_iRootIndex = 162;
			m_iBottomIndex = 162;
		}
		else
		{
			m_iUpperIndex = 161;
			m_iRootIndex = 161;
			m_iBottomIndex = 161;

			m_pCharacterController->Set_FootPosition(_float3(40.f, 0.090f, 39.554f));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(40.f, 0.090f, 39.554f)), 1.f));
		}
		m_bFirstCheck = true;
	}
}

void CPlayer_Dish_Throw::Free()
{
}

