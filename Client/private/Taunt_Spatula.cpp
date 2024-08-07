#include "stdafx.h"
#include "..\public\Taunt_Spatula.h"

CTaunt_Spatula::CTaunt_Spatula(CSpatula * _pSpatula)
	:m_pSpatula(_pSpatula)
{
}

_bool CTaunt_Spatula::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (!m_pSpatula->IsTaunt())
		return true;

	else
	{
		if (5 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(5);
		}
		return false;
	}
}

CTaunt_Spatula * CTaunt_Spatula::Create(CSpatula * _pSpatula)
{
	CTaunt_Spatula* pInstance = new CTaunt_Spatula(_pSpatula);

	return pInstance;
}

void CTaunt_Spatula::Free()
{
}
