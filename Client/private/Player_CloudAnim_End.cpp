#include "stdafx.h"
#include "..\public\Player_CloudAnim_End.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Cloud.h"
#include "Player_CloudAnim.h"
//#include "Player_AnimState.h"


IMPLEMENT_SINGLETON(CPlayer_CloudAnim_End)

CPlayer_CloudAnim_End::CPlayer_CloudAnim_End()
{
}


void CPlayer_CloudAnim_End::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;

	_bool bJump = false;
	_bool bNext_CloudCheck= false;

	_bool bDirJumpLook = false;

	First_ChangeMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
		}
		if ('S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			bDirJumpLook = true;
		}
		if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			bJump = true;
		}
	}
	if ('V' == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_DOWN)
	{
		bNext_CloudCheck = true;
	}


	m_bNext_CloudCheck = bNext_CloudCheck;

	if (bJump && false == bNext_CloudCheck)
	{
		/* 기본 점프 */
		Reset_Anim(pPlayer_AnimState);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
		CPlayer_Jump::Get_Instance()->Set_PlayerMoveValue(0.6f);
		CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	if (bJump && false == bNext_CloudCheck && true == bDirJumpLook)
	{
		/* 기본 뒤로  점프 */
		Reset_Anim(pPlayer_AnimState);
		Reverse_Look(TimeDelta);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
		CPlayer_Jump::Get_Instance()->Set_PlayerMoveValue(0.6f);
		CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	if (true == Player_CloudAnim_End_TO_Player_CloudAnim(pPlayer_AnimState, pAnimKeyDesc, TimeDelta)) /* 다시 구름타기*/
		return;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_CloudAnim_End::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{


	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_CloudAnim_End::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);
	pPlayer_AnimState->Set_AllLerpCheckTrue();
	m_bNext_CloudCheck = false;
	m_bFristCheck  = false;
}

void CPlayer_CloudAnim_End::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_CloudAnim_End::First_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFristCheck)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(70.f, 1.5f);		
		RELEASE_INSTANCE(CGameInstance);
		
		m_iUpperIndex = PLAYER_INNODE_IDLE;
		m_iRootIndex = PLAYER_INNODE_IDLE;
		m_iBottomIndex = PLAYER_INNODE_IDLE;

		m_bNext_CloudCheck = false;
		m_bFristCheck = true;
	}
}

void CPlayer_CloudAnim_End::Enter_Motion_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
}

_bool CPlayer_CloudAnim_End::Player_CloudAnim_End_TO_Player_CloudAnim(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bNext_CloudCheck)
	{
		if (true == pPlayer_AnimState->Cloud_RayCast_Ready(TimeDelta))
		{
			if (nullptr != pPlayer_AnimState->Get_CloudObject())
			{
				Reset_Anim(pPlayer_AnimState);
				m_pTransform->Set_Velocity(XMVectorZero());
				//만약필요한경우에만
				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_CloudAnim::Get_Instance());
				CPlayer_CloudAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
		return false;
	}
	return false;
}

void CPlayer_CloudAnim_End::Free()
{
}
