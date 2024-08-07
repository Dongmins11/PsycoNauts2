#include "stdafx.h"
#include "..\public\Move_Doubt.h"

CMove_Doubt::CMove_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CMove_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	
	if (1.f >= m_pDoubt->Get_Distance() ||
		0 > m_pDoubt->Get_Hp())
	{
		return true;
	}

	else
	{
		m_dSpeed = 1.0;
		if (m_pDoubt->Is_AttackSure())
		{
			if (4 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(4);
			}
		}
		else
		{
			if (28 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(28);
			}
		}

		if (5.f < m_pDoubt->Get_Distance())
			m_dSpeed = 2.f;

		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*(_float)m_dSpeed);
		return false;
	}
}

CMove_Doubt * CMove_Doubt::Create(CDoubt * _pDoubt)
{
	CMove_Doubt* pInstance = new CMove_Doubt(_pDoubt);

	return pInstance;
}

void CMove_Doubt::Free()
{
	__super::Free();
}
