#include "stdafx.h"
#include "..\public\Attack_Doubt.h"

CAttack_Doubt::CAttack_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CAttack_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 > m_pDoubt->Get_Hp())
		return true;

	if (1.f > m_pDoubt->Get_Distance() &&
		!m_IsAttackLoop)
	{
		m_IsAttackLoop = true;
		m_pDoubt->Set_AttackLoop(true);
	}
	if (_pModel->Get_IsAnimFinishied(0))
	{
		m_IsAttackLoop = false;
		m_IsDealyAttack = true;
		if (!m_pDoubt->Is_AttackSure())
			m_IsUnArmed = true;
		m_pDoubt->Set_AttackLoop(false);
		_pModel->Set_IsAnimLefp(true);
		_pModel->Set_Animation(28);
	}

	if (1 == _pModel->Get_AnimIndex() &&
		3 >= _pModel->Get_AnimationCurrentFrame(1))
		m_pDoubt->Set_Armed(true);

	if (m_IsDealyAttack)
	{
		m_dDelayTime += TimeDelta;
		if (4.0 < m_dDelayTime)
		{
			m_IsDealyAttack = false;
			m_dDelayTime = 0.0;
		}
	}
	if (m_IsUnArmed ||
		m_IsDealyAttack ||
		!m_IsAttackLoop)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야 시야각 10도 안에 들어와있어야 true
	{
		return true;
	}

	else // 1 - 4 추를 떨어트릴 것.
	{
		if (m_IsAttackLoop)
		{
			if (0 != _pModel->Get_AnimIndex() &&
				1 != _pModel->Get_AnimIndex() &&
				2 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(2);
			}
			else if (_pModel->Get_IsAnimFinishied(2))
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(1);
			}
			else if (_pModel->Get_IsAnimFinishied(1))
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(0);
			}
		}
		return false;
	}
}

CAttack_Doubt * CAttack_Doubt::Create(CDoubt * _pDoubt)
{
	CAttack_Doubt* pInstance = new CAttack_Doubt(_pDoubt);

	return pInstance;
}

void CAttack_Doubt::Free()
{
	__super::Free();
}
