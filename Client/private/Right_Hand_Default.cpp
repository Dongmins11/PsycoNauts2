#include "stdafx.h"
#include "..\public\Right_Hand_Default.h"
#include "Right_Hand_ChopB.h"
#include "Right_Hand_Grap.h"
#include "Right_Hand_Throw_Idle.h"
#include "PullingGrab.h"

IMPLEMENT_SINGLETON(CRight_Hand_Default)

CRight_Hand_Default::CRight_Hand_Default()
{
}

void CRight_Hand_Default::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	pRightHandAnim->Set_RenderCheck(false);


	if (true == pRightHandAnim->Get_ChangeCheck())
	{
		switch (pRightHandAnim->Get_Right_HandState())
		{
		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_CHOPB:
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CRight_Hand_ChopB::Get_Instance());
			CRight_Hand_ChopB::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;

		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_GRAB:
			pRightHandAnim->Set_BoneCaculationCheck(true);
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CRight_Hand_Grap::Get_Instance());
			CRight_Hand_Grap::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;

		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_GRAB_PULLING:
			pRightHandAnim->Set_BoneCaculationCheck(true);
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CPullingGrab::Get_Instance());
			CPullingGrab::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;

		case CRight_Hand_Anim::RIGHT_HAND_STATE::RIGHT_HAND_STATE_DISH_IDLE:
			pRightHandAnim->Set_ChangeCheck(false);
			pRightHandAnim->SetUp_State(CRight_Hand_Throw_Idle::Get_Instance());
			CRight_Hand_Throw_Idle::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
			break;
		}
	}
}

void CRight_Hand_Default::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
}

void CRight_Hand_Default::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
}

void CRight_Hand_Default::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}

void CRight_Hand_Default::Free()
{
}
