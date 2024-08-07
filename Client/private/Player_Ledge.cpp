#include "stdafx.h"
#include "..\public\Player_Ledge.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Dodge.h"
#include "Player_Falling.h"
#include "Player_LedgeJump.h"
#include "Player_LedgeMove.h"
#include "Camera_Fly.h"
IMPLEMENT_SINGLETON(CPlayer_Ledge)

CPlayer_Ledge::CPlayer_Ledge()
{
}

void CPlayer_Ledge::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;
	pPlayer_AnimState->Set_LedgeCheck(true);

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if (PLAYER_LEDGEGRAB_LOOP == m_iRootIndex)
		{
			if (VK_SHIFT == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				m_iUpperIndex = PLAYER_LEDGEGRAB_DOWN;
				m_iRootIndex = PLAYER_LEDGEGRAB_DOWN;
				m_iBottomIndex = PLAYER_LEDGEGRAB_DOWN;
			}
			else if ('A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();

				pPlayer_AnimState->SetUp_State(CPlayer_LedgeMove::Get_Instance());
				CPlayer_LedgeMove::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return;
			}
			else if ('D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				pPlayer_AnimState->Set_AllLerpCheckTrue();

				pPlayer_AnimState->SetUp_State(CPlayer_LedgeMove::Get_Instance());
				CPlayer_LedgeMove::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return;
			}
		}
	}

	Set_FirstCheck(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	SetUp_LerpState(pPlayer_AnimState, TimeDelta);

	Set_Loop_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == GrabDown_To_Falling(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Ledge_to_Jump(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == m_bFirstCheck)
	{
		if (m_iRootPreIndex != m_iRootIndex)
		{
			pPlayer_AnimState->Set_AllLerpCheckTrue();
			m_iRootPreIndex = m_iRootIndex;
		}
	}


	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Ledge::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Ledge::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bFirstLerpCheck = false;
	m_bIsLefp = false;
	m_bFirstCheck = false;
	m_bIsLerpState = false;

	m_vLerpPosition = _float4(0.f,0.f,0.f,0.f);
	m_vTriggerPosition = _float4(0.f, 0.f, 0.f, 0.f);
	m_vSourRotation = _float3(0.f, 0.f, 0.f);
	m_vDestRotation = _float3(0.f, 0.f, 0.f);
	m_bReverse = false;

	m_lfTimeAcc = 0.0;
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Ledge::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
}

void CPlayer_Ledge::Set_FirstCheck(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iUpperIndex = PLAYER_DOWN_LEDGEGRAB;
		m_iRootIndex = PLAYER_DOWN_LEDGEGRAB;
		m_iBottomIndex = PLAYER_DOWN_LEDGEGRAB;

		m_pModel->Set_UpperAnimBlendSpeed(PLAYER_DOWN_LEDGEGRAB, 6.f);
		m_pModel->Set_RootAnimBlendSpeed(PLAYER_DOWN_LEDGEGRAB, 6.f);
		m_pModel->Set_BottomAnimBlendSpeed(PLAYER_DOWN_LEDGEGRAB, 6.f);

		m_bFirstCheck = true;


		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_LEDGE_SOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Ledge.wav"), CHANNELID::PLAYER_LEDGE_SOUND)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		Set_Signal_Ledge();
	}
}

void CPlayer_Ledge::Set_Loop_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_DOWN_LEDGEGRAB == m_iRootIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_DOWN_LEDGEGRAB))
	{
		m_iUpperIndex = PLAYER_LEDGEGRAB_LOOP;
		m_iRootIndex = PLAYER_LEDGEGRAB_LOOP;
		m_iBottomIndex = PLAYER_LEDGEGRAB_LOOP;
	}
}


_bool CPlayer_Ledge::Ledge_to_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			//여기서 점프진행 
			Set_Signal_Exit_Up();
			m_iTestTickCount = 0;
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_LedgeJump::Get_Instance());
			CPlayer_LedgeJump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

_vector CPlayer_Ledge::SetUp_LerpPosition(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (nullptr == m_pTrigger_Parkour)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (false == m_bFirstLerpCheck)
	{
		XMStoreFloat4(&m_vLerpPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));

		m_vTriggerPosition = m_pTrigger_Parkour->Get_Trigger_Position();

		m_bFirstLerpCheck = true;
	}


	return XMVectorLerp(XMLoadFloat4(&m_vLerpPosition), XMLoadFloat4(&m_vTriggerPosition), (_float)TimeDelta);
}

