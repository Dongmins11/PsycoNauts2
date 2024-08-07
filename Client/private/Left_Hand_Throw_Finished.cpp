#include "stdafx.h"
#include "..\public\Left_Hand_Throw_Finished.h"
#include "Left_Hand_Default.h"
#include "Left_Hand_Throw_Left.h"
#include "Player.h"
#include "Boss_Dish.h"

IMPLEMENT_SINGLETON(CLeft_Hand_Throw_Finished)

CLeft_Hand_Throw_Finished::CLeft_Hand_Throw_Finished()
{
}

void CLeft_Hand_Throw_Finished::HandleInput(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	First_MotionState_Cheking(pLeftHandAnim, pLeftHandDesc, TimeDelta);

	pLeftHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CLeft_Hand_Throw_Finished::Update(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);

	if (false == m_IsDishGrab)
	{
		if (18 <= m_pModelCom->Get_AnimationCurrentFrame(Left_Throw_Finished) && 45 > m_pModelCom->Get_AnimationCurrentFrame(Left_Throw_Finished))
			pLeftHandAnim->Set_Dish_Grab(true);
	}


	if (45 <= m_pModelCom->Get_AnimationCurrentFrame(Left_Throw_Finished))
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

		pLeftHandAnim->Set_Dish_Grab(false);
		pLeftHandAnim->Set_Dish_Throw(true);
		m_IsDishGrab = true;
	}


	if (true == m_pModelCom->Get_IsAnimFinishied(Left_Throw_Finished))
	{
		Reset_Anim(pLeftHandAnim, pLeftHandDesc, TimeDelta);
		pLeftHandAnim->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_END);
		pLeftHandAnim->SetUp_State(CLeft_Hand_Default::Get_Instance());
		CLeft_Hand_Default::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
		return;
	}
}

void CLeft_Hand_Throw_Finished::Reset_Anim(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	pLeftHandAnim->Set_Dish_Grab(false);
	pLeftHandAnim->Set_Dish_Throw(false);
	m_iAnimIndex = Left_Throw_Finished;

	m_bSoundCheck = false;
	m_IsDishGrab = false;
	m_bFirstCheck = false;
}

void CLeft_Hand_Throw_Finished::Set_Colider(CLeft_Hand_Anim* pLeftHandAnim)
{
}

void CLeft_Hand_Throw_Finished::First_MotionState_Cheking(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iAnimIndex = Left_Throw_Finished;
		m_bFirstCheck = true;
	}
}

void CLeft_Hand_Throw_Finished::Free()
{
}
