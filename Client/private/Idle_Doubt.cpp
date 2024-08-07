#include "stdafx.h"
#include "..\public\Idle_Doubt.h"

CIdle_Doubt::CIdle_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CIdle_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (15.f > m_pDoubt->Get_Distance() ||
		0 > m_pDoubt->Get_Hp())
	{
		return true;
	}

	else
	{
		if (3 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(3);
		}
		return false;
	}
}

CIdle_Doubt * CIdle_Doubt::Create(CDoubt * _pDoubt)
{
	CIdle_Doubt* pInstance = new CIdle_Doubt(_pDoubt);

	return pInstance;
}

void CIdle_Doubt::Free()
{
	__super::Free();
}
