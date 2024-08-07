#include "stdafx.h"
#include "..\public\Idle_Spatula.h"

CIdle_Spatula::CIdle_Spatula(CSpatula * _pSpatula)
	:m_pSpatula(_pSpatula)
{
}

_bool CIdle_Spatula::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pSpatula->Get_Hp())
		return true;
	else
	{
		if (4 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(4);
		}
		return false;
	}
}

CIdle_Spatula * CIdle_Spatula::Create(CSpatula* _pSpatula)
{
	CIdle_Spatula* pInstance = new CIdle_Spatula(_pSpatula);

	return pInstance;
}

void CIdle_Spatula::Free()
{
}
