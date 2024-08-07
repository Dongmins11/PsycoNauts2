#include "stdafx.h"
#include "..\public\Player_PullSwitch.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_PullSwitch_Move.h"


IMPLEMENT_SINGLETON(CPlayer_PullSwitch)

CPlayer_PullSwitch::CPlayer_PullSwitch()
{
}

void CPlayer_PullSwitch::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	First_Motion_Change(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Enter_to_Idle_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Lerp_LookChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == PullSwitch_to_IdleChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (true == Key_Check_MotionChange(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	m_pModel->Set_UpperAnimation(m_iAnimIndex);
	m_pModel->Set_RootAnimation(m_iAnimIndex);
	m_pModel->Set_BottomAnimation(m_iAnimIndex);
}

void CPlayer_PullSwitch::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_PullSwitch::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD);
	m_pPlayer->Set_PullSwitchMove(false, CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD);

	m_bIsLefp = false;
	m_bFirstCheck = false;
	m_bIsLerpState = false;
	m_bReverse = false;

	m_vSourRotation = _float3(0.f, 0.f, 0.f);
	m_vDestRotation = _float3(0.f, 0.f, 0.f);

	m_lfTimeAcc = 0.0;
	m_iAnimIndex = 0;

	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_PullSwitch::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);
}

void CPlayer_PullSwitch::First_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFirstCheck)
	{
		m_iAnimIndex = PLAYER_PULL_SWITCH_ENTER;

		m_bFirstCheck = true;
	}
}

void CPlayer_PullSwitch::Enter_to_Idle_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PLAYER_PULL_SWITCH_ENTER == m_iAnimIndex && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_PULL_SWITCH_ENTER))
	{
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		m_iAnimIndex = PLAYER_PULL_SWITCH_IDLE;
	}

}

void CPlayer_PullSwitch::Lerp_LookChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (2 >= m_iTickCount)
		++m_iTickCount;

	if (false == m_bIsLerpState && 2 < m_iTickCount)
	{
		m_lfTimeAcc += TimeDelta * 5;

		if (1.0 <= m_lfTimeAcc)
		{
			m_bIsLerpState = true;
			m_lfTimeAcc = 1.0;
		}
		m_pTransform->Set_MyLook(Lerp_Look_State(pPlayer_AnimState, m_lfTimeAcc));

		if (true == m_bIsLerpState)
			m_lfTimeAcc = 0.0;
	}


}

_vector CPlayer_PullSwitch::Lerp_Look_State(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == m_bIsLefp)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CTransform* pTargetTransform = (CTransform*)pGameInstance->Get_Components(SCENE::SCENE_STAGE03, L"Layer_Choppig", L"Com_Transform", 1);
		if (nullptr == pTargetTransform)
		{
			Safe_Release(pGameInstance);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}


		Safe_Release(pGameInstance);
		_vector vPlayerLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
		_vector vTriggerLook = -pTargetTransform->Get_State(CTransform::STATE_LOOK);

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

_bool CPlayer_PullSwitch::Key_Check_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING ||
			'D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			if (PLAYER_PULL_SWITCH_IDLE == m_iAnimIndex)
			{
				//Reset_Anim(pPlayer_AnimState);

				pPlayer_AnimState->Set_AllLerpCheckTrue();
				pPlayer_AnimState->SetUp_State(CPlayer_PullSwitch_Move::Get_Instance());
				CPlayer_PullSwitch_Move::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
				return true;
			}
		}
		if (VK_SHIFT == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		{
			Reset_Anim(pPlayer_AnimState);

			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}

	return false;
}

_bool CPlayer_PullSwitch::PullSwitch_to_IdleChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_pPlayer->Get_PullSwitchingEnd())
	{
		Reset_Anim(pPlayer_AnimState);

		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		return true;
	}

	return false;
}

void CPlayer_PullSwitch::Free()
{
}

