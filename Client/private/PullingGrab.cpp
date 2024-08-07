#include "stdafx.h"
#include "../public/PullingGrab.h"
#include "Right_Hand_Default.h"

IMPLEMENT_SINGLETON(CPullingGrab)

CPullingGrab::CPullingGrab()
{
}

void CPullingGrab::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iAnimIndex = Grab_Fulling;
		pRightHandAnim->Set_BoneCaculationCheck(true);
		pRightHandAnim->Set_RenderCheck(true);
		m_bFirstCheck = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("GrabPulling.wav"), CHANNELID::PLAYER_RIGHT_HAND_GRAB)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}


	if (Grab_Fulling == m_iAnimIndex && true == m_pModelCom->Get_IsAnimFinishied(Grab_Fulling))
		m_iAnimIndex = Grab_FullingLoop;



	if (true == pRightHandAnim->Get_PullingSuccessCheck())
	{
		//  ÀÌÆåÆ®Ãß°¡ //
		pRightHandAnim->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_END);
		pRightHandAnim->Set_BoneCaculationCheck(false);
		Reset_Anim(pRightHandAnim, pRightHandDesc, TimeDelta);
		pRightHandAnim->SetUp_State(CRight_Hand_Default::Get_Instance());
		CRight_Hand_Default::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
		return;
	}


	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CPullingGrab::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta * 2.f);
}

void CPullingGrab::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_bFirstCheck = false;
	m_bGrabFinishied = false;
}

void CPullingGrab::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}


void CPullingGrab::Free()
{
}
