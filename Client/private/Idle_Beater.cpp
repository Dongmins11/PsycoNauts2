#include "stdafx.h"
#include "..\public\Idle_Beater.h"

CIdle_Beater::CIdle_Beater(CBeater * _pBeater)
	:m_pBeater(_pBeater)
{
}

_bool CIdle_Beater::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBeater->Get_Hp())
		return true;
	else
	{
		if (7 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(7);
		}
		return false;
	}
}

CIdle_Beater * CIdle_Beater::Create(CBeater * _pBeater)
{
	CIdle_Beater* pInstance = new CIdle_Beater(_pBeater);

	return pInstance;
}

void CIdle_Beater::Free()
{
}
