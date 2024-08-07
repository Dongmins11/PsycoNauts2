#include "stdafx.h"
#include "../public/Right_Hand_Grap.h"
#include "Right_Hand_Default.h"

IMPLEMENT_SINGLETON(CRight_Hand_Grap)

CRight_Hand_Grap::CRight_Hand_Grap()
{
}

void CRight_Hand_Grap::HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		SetUp_BoneTargetMatrix("goggles1");
		m_iAnimIndex = Grab;
		pRightHandAnim->Set_BoneCaculationCheck(true);
		pRightHandAnim->Set_RenderCheck(true);
		m_bFirstCheck = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_GrapRect"),
			SCENE_STATIC, TEXT("Effect_Repeat_GrapRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransformCom, TEXT("Layer_Repeat_GrapRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Grab.wav"), CHANNELID::PLAYER_RIGHT_HAND_GRAB)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}


	if (true == pRightHandAnim->Get_ChangeCheck())
	{
		pRightHandAnim->Set_ChangeCheck(false);

		if (CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB_ATTACK == pRightHandAnim->Get_Right_HandState())
		{
			pRightHandAnim->Set_LerpTrue();
			m_iAnimIndex = Toss;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			
			_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_GrapRect"));
			if (0 < iSize)
			{
				if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_GrapRect"), 0)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Throw.wav"), CHANNELID::PLAYER_RIGHT_HAND_THROW)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}


	if (m_iAnimIndex == Toss && true == m_pModelCom->Get_IsAnimFinishied(Toss))
	{
		pRightHandAnim->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_END);
		pRightHandAnim->Set_BoneCaculationCheck(false);
		Reset_Anim(pRightHandAnim, pRightHandDesc, TimeDelta);
		pRightHandAnim->SetUp_State(CRight_Hand_Default::Get_Instance());
		CRight_Hand_Default::Get_Instance()->HandleInput(pRightHandAnim, pRightHandDesc, TimeDelta);
		return;
	}


	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CRight_Hand_Grap::Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	if (Grab == m_pModelCom->Get_AnimIndex() && 10 <= m_pModelCom->Get_AnimationCurrentFrame(Grab) && false == m_bGrabFinishied)
	{
		m_pModelCom->Set_AnimSpeed(Grab, 0.0);
		m_bGrabFinishied = true;
	}

	if (true == m_bGrabFinishied)
		Lerp_Hand(pRightHandAnim, pRightHandDesc, TimeDelta);


	m_pModelCom->Lerp_Update(TimeDelta * 2.f);
}

void CRight_Hand_Grap::Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	m_pModelCom->Set_AnimSpeed(Grab, 1.0);
	m_bFirstCheck = false;
	m_bGrabFinishied = false;
}

void CRight_Hand_Grap::Set_Colider(CRight_Hand_Anim * pRightHandAnim)
{
}

void CRight_Hand_Grap::Lerp_Hand(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta)
{
	Bone_MatrixCalculation(TimeDelta);
}

void CRight_Hand_Grap::Free()
{
}