_vector CPlayer_Ledge::SetUp_LerpLook(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == m_bIsLefp)
	{
		_vector vPlayerLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
		_vector vTriggerLook = XMLoadFloat3(&m_pTrigger_Parkour->Get_Trigger_LookDirection());

		XMStoreFloat3(&m_vSourRotation, vPlayerLook);
		XMStoreFloat3(&m_vDestRotation, vTriggerLook);

		_vector vLook;

		if (!XMVectorGetX(XMVectorEqual(XMLoadFloat3(&m_vDestBackUp), XMLoadFloat3(&m_vDestRotation))))
			XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));

		_float fCos = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&m_vSourRotation)), XMVector3Normalize(XMLoadFloat3(&m_vDestRotation))));
		if (-0.9f >= fCos)
			m_bReverse = true;


		if (false == m_bReverse)
		{
			m_fRatio += (_float)TimeDelta * 2.f;
			if (1.f <= m_fRatio)
			{
				m_fRatio = 0.f;
				m_bIsLefp = true;
				return XMLoadFloat3(&m_vDestRotation);
			}

			_float fRad = XMScalarACos(fCos);
			_float fSinRad = XMScalarSin(fRad);
			{
				if (0.f == fSinRad)
				{
					XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
					return XMLoadFloat3(&m_vSourRotation);
				}
			}
			vLook = (XMScalarSin(fabsf(fRad)*(1 - m_fRatio)) / fabsf(fSinRad))*XMVector3Normalize(XMLoadFloat3(&m_vSourRotation)) + (XMScalarSin(fabsf(fRad)*m_fRatio) / fabsf(fSinRad))* XMVector3Normalize(XMLoadFloat3(&m_vDestRotation));
		}
		else
		{
			m_fRatio += (_float)TimeDelta;
			if (1.f <= m_fRatio)
			{
				m_fRatio = 0.f;
				m_bIsLefp = true;
				m_bReverse = false;
				return XMLoadFloat3(&m_vDestRotation);
			}
			m_pTransform->RotationPerSec(m_pTransform->Get_State(CTransform::STATE_UP), TimeDelta);
			vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
		}
		m_vDestBackUp = m_vDestRotation;
		m_bReverse = false;

		return vLook;
	}

	return m_pTransform->Get_State(CTransform::STATE_LOOK);
}

void CPlayer_Ledge::SetUp_LerpState(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (2 >= m_iTestTickCount)
		++m_iTestTickCount;

	if (false == m_bIsLerpState && 2 < m_iTestTickCount)
	{
		m_lfTimeAcc += TimeDelta * 5;

		if (1.0 <= m_lfTimeAcc)
		{
			m_bIsLerpState = true;
			m_lfTimeAcc = 1.0;
		}


		_vector LerpPosition = SetUp_LerpPosition(pPlayer_AnimState, m_lfTimeAcc);
		_float3 LerpPos;
		XMStoreFloat3(&LerpPos, LerpPosition);

		m_pCharacterController->Set_FootPosition(LerpPos);
		m_pTransform->Set_State(CTransform::STATE_POSITION, LerpPosition);
		m_pTransform->Set_MyLook(SetUp_LerpLook(pPlayer_AnimState, m_lfTimeAcc));

		if (true == m_bIsLerpState)
		{
			m_bFirstLerpCheck = false;
			m_lfTimeAcc = 0.0;
		}
	}
}

void CPlayer_Ledge::Set_Signal_Ledge()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Signal_Ledge();
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_Ledge::Set_Signal_Exit_Up()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Signal_Exit_Up();
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_Ledge::Set_Signal_Exit_Down()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Signal_Exit_Down();
	RELEASE_INSTANCE(CGameInstance);
}

_bool CPlayer_Ledge::GrabDown_To_Falling(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_LEDGEGRAB_DOWN == m_iRootIndex)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PLAYER_LEDGEGRAB_DOWN))
		{
			//여기
			Set_Signal_Exit_Down();
			m_iTestTickCount = 0;
			pPlayer_AnimState->Set_LedgeCheck(false);
			Reset_Anim(pPlayer_AnimState);
			pPlayer_AnimState->SetUp_State(CPlayer_Falling::Get_Instance());
			CPlayer_Falling::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
		else if (0 <= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_LEDGEGRAB_DOWN) && 2 >= m_pModel->Get_RootAnimationCurrentFrame(PLAYER_LEDGEGRAB_DOWN))
		{
			m_pTransform->Go_Backward(TimeDelta * 0.15);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(m_pTransform->Get_State(CTransform::STATE_POSITION)), XMVectorGetY(m_pTransform->Get_State(CTransform::STATE_POSITION)) - (TimeDelta * 2.0), XMVectorGetZ(m_pTransform->Get_State(CTransform::STATE_POSITION)), 1.f));
			m_pCharacterController->Set_FootPosition(_float3(XMVectorGetX(m_pTransform->Get_State(CTransform::STATE_POSITION)), XMVectorGetY(m_pTransform->Get_State(CTransform::STATE_POSITION)), XMVectorGetZ(m_pTransform->Get_State(CTransform::STATE_POSITION))));
		}
	}

	return false;
}

void CPlayer_Ledge::Free()
{
}

