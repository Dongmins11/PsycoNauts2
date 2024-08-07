#include "stdafx.h"
#include "..\public\Attack_BadIdea.h"
#include "Bulb.h"

CAttack_BadIdea::CAttack_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CAttack_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBadIdea->Get_Hp())
		return true;

	if (_pModel->Get_IsAnimFinishied(3) ||
		_pModel->Get_IsAnimFinishied(5))
	{
		m_IsDealyAttack = true;
		m_IsFarAttack = false;
		m_IsNearAttack = false;
		m_IsAttackLoop = false;
		m_IsThrow = false;
		m_IsBite = false;
		m_pBadIdea->Set_AttackLoop(false);
		return true;
	}
	if (7.f > m_pBadIdea->Get_Distance() &&
		!m_IsAttackLoop &&
		!m_IsDealyAttack&&
		(340.f <= m_pBadIdea->Get_Degree() ||
			20.f >= m_pBadIdea->Get_Degree()))
	{
		m_pBadIdea->Set_AttackLoop(true);
		m_IsAttackLoop = true;
	}

	if (m_IsDealyAttack)
	{
		m_dDelayTime += TimeDelta;
		if (4.0 < m_dDelayTime||
			1.5f > m_pBadIdea->Get_Distance())
		{
			m_IsDealyAttack = false;
			m_dDelayTime = 0.0;
		}
	}
	else if(m_IsAttackLoop)
	{
		if (2.f > m_pBadIdea->Get_Distance())
		{
			m_IsFarAttack = false;
			m_IsNearAttack = true;
		}
		else
		{
			m_IsFarAttack = true;
			m_IsNearAttack = false;
		}
	}
	if (!m_IsAttackLoop||
		m_IsDealyAttack)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야 시야각 10도 안에 들어와있어야 true
	{
		return true;
	}

	else 
	{
		if (m_IsAttackLoop)
		{
			if (m_IsNearAttack)
			{
				if (3 != _pModel->Get_AnimIndex() &&
					4 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(4);
				}
				else if (3 != _pModel->Get_AnimIndex() &&
					_pModel->Get_IsAnimFinishied(4))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(3);
				}
			}
			else if(m_IsFarAttack)
			{
				if (5 != _pModel->Get_AnimIndex() &&
					6 != _pModel->Get_AnimIndex() &&
					7 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(7);
				}
				else if (_pModel->Get_IsAnimFinishied(7))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(6);
				}
				else if (_pModel->Get_IsAnimFinishied(6))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(5);
				}
			}
			if ((6 == _pModel->Get_AnimIndex() && (3< _pModel->Get_AnimationCurrentFrame(6)))&&
				!m_IsThrow)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CBulb::BulbWorldDesc tDesc;
				tDesc.fDistance = m_pBadIdea->Get_Distance();
				XMStoreFloat3(&tDesc.vPos, _pTransform->Get_State(CTransform::STATE_POSITION));
				tDesc.vDir.x = m_pBadIdea->Get_TargetDir().x;
				tDesc.vDir.y = m_pBadIdea->Get_TargetDir().y;
				tDesc.vDir.z = m_pBadIdea->Get_TargetDir().z;
				// 전구 던지기 공격이고 Bulb클래스에서 자세하게 설명
				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Bulb"), m_pBadIdea->Get_CurrentScene(), TEXT("Layer_Bulb"), &tDesc);
				RELEASE_INSTANCE(CGameInstance);
				m_IsThrow = true;
			}
			else if ((3 == _pModel->Get_AnimIndex() && (2<= _pModel->Get_AnimationCurrentFrame(3))) &&
				!m_IsBite)
			{
				//깨물기 공격이라서 이빨 모양 렉트이펙트 하면될듯?
				m_IsBite = true;
				m_pBadIdea->Bite(true);
			}
		}
		return false;
	}
}

CAttack_BadIdea * CAttack_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CAttack_BadIdea* pInstance = new CAttack_BadIdea(_pBadIdea);

	return pInstance;
}

void CAttack_BadIdea::Free()
{
}
