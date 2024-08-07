#include "stdafx.h"
#include "../public/Right_Hand_ChopB.h"
#include "Right_Hand_Default.h"

IMPLEMENT_SINGLETON(CRight_Hand_ChopB)

CRight_Hand_ChopB::CRight_Hand_ChopB()
{
}

void CRight_Hand_ChopB::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_iAnimIndex = Chop_B;
	pRightHandAnim->Set_RenderCheck(true);

	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CRight_Hand_ChopB::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta * 2.f);

	_uint i = m_pModelCom->Get_AnimationCurrentFrame(Chop_B);
	if (4 >= i)
		pRightHandAnim->Set_TimeAcc((_double)i / 4.0);	
	else if (9 <= i)
		pRightHandAnim->Set_TimeAcc(1.0 - ((_double)(i - 9) / 4.0));

	if (true == m_pModelCom->Get_IsAnimFinishied(Chop_B))
	{
		pRightHandAnim->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_END);
		pRightHandAnim->Set_TimeAcc(1.0);
		pRightHandAnim->SetUp_State(CRight_Hand_Default::Get_Instance());
		CRight_Hand_Default::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
		return;
	}
}

void CRight_Hand_ChopB::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
}

void CRight_Hand_ChopB::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}

void CRight_Hand_ChopB::Free()
{
}
