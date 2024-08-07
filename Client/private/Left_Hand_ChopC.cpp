#include "stdafx.h"
#include "..\public\Left_Hand_ChopC.h"
#include "Left_Hand_Default.h"

IMPLEMENT_SINGLETON(CLeft_Hand_ChopC)

CLeft_Hand_ChopC::CLeft_Hand_ChopC()
{
}

void CLeft_Hand_ChopC::HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	m_iAnimIndex = Chop_C;
	pLeftHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CLeft_Hand_ChopC::Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta * 2.f);

	_uint i = m_pModelCom->Get_AnimationCurrentFrame(Chop_C);
	if (9 >= i)
		pLeftHandAnim->Set_TimeAcc(i / 9.0);
	else if (15 <= i)
		pLeftHandAnim->Set_TimeAcc(1.0 - (_double(i - 15) / 9.0));
	else if (24 < i)
		pLeftHandAnim->Set_TimeAcc(0.0);

	if (true == m_pModelCom->Get_IsAnimFinishied(Chop_C))
	{
		pLeftHandAnim->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_END);
		pLeftHandAnim->Set_TimeAcc(1.0);
		pLeftHandAnim->SetUp_State(CLeft_Hand_Default::Get_Instance());
		CLeft_Hand_Default::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
		return;
	}
}

void CLeft_Hand_ChopC::Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
}

void CLeft_Hand_ChopC::Set_Colider(CLeft_Hand_Anim * pLeftHandAnim)
{
}

void CLeft_Hand_ChopC::Free()
{
}
