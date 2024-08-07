#include "stdafx.h"
#include "..\public\Left_Hand_Throw_Idle.h"
#include "Left_Hand_Default.h"
#include "Left_Hand_Throw_Left.h"
#include "Left_Hand_Throw_Finished.h"
#include "Boss_Dish.h"

IMPLEMENT_SINGLETON(CLeft_Hand_Throw_Idle)

CLeft_Hand_Throw_Idle::CLeft_Hand_Throw_Idle()
{
}

void CLeft_Hand_Throw_Idle::HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	First_MotionState_Cheking(pLeftHandAnim, pLeftHandDesc, TimeDelta);

	pLeftHandAnim->Set_RenderCheck(true);
	
	m_pModelCom->Set_Animation(m_iAnimIndex);



	if (true == pLeftHandAnim->Get_ChangeCheck())
	{
		switch (pLeftHandAnim->Get_Left_HandState())
		{
		case CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_DISH_THROW:
			pLeftHandAnim->Set_ChangeCheck(false);
			pLeftHandAnim->SetUp_State(CLeft_Hand_Throw_Left::Get_Instance());
			CLeft_Hand_Throw_Left::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			break;
		case CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_DISHEND:
			Reset_Anim(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			pLeftHandAnim->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_END);
			pLeftHandAnim->Set_ChangeCheck(false);
			pLeftHandAnim->SetUp_State(CLeft_Hand_Default::Get_Instance());
			CLeft_Hand_Default::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			break;
		case CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_DISH_FINISHED:
			Reset_Anim(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			pLeftHandAnim->Set_ChangeCheck(false);
			pLeftHandAnim->SetUp_State(CLeft_Hand_Throw_Finished::Get_Instance());
			CLeft_Hand_Throw_Finished::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			break;
		}
	}
}

void CLeft_Hand_Throw_Idle::Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);
}

void CLeft_Hand_Throw_Idle::Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	pLeftHandAnim->Set_ThrowBoneCaculationCheck(false);
	m_iAnimIndex = Throw_Idle_One;
	m_bFirstCheck = false;
}

void CLeft_Hand_Throw_Idle::Set_Colider(CLeft_Hand_Anim * pLeftHandAnim)
{
}

void CLeft_Hand_Throw_Idle::First_MotionState_Cheking(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject*	pPlayer = pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
		if (nullptr == pPlayer)
		{
			Safe_Release(pGameInstance);
			return;
		}

		switch (dynamic_cast<CPlayer*>(pPlayer)->Get_DishType())
		{
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_DISH:
			m_iAnimIndex = Throw_Idle_One;
			break;
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_SOUP:
			pLeftHandAnim->Set_ThrowBoneCaculationCheck(true);
			m_iAnimIndex = Throw_Idle_Two;
			break;
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_COOKIE:
			m_iAnimIndex = Throw_Idle_Three;
			break;
		}



		Safe_Release(pGameInstance);

		m_bFirstCheck = true;
	}
}

void CLeft_Hand_Throw_Idle::Free()
{
}
