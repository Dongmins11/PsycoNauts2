#include "stdafx.h"
#include "..\public\Idle_GoatO.h"

CIdle_GoatO::CIdle_GoatO(CGoatOtto * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CIdle_GoatO::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pGoat->Get_Hp())
		return true;
	else
	{
		if (8 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(8);
		}
		return false;
	}
}

CIdle_GoatO * CIdle_GoatO::Create(CGoatOtto * _pGoat)
{
	CIdle_GoatO* pInstance = new CIdle_GoatO(_pGoat);

	return pInstance;
}

void CIdle_GoatO::Free()
{
}
