#include "stdafx.h"
#include "..\public\Player_CloudAnim.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Player_CloudAnim_End.h"
#include "Cloud.h"
#include "QuestManager.h"

IMPLEMENT_SINGLETON(CPlayer_CloudAnim)

CPlayer_CloudAnim::CPlayer_CloudAnim()
{
}

void CPlayer_CloudAnim::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;

	First_ChangeMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	/* Animation 슈윳~~ 해줘 */

	if (m_bLookLurp[LOOK_LURP_START]) /*Look 보간 */
	{
		Look_Timer += TimeDelta;

		if (1.0 <= Look_Timer)
			Look_Timer = 1.0;

		_vector LurpLook = XMVectorLerp(XMLoadFloat4(&m_vUpStart), XMLoadFloat4(&m_vUpEnd), Look_Timer);
		m_pTransform->Set_MyUp(LurpLook);

		if (1.0 <= Look_Timer)
		{
			Look_Timer = 0.0;
			m_bLookLurp[LOOK_LURP_START] = false;
			m_bLookLurp[LOOK_LURP_ENDING] = true;
		}
	}



	if (PLAYER_INNODE_START == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_INNODE_START))
	{
		m_iUpperIndex = PLAYER_INNODE_LOOP;
		m_iRootIndex = PLAYER_INNODE_LOOP;
		m_iBottomIndex = PLAYER_INNODE_LOOP;
	}


	if (PLAYER_INNODE_LOOP == m_iRootIndex)
	{
		if (m_bLerpCheck)
			Player_MovePostion(pPlayer_AnimState, TimeDelta);
		else
		{
			m_iUpperIndex = PLAYER_INNODE_ARRIVE;
			m_iRootIndex = PLAYER_INNODE_ARRIVE;
			m_iBottomIndex = PLAYER_INNODE_ARRIVE;


		}
	}


	if (PLAYER_INNODE_ARRIVE == m_iRootIndex)
	{
		//보간진행
		if (m_bLookLurp[LOOK_LURP_ENDING])
		{
			Look_Timer += TimeDelta * 2.0;
			if (1.0 <= Look_Timer)
				Look_Timer = 1.0;

			_vector LurpLook = XMVectorLerp(XMLoadFloat4(&m_vUpEnd), XMVectorSet(0.f, 1.f, 0.f, 0.f), Look_Timer);
			m_pTransform->Set_MyUp(LurpLook);

			if (1.0 <= Look_Timer)
			{
				Look_Timer = 0.0;
				m_bLookLurp[LOOK_LURP_ENDING] = false;
			}
		}

		if (false == m_bLookLurp[LOOK_LURP_ENDING] && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_INNODE_ARRIVE))
		{
			if (true == Player_CloudAnim_To_Player_CloudAnim_End(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
			{
				return;
			}
		}
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_CloudAnim::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_CloudAnim::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_bLookLurp[LOOK_LURP_START] = true; /* 시작 룩 보간 할지 */
	m_bLookLurp[LOOK_LURP_ENDING] = false; /* 끝 룩 보간 할지 */

	m_Move[MOVESTATE_START_POSTION] = _float4(0.f, 0.f, 0.f, 0.f);
	m_Move[MOVESTATE_END_POSTION] = _float4(0.f, 0.f, 0.f, 0.f);

	//m_iUpperIndex = 0;
	//m_iRootIndex = 0;
	//m_iBottomIndex = 0;

	pPlayer_AnimState->Set_CloudObject(nullptr);
	pPlayer_AnimState->Set_AllLerpCheckTrue();

	Look_Timer = 0.0;
	AccTimer = 0.0;
	m_MoveTimer = 0.0;

	m_bLerpCheck = true;

	m_bFristCheck = false;
};


void CPlayer_CloudAnim::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_CloudAnim::First_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFristCheck)
	{
		CQuestManager::Get_Instance()->Clear_Quest(0);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(80.f, 1.f);
		
		pGameInstance->StopSound(CHANNELID::PLAYER_MOVESKILLE_ONE);

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("CloudMove.wav"), CHANNELID::PLAYER_MOVESKILLE_ONE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_iUpperIndex = PLAYER_INNODE_START;
		m_iRootIndex = PLAYER_INNODE_START;
		m_iBottomIndex = PLAYER_INNODE_START;

		XMStoreFloat4(&m_Move[MOVESTATE_START_POSTION], m_pTransform->Get_Position());

		CCloud* pTarget = (CCloud*)pPlayer_AnimState->Get_CloudObject();
		if (nullptr != pTarget)
		{
			XMStoreFloat4(&m_Move[MOVESTATE_END_POSTION], pTarget->Get_CloudPos()); /* 좌표를 받아 내고 */
		}

		XMStoreFloat4(&m_vUpStart, m_pTransform->Get_State(CTransform::STATE_UP));
		XMStoreFloat4(&m_vUpEnd, XMLoadFloat4(&m_Move[MOVESTATE_END_POSTION]) - XMLoadFloat4(&m_Move[MOVESTATE_START_POSTION]));

		m_bLookLurp[LOOK_LURP_START] = true;
		m_bLookLurp[LOOK_LURP_ENDING] = false;

		pPlayer_AnimState->Set_CloudObject(nullptr); /* 초기화 입니다. */

		AccTimer = 0.0;
		m_MoveTimer = 0.0;
		Look_Timer = 0.0;

		m_bLerpCheck = true;
		m_bFristCheck = true;
	}
}

void CPlayer_CloudAnim::Enter_Motion_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
}

_bool CPlayer_CloudAnim::Player_CloudAnim_To_Player_CloudAnim_End(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bLerpCheck)
	{
		Reset_Anim(pPlayer_AnimState);
		m_pTransform->Set_Velocity(XMVectorZero());
		//만약필요한경우에만
		pPlayer_AnimState->SetUp_State(CPlayer_CloudAnim_End::Get_Instance());
		CPlayer_CloudAnim_End::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

void CPlayer_CloudAnim::Player_MovePostion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	m_MoveTimer += TimeDelta + AccTimer;

	if (1.0 <= m_MoveTimer)
		m_MoveTimer = 1.0;

	_float3 vPlayerNewPostion;
	XMStoreFloat3(&vPlayerNewPostion, XMVectorLerp(XMLoadFloat4(&m_Move[MOVESTATE_START_POSTION]), XMLoadFloat4(&m_Move[MOVESTATE_END_POSTION]), (_float)m_MoveTimer));
	m_pCharacterController->Set_FootPosition(vPlayerNewPostion);
	AccTimer += 0.002;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
	if (nullptr == pRenderer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, true);
	RELEASE_INSTANCE(CGameInstance);

	if (1.0 <= m_MoveTimer)
	{
		AccTimer = 0.0;
		m_MoveTimer = 0.0;
		m_bLerpCheck = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
		if (nullptr == pRenderer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, false);

		pGameInstance->StopSound(CHANNELID::PLAYER_MOVESKILLE_TWO);

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("CloudMoveTwo.wav"), CHANNELID::PLAYER_MOVESKILLE_TWO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer_CloudAnim::Free()
{
}
