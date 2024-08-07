#include "stdafx.h"
#include "..\public\Right_Hand_Throw_Finished.h"
#include "Right_Hand_Default.h"
#include "Right_Hand_Throw_Right.h"
#include "Player.h"
#include "Boss_Dish.h"

IMPLEMENT_SINGLETON(CRight_Hand_Throw_Finished)

CRight_Hand_Throw_Finished::CRight_Hand_Throw_Finished()
{
}

void CRight_Hand_Throw_Finished::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	First_MotionState_Cheking(pRightHandAnim, pRightHandDesc, TimeDelta);

	pRightHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CRight_Hand_Throw_Finished::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);

	if (false == m_IsDishGrab)
	{
		if (18 <= m_pModelCom->Get_AnimationCurrentFrame(Right_Throw_Finished) && 45 > m_pModelCom->Get_AnimationCurrentFrame(Right_Throw_Finished))
			pRightHandAnim->Set_Dish_Grab(true);
	}


	if (45 <= m_pModelCom->Get_AnimationCurrentFrame(Right_Throw_Finished))
	{
		if (false == m_bSoundCheck)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("BossDishThrwoing.wav"), CHANNELID::PLAYER_DISH_THROW)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			RELEASE_INSTANCE(CGameInstance);

			m_bSoundCheck = true;
		}
		pRightHandAnim->Set_Dish_Grab(false);
		pRightHandAnim->Set_Dish_Throw(true);

		m_IsDishGrab = true;
	}


	if (true == m_pModelCom->Get_IsAnimFinishied(Right_Throw_Finished))
	{
		Reset_Anim(pRightHandAnim, pRightHandDesc, TimeDelta);
		pRightHandAnim->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_END);
		pRightHandAnim->SetUp_State(CRight_Hand_Default::Get_Instance());
		CRight_Hand_Default::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
		return;
	}
}

void CRight_Hand_Throw_Finished::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	pRightHandAnim->Set_Dish_Grab(false);
	pRightHandAnim->Set_Dish_Throw(false);
	m_iAnimIndex = Right_Throw_Finished;

	m_bSoundCheck = true;
	m_IsDishGrab = false;
	m_bFirstCheck = false;
}

void CRight_Hand_Throw_Finished::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}

void CRight_Hand_Throw_Finished::First_MotionState_Cheking(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iAnimIndex = Right_Throw_Finished;
		m_bFirstCheck = true;
	}
}

void CRight_Hand_Throw_Finished::Free()
{
}
