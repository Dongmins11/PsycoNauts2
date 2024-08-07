#include "stdafx.h"
#include "..\public\Left_Hand_Default.h"
#include "Left_Hand_ChopA.h"
#include "Left_Hand_ChopC.h"
#include "Left_Hand_Throw_Idle.h"

IMPLEMENT_SINGLETON(CLeft_Hand_Default)

CLeft_Hand_Default::CLeft_Hand_Default()
{
}

void CLeft_Hand_Default::HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	pLeftHandAnim->Set_RenderCheck(false);
	

	if (true == pLeftHandAnim->Get_ChangeCheck())
	{
		switch (pLeftHandAnim->Get_Left_HandState())
		{
		case CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_CHOPA:
			pLeftHandAnim->Set_ChangeCheck(false);
			pLeftHandAnim->SetUp_State(CLeft_Hand_ChopA::Get_Instance());
			CLeft_Hand_ChopA::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			break;
		case CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_CHOPC:
			pLeftHandAnim->Set_ChangeCheck(false);
			pLeftHandAnim->SetUp_State(CLeft_Hand_ChopC::Get_Instance());
			CLeft_Hand_ChopC::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			break;
		case CLeft_Hand_Anim::LEFT_HAND_STATE::LEFT_HAND_STATE_DISH_IDLE:
			pLeftHandAnim->Set_ChangeCheck(false);
			pLeftHandAnim->SetUp_State(CLeft_Hand_Throw_Idle::Get_Instance());
			CLeft_Hand_Throw_Idle::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
			return;
		}
	}
}

void CLeft_Hand_Default::Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
}

void CLeft_Hand_Default::Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
}

void CLeft_Hand_Default::Set_Colider(CLeft_Hand_Anim * pLeftHandAnim)
{
}

void CLeft_Hand_Default::Free()
{
}
