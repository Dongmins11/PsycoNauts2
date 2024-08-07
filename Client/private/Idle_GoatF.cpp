#include "stdafx.h"
#include "..\public\Idle_GoatF.h"

CIdle_GoatF::CIdle_GoatF(CGoatFord * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CIdle_GoatF::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
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

CIdle_GoatF * CIdle_GoatF::Create(CGoatFord * _pGoat)
{
	CIdle_GoatF* pInstance = new CIdle_GoatF(_pGoat);

	return pInstance;
}

void CIdle_GoatF::Free()
{
}
