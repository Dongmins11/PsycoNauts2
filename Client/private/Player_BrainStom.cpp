#include "stdafx.h"
#include "..\public\Player_BrainStom.h"
#include "Player_IdleAnim.h"
#include "Skill_BrainStrom.h"
#include "Skill_UI.h"


IMPLEMENT_SINGLETON(CPlayer_BrainStom)

CPlayer_BrainStom::CPlayer_BrainStom()
{
}

void CPlayer_BrainStom::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
		Animation_BlendSpeedSet(pPlayer_AnimState, TimeDelta);


	_uint iMoveCount = 0;
	_uint iStopCount = 0;

	_bool bMoveCheck = false;

	if (false == m_bEndCheck)
	{
		for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
		{
			if ('Q' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				//루프돌때
				m_iUpperIndex = PLAYER_BRAIN_FIRE_LOOP;
				m_iRootIndex = PLAYER_IDLE;
				m_iBottomIndex = PLAYER_IDLE;
				m_bKeyCheck = true;

				for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
				{
					if ('W' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING ||
						'S' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING ||
						'A' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING ||
						'D' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING)
					{
						m_iUpperIndex = PLAYER_BRAIN_FIRE_LOOP;
						m_iRootIndex = PLAYER_RUN;
						m_iBottomIndex = PLAYER_RUN;
						m_bRootHoldCheck = true;
						bMoveCheck = true;

						++iMoveCount;
					}
					if ('W' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_UP ||
						'S' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_UP ||
						'A' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_UP ||
						'D' == pAnimKeyDesc.iKey[j] && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_UP)
					{
						++iStopCount;
					}
				}
			}
		}
	}

	if (true == m_bKeyCheck)
		m_lfAnimSpeed += TimeDelta;

	if ('Q' == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_UP)
	{
		//캐스팅 할때
		if (m_iUpperIndex != PLAYER_BRAIN_FIRE_CASTING)
		{
			m_iUpperIndex = PLAYER_BRAIN_FIRE_CASTING;
			m_iRootIndex = PLAYER_BRAIN_FIRE_CASTING;
			m_iBottomIndex = PLAYER_BRAIN_FIRE_CASTING;

			m_bEndCheck = true;

			CGameInstance* pGameInstace = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstace);
			
			_uint iCurrentScene = pGameInstace->Get_CurrentSceneIndex();

			CSkill_UI* pBrainStorm_UI = dynamic_cast<CSkill_UI*>(pGameInstace->Get_GameObject(iCurrentScene, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pBrainStorm_UI)
			{
				Safe_Release(pGameInstace);
				return;
			}
			pBrainStorm_UI->Set_BrainStorm();
			//스킬 ui 얻어와서 셋시켜줄거 하나시켜주면 스킬ui 내부적으로 연산이들어가게하면 되지않을까요?>

			
			pGameInstace->StopSound(PLAYER_SKILLE);

			if (FAILED(pGameInstace->PlaySoundOnce(TEXT("BrainStrom.wav"), CHANNELID::PLAYER_SKILLE)))
			{
				Safe_Release(pGameInstace);
				return;
			}
			Safe_Release(pGameInstace);

			if (nullptr != m_pEffectObject)
			{
				dynamic_cast<CSkill_BrainStrom*>(m_pEffectObject)->Set_DeadCheck(true);
				m_pEffectObject = nullptr;
			}

			//Safe_Release(m_pEffectObject);
		}
	}

	if (PLAYER_BRAIN_FIRE_CASTING == m_iRootIndex && m_pModel->Get_IsUpperAnimFinishied(PLAYER_BRAIN_FIRE_CASTING))
	{
		Reset_Anim(pPlayer_AnimState);
		ZeroMemory(&pAnimKeyDesc, sizeof(CPlayer_AnimState::ANIMDESC));
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	if (m_iRootPreIndex != m_iRootIndex)
	{
		pPlayer_AnimState->Set_RootBottomLerpCheckTrue();
		m_iRootPreIndex = m_iRootIndex;
	}


	m_bIsMoveOn = bMoveCheck;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_BrainStom::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		m_bRootHoldCheck = true;
		Handle_Movement(pAnimKeyDesc);
		Player_Move_NonMoveOffset(TimeDelta);
	}
	else
		m_bRootHoldCheck = false;

	m_pModel->Root_Update(TimeDelta);
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_BrainStom::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN, 10.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN, 10.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN, 10.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_IDLE, 10.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_IDLE, 10.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_IDLE, 10.f);

	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bKeyCheck = false;
	m_bEndCheck = false;
	m_bFirstCheck = false;
	m_lfAnimSpeed = 0.0;
}

void CPlayer_BrainStom::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN, 10.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN, 10.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN, 10.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_IDLE, 10.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_IDLE, 10.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_IDLE, 10.f);

	m_bKeyCheck = false;
	m_bEndCheck = false;
	m_bFirstCheck = false;
	m_lfAnimSpeed = 0.0;

	if (nullptr != m_pEffectObject)
	{
		dynamic_cast<CSkill_BrainStrom*>(m_pEffectObject)->Set_DeadCheck(true);
		m_pEffectObject = nullptr;
	}
}

void CPlayer_BrainStom::Animation_BlendSpeedSet(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_BRAIN_FIRE_CASTING, 6.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_BRAIN_FIRE_CASTING, 6.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_BRAIN_FIRE_CASTING, 6.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_IDLE, 6.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_IDLE, 6.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_IDLE, 6.f);

	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_RUN, 6.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_RUN, 6.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_RUN, 6.f);


	Create_Brain_Strom_Effect(pPlayer_AnimState, TimeDelta);

	m_bFirstCheck = true;
}

void CPlayer_BrainStom::Create_Brain_Strom_Effect(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_BrainStrom"), m_iScene, TEXT("Layer_BrainStrom"), (CGameObject**)&m_pEffectObject, nullptr)))
		return;

	Safe_Release(pGameInstance);
}

void CPlayer_BrainStom::Free()
{
}

