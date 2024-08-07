#include "stdafx.h"
#include "..\public\Player_Pisblast.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Skill_UI.h"

IMPLEMENT_SINGLETON(CPlayer_Pisblast)

CPlayer_Pisblast::CPlayer_Pisblast()
{
}

void CPlayer_Pisblast::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->StopSound(CHANNELID::PLAYER_SKILLE_ONE))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PsiBlast.wav"), CHANNELID::PLAYER_SKILLE_ONE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_bFirstCheck = true;
	}
	First_KeyCheck_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);


	if (PLAYER_PSIBLAST_QUICK_FIRE == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_PSIBLAST_QUICK_FIRE))
		m_bKeyCheck = true;


	//Charge_Move_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);


	if (pAnimKeyDesc.iOnceKey == 'E' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_UP && true == m_bChargeCheck)
	{
		m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_FIRE;
		m_iRootIndex = PLAYER_PSIBLAST_CHARGE_FIRE;
		m_iBottomIndex = PLAYER_PSIBLAST_CHARGE_FIRE;
		m_bEndCheck = true;
	}

	if (true == m_bKeyCheck)
	{
		if (m_iRootPreIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreIndex = m_iRootIndex;
		}
	}



	if (true == Quick_Fire_to_RunMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Fire_To_Idle_Motion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;



	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Pisblast::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_iRootIndex == PLAYER_BALL_RUN_RIGHT || m_iRootIndex == PLAYER_BALL_RUN_LEFT)
	{
		m_iCount = 0;
		m_bRootHoldCheck = true;
	}
	else
	{
		if (true == m_bRootHoldCheck)
		{
			++m_iCount;
			if (6 == m_iCount)
				m_bRootHoldCheck = false;
		}
	}

	Cam_To_Look_At(TimeDelta);

	Create_BlastBullet();


	m_pModel->Root_Update(TimeDelta, m_bRootHoldCheck);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Pisblast::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iRootPreIndex = 0;
	m_fMoveOffset = 0.f;
	m_bFirstRotationCheck = false;
	m_bCreateCheck = false;
	m_bEndCheck = false;
	m_bChargeCheck = false;
	m_bKeyCheck = false;
	m_bFirstCheck = false;
	m_eCurrentLookState = LOOK_END;

	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Pisblast::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iRootPreIndex = 0;
	m_fMoveOffset = 0.f;
	m_bFirstRotationCheck = false;
	m_bCreateCheck = false;
	m_bEndCheck = false;
	m_bChargeCheck = false;
	m_bKeyCheck = false;
	m_bFirstCheck = false;
	m_eCurrentLookState = LOOK_END;
}

_bool CPlayer_Pisblast::Move_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool	MoveCheck = false;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			MoveCheck = true;
		}
	}

	m_bIsMoveOn = MoveCheck;

	return m_bIsMoveOn;
}

void CPlayer_Pisblast::First_KeyCheck_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (pAnimKeyDesc.iOnceKey == 'E' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_DOWN && false == m_bKeyCheck)
	{
		if (PLAYER_PSIBLAST_QUICK_FIRE != m_iRootIndex && false == m_bChargeCheck)
		{
			m_iUpperIndex = PLAYER_PSIBLAST_QUICK_FIRE;
			m_iRootIndex = PLAYER_PSIBLAST_QUICK_FIRE;
			m_iBottomIndex = PLAYER_PSIBLAST_QUICK_FIRE;
		}
	}
}

void CPlayer_Pisblast::Create_BlastBullet()
{
	if (false == m_bCreateCheck)
	{
		if (PLAYER_PSIBLAST_QUICK_FIRE == m_iUpperIndex && 3 <= m_pModel->Get_UpperAnimationCurrentFrame(PLAYER_PSIBLAST_QUICK_FIRE))
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_PisBlast"), m_iScene, TEXT("Layer_PisBlast"))))
				return;

			m_bCreateCheck = true;

			_uint iCurrentScene = pGameInstance->Get_CurrentSceneIndex();

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(iCurrentScene, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				Safe_Release(pGameInstance);
				return;

			}
			pSkill_UI->Set_Pisblast();
			//스킬 ui 얻어와서 셋시켜줄거 하나시켜주면 스킬ui 내부적으로 연산이들어가게하면 되지않을까요?>

			/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PSI_Impact"),
				SCENE_STATIC, TEXT("Effect_Impact"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, false, m_pTransform, TEXT("Layer_PSI_Impact"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			_vector vPos = m_pTransform->Get_State(CTransform::STATE_UP) + m_pTransform->Get_State(CTransform::STATE_LOOK); 
			pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PSI_Impact"), TEXT("Com_Option"), vPos);

			Safe_Release(pGameInstance);
		}
	}
}

void CPlayer_Pisblast::Cam_To_Look_At(_double TimeDelta)
{
	if (PLAYER_PSIBLAST_QUICK_FIRE == m_iUpperIndex)
		SetUp_Cam_to_LookAt(TimeDelta);

}

_bool CPlayer_Pisblast::Quick_Fire_to_RunMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == Move_Checking(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	{
		if (PLAYER_PSIBLAST_QUICK_FIRE == m_iUpperIndex && 20 <= m_pModel->Get_UpperAnimationCurrentFrame(PLAYER_PSIBLAST_QUICK_FIRE))
		{
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_RunAnim::Get_Instance());
			CPlayer_RunAnim::Get_Instance()->Set_MoveOffset_ChangeCheck(true);
			CPlayer_RunAnim::Get_Instance()->Set_PlayerMoveAccelecrtaion(1.f);
			//CPlayer_RunAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

_bool CPlayer_Pisblast::Fire_To_Idle_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bChargeCheck && true == m_bKeyCheck)
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	if (PLAYER_PSIBLAST_CHARGE_FIRE == m_iRootIndex && m_pModel->Get_IsUpperAnimFinishied(PLAYER_PSIBLAST_CHARGE_FIRE))
	{
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}


	return false;
}

void CPlayer_Pisblast::Charge_Move_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bKeyCheck && false == m_bEndCheck && false == m_pModel->Get_IsRootLerp())
	{
		for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
		{
			if (pAnimKeyDesc.iKey[i] == 'E' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_PRESING)
			{
				m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
				m_iRootIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
				m_iBottomIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
				m_bChargeCheck = true;

				for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
				{
					if (pAnimKeyDesc.iKey[j] == 'A' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_PRESING)
					{
						m_pTransform->Go_Left(TimeDelta * 0.5, m_pCharacterController);

						m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_LEFT;
						m_iRootIndex = PLAYER_BALL_RUN_LEFT;
						m_iBottomIndex = PLAYER_BALL_RUN_LEFT;
					}
					else if (pAnimKeyDesc.iKey[j] == 'A' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_UP)
					{

						m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
						m_iRootIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
						m_iBottomIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
					}
					else if (pAnimKeyDesc.iKey[j] == 'D' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_PRESING)
					{
						m_pTransform->Go_Right(TimeDelta * 0.5, m_pCharacterController);

						m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_RIGHT;
						m_iRootIndex = PLAYER_BALL_RUN_RIGHT;
						m_iBottomIndex = PLAYER_BALL_RUN_RIGHT;
					}
					else if (pAnimKeyDesc.iKey[j] == 'D' && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_UP)
					{
						m_iUpperIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
						m_iRootIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
						m_iBottomIndex = PLAYER_PSIBLAST_CHARGE_LOOP;
					}
				}
			}
		}
	}
}

void CPlayer_Pisblast::Free()
{
}

