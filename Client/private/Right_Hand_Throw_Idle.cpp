#include "stdafx.h"
#include "..\public\Right_Hand_Throw_Idle.h"
#include "Right_Hand_Default.h"
#include "Right_Hand_Throw_Right.h"
#include "Right_Hand_Throw_Finished.h"
#include "Player.h"
#include "Boss_Dish.h"

IMPLEMENT_SINGLETON(CRight_Hand_Throw_Idle)

CRight_Hand_Throw_Idle::CRight_Hand_Throw_Idle()
{
}

void CRight_Hand_Throw_Idle::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	First_MotionState_Cheking(pRightHandAnim, pRightHandDesc, TimeDelta);

	pRightHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);



	if (true == pRightHandAnim->Get_ChangeCheck())
	{
		switch (pRightHandAnim->Get_Right_HandState())
		{
		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_DISH_THROW:
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CRight_Hand_Throw_Right::Get_Instance());
			CRight_Hand_Throw_Right::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;
		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_DISH_END:
			Reset_Anim(pRightHandAnim, pRightHandDesc, TimeDelta);
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CRight_Hand_Default::Get_Instance());
			CRight_Hand_Default::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;
		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_DISH_FINISIHED:
			Reset_Anim(pRightHandAnim, pRightHandDesc, TimeDelta);
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CRight_Hand_Throw_Finished::Get_Instance());
			CRight_Hand_Throw_Finished::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;
		}
	}
}

void CRight_Hand_Throw_Idle::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);
}

void CRight_Hand_Throw_Idle::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	pRightHandAnim->Set_BoneCaculationCheck(false);
	pRightHandAnim->Set_ThrowBoneCaculationCheck(false);
	m_iAnimIndex = Right_Throw_Idle_One;
	m_bFirstCheck = false;
}

void CRight_Hand_Throw_Idle::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}

void CRight_Hand_Throw_Idle::First_MotionState_Cheking(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
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
			m_iAnimIndex = Right_Throw_Idle_One;
			break;
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_SOUP:
			m_iAnimIndex = Right_Throw_Idle_Two;
			pRightHandAnim->Set_BoneCaculationCheck(true);
			pRightHandAnim->Set_ThrowBoneCaculationCheck(true);
			break;
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_COOKIE:
			m_iAnimIndex = Right_Throw_Idle_Three
				;
			break;
		}

		Safe_Release(pGameInstance);

		m_bFirstCheck = true;
	}
}

void CRight_Hand_Throw_Idle::Free()
{
}
