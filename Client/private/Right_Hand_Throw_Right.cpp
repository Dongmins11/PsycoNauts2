
#include "stdafx.h"
#include "..\public\Right_Hand_Throw_Right.h"
#include "Right_Hand_Default.h"
#include "Right_Hand_Throw_Idle.h"
#include "Player.h"
#include "Boss_Food_Projectile.h"
#include "Boss_Dish.h"

IMPLEMENT_SINGLETON(CRight_Hand_Throw_Right)

CRight_Hand_Throw_Right::CRight_Hand_Throw_Right()
{
}

void CRight_Hand_Throw_Right::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	First_MotionState_Cheking(pRightHandAnim, pRightHandDesc, TimeDelta);

	pRightHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);

}

void CRight_Hand_Throw_Right::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);

	Food_Create(pRightHandAnim, pRightHandDesc, TimeDelta);

	if (true == m_pModelCom->Get_IsAnimFinishied(Throw_Right_Throw_One) ||
		true == m_pModelCom->Get_IsAnimFinishied(Throw_Right_Throw_Two) ||
		true == m_pModelCom->Get_IsAnimFinishied(Throw_Right_Throw_Three))
	{
		Reset_Anim(pRightHandAnim, pRightHandDesc, TimeDelta);
		pRightHandAnim->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_IDLE);
		pRightHandAnim->SetUp_State(CRight_Hand_Throw_Idle::Get_Instance());
		CRight_Hand_Throw_Idle::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
		return;
	}
}

void CRight_Hand_Throw_Right::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_iAnimIndex = Throw_Right_Throw_One;
	m_bFirstCheck = false;
	m_bFoodCreateCheck = false;
}

void CRight_Hand_Throw_Right::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}

void CRight_Hand_Throw_Right::First_MotionState_Cheking(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
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
		Safe_Release(pGameInstance);

		switch (dynamic_cast<CPlayer*>(pPlayer)->Get_DishType())
		{
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_DISH:
			m_iAnimIndex = Throw_Right_Throw_One;
			break;
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_SOUP:
			m_iAnimIndex = Throw_Right_Throw_Two;
			break;
		case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_COOKIE:
			m_iAnimIndex = Throw_Right_Throw_Three;
			break;
		}


		m_bFirstCheck = true;
	}
}

void CRight_Hand_Throw_Right::Food_Create(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	if (9 <= m_pModelCom->Get_AnimationCurrentFrame(Throw_Right_Throw_One) ||
		9 <= m_pModelCom->Get_AnimationCurrentFrame(Throw_Right_Throw_Two) ||
		9 <= m_pModelCom->Get_AnimationCurrentFrame(Throw_Right_Throw_Three))
	{
		if (false == m_bFoodCreateCheck)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


			CGameObject*	pPlayer = pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
			if (nullptr == pPlayer)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			switch (dynamic_cast<CPlayer*>(pPlayer)->Get_DishType())
			{
			case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_DISH:
			{
				if (0 == m_iDishFoodCount)
				{
					++m_iDishFoodCount;

					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_CARROT;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_One"), &BossDishArg)))
						return;

				}
				else if (1 == m_iDishFoodCount)
				{
					++m_iDishFoodCount;

					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_POTATO;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_One"), &BossDishArg)))
						return;

				}
				else if (2 == m_iDishFoodCount)
				{
					m_iDishFoodCount = 0;

					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_CARROT;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_One"), &BossDishArg)))
						return;
				}


				break;
			}
			case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_SOUP:
			{
				_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_SOUP;

				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_Two"), &BossDishArg)))
					return;

				break;
			}
			case CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_COOKIE:
			{

				switch (m_iCookieFoodCount)
				{
				case 0:
				{
					++m_iCookieFoodCount;
					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_COOKIE_TWO;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_Three"), &BossDishArg)))
						return;

					break;
				}
				case 1:
				{
					++m_iCookieFoodCount;
					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_COOKIE_THREE;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_Three"), &BossDishArg)))
						return;

					break;
				}
				case 2:
				{
					++m_iCookieFoodCount;
					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_COOKIE_FOR;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_Three"), &BossDishArg)))
						return;

					break;
				}
				case 3:
				{
					m_iCookieFoodCount = 0;
					_uint BossDishArg = CBoss_Food_Projectile::FOOD_TYPE::FOOD_TYPE_COOKIE_ONE;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), SCENE_STAGE05, TEXT("Boss_Projectile_Three"), &BossDishArg)))
						return;

					break;
				}
				}
			}
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("FoodThrow.wav"), CHANNELID::PLAYER_RIGHT_FOOD_THROW)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}


			RELEASE_INSTANCE(CGameInstance);

			m_bFoodCreateCheck = true;
		}
	}
}

void CRight_Hand_Throw_Right::Free()
{
}
