#include "stdafx.h"
#include "..\public\Player_Food_Throw.h"
#include "Player_IdleAnim.h"

#include "Player_Throw_Left.h"
#include "Player_Throw_Right.h"
#include "Player_Dish_Throw.h"

#include "Boss_Dish.h"
#include "Boss_Goat.h"
#include "Enemy.h"
#include "Camera_Fly.h"


IMPLEMENT_SINGLETON(CPlayer_Food_Throw)

CPlayer_Food_Throw::CPlayer_Food_Throw()
{
}

void CPlayer_Food_Throw::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_iUpperIndex = 158;
	m_iRootIndex = 158;
	m_iBottomIndex = 158;

	Set_First_State(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Look_Lerp(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == Throw_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Food_Throw::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Food_Throw::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bFirstCheck = false;
	m_bIsLerpCheck = false;
}

void CPlayer_Food_Throw::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_eTrowType = THROW_TYPE_END;
	m_bFirstCheck = false;
	m_bIsLerpCheck = false;
}

void CPlayer_Food_Throw::Set_First_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if(false == m_bFirstCheck)
	{
		m_pLeftHand->Set_ChangeAnimCheck(true);
		m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_DISH_IDLE);
		m_pRightHand->Set_ChangeAnimCheck(true);
		m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_IDLE);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);


		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_BOSSDISH_THROW);

		CGameObject* pBossObject = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
		if (nullptr == pBossObject)
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);

		if (CBoss_Dish::BOSS_FOODTYPE_COOKIE == m_pPlayer->Get_DishType())
		{
			XMStoreFloat3(&m_vSourRotation,m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_vDestRotation, XMLoadFloat4(&*(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_F).m[3]) - m_pTransform->Get_State(CTransform::STATE_POSITION));
		}
		else if (CBoss_Dish::BOSS_FOODTYPE_SOUP == m_pPlayer->Get_DishType())
		{
			XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_vDestRotation, XMLoadFloat4(&*(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_H).m[3]) - m_pTransform->Get_State(CTransform::STATE_POSITION));
		}
		else if (CBoss_Dish::BOSS_FOODTYPE_DISH == m_pPlayer->Get_DishType())
		{
			XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_vDestRotation,XMLoadFloat4(&*(_float4*)dynamic_cast<CBoss_Goat*>(pBossObject)->Get_MouthMatrix(CBoss_Goat::BOSS_O).m[3]) - m_pTransform->Get_State(CTransform::STATE_POSITION));
		}

		m_vDestRotation.y = 0;

		m_iAttackCount = 15;
		m_bIsLerpCheck = true;
		m_bFirstCheck = true;
	}
}

void CPlayer_Food_Throw::Look_Lerp(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsLerpCheck)
	{
		m_lfTimeAcc += TimeDelta * 3.0;

		if (1.0 <= m_lfTimeAcc)
			m_lfTimeAcc = 1.0;

		_vector LerpLook = XMVectorLerp(XMLoadFloat3(&m_vSourRotation), XMLoadFloat3(&m_vDestRotation), (_float)m_lfTimeAcc);

		m_pTransform->Set_MyLook(LerpLook);

		if (1.0 <= m_lfTimeAcc)
		{
			m_lfTimeAcc = 0.0;
			m_bIsLerpCheck = false;
		}
	}

}

_bool CPlayer_Food_Throw::Throw_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (CLeft_Hand_Anim::LEFT_HAND_STATE_DISH_IDLE == m_pLeftHand->Get_HandState() &&
		CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_IDLE == m_pRightHand->Get_HandState())
	{
		for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
		{
			if ('T' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				if (1 <= m_iAttackCount)
				{
					if (THROW_TYPE_END == m_eTrowType)
					{

						m_eTrowType = THROW_TYPE_RIGHT;
						--m_iAttackCount;

						m_pLeftHand->Set_ChangeAnimCheck(true);
						m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_DISH_THROW);

						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_Throw_Left::Get_Instance());
						CPlayer_Throw_Left::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

						return true;
					}
					else if (THROW_TYPE_LEFT == m_eTrowType)
					{
						m_eTrowType = THROW_TYPE_RIGHT;
						--m_iAttackCount;

						m_pLeftHand->Set_ChangeAnimCheck(true);
						m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_DISH_THROW);

						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_Throw_Left::Get_Instance());
						CPlayer_Throw_Left::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
					else if (THROW_TYPE_RIGHT == m_eTrowType)
					{
						m_eTrowType = THROW_TYPE_LEFT;
						--m_iAttackCount;

						m_pRightHand->Set_ChangeAnimCheck(true);
						m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_THROW);

						pPlayer_AnimState->Set_AllLerpCheckTrue();
						pPlayer_AnimState->SetUp_State(CPlayer_Throw_Right::Get_Instance());
						CPlayer_Throw_Right::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
				}
				else
				{
					m_eTrowType = THROW_TYPE_END;
					pPlayer_AnimState->Set_AllLerpCheckTrue();
					Reset_Anim(pPlayer_AnimState);

					if (CBoss_Dish::BOSS_FOODTYPE_SOUP == m_pPlayer->Get_DishType())
					{
						m_pLeftHand->Set_ChangeAnimCheck(true);
						m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_DISH_FINISHED);

						m_pRightHand->Set_ChangeAnimCheck(true);
						m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_END);

						pPlayer_AnimState->SetUp_State(CPlayer_Dish_Throw::Get_Instance());
						CPlayer_Dish_Throw::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
					else if(CBoss_Dish::BOSS_FOODTYPE_DISH == m_pPlayer->Get_DishType())
					{
						m_pRightHand->Set_ChangeAnimCheck(true);
						m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_FINISIHED);

						m_pLeftHand->Set_ChangeAnimCheck(true);
						m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_DISHEND);

						pPlayer_AnimState->SetUp_State(CPlayer_Dish_Throw::Get_Instance());
						CPlayer_Dish_Throw::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
					else if (CBoss_Dish::BOSS_FOODTYPE_COOKIE == m_pPlayer->Get_DishType())
					{
						m_pLeftHand->Set_ChangeAnimCheck(true);
						m_pLeftHand->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_DISHEND);

						m_pRightHand->Set_ChangeAnimCheck(true);
						m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_DISH_END);

						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Safe_AddRef(pGameInstance);
						dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DishCheckType_On();
						CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);

						CGameObject* pFord = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Bosses(CBoss_Goat::BOSS::BOSS_F);
						dynamic_cast<CEnemy*>(pFord)->Damaged(200);

						Safe_Release(pGameInstance);

						pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
						CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
						return true;
					}
				}
			}
			if (VK_LSHIFT == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				m_eTrowType = THROW_TYPE_END;
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				Reset_Anim(pPlayer_AnimState);

				pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
				CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
	}



	return false;
}

void CPlayer_Food_Throw::Free()
{
}

