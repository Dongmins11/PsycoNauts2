#include "stdafx.h"
#include "..\public\Left_Hand_ChopA.h"
#include "Left_Hand_Default.h"

IMPLEMENT_SINGLETON(CLeft_Hand_ChopA)

CLeft_Hand_ChopA::CLeft_Hand_ChopA()
{
}

void CLeft_Hand_ChopA::HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	m_iAnimIndex = Chop_A;
	pLeftHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CLeft_Hand_ChopA::Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);
	
	_uint i = m_pModelCom->Get_AnimationCurrentFrame(Chop_A);
	if (3 >= i)
		pLeftHandAnim->Set_TimeAcc(i * 0.33);	
	else if (7 <= i)
		pLeftHandAnim->Set_TimeAcc(1.0 - ((i - 7) * 0.33));

	if (true == m_pModelCom->Get_IsAnimFinishied(Chop_A))
	{
		pLeftHandAnim->Set_HandState(CLeft_Hand_Anim::LEFT_HAND_STATE_END);
		pLeftHandAnim->Set_TimeAcc(1.0);
		pLeftHandAnim->SetUp_State(CLeft_Hand_Default::Get_Instance());
		CLeft_Hand_Default::Get_Instance()->HandleInput(pLeftHandAnim, pLeftHandDesc, TimeDelta);
		return;
	}
}

void CLeft_Hand_ChopA::Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta)
{
}

void CLeft_Hand_ChopA::Set_Colider(CLeft_Hand_Anim * pLeftHandAnim)
{
}

void CLeft_Hand_ChopA::Free()
{
}
