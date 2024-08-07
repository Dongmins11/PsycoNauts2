#include "stdafx.h"
#include "..\public\Taunt_Beater.h"

CTaunt_Beater::CTaunt_Beater(CBeater * _pBeater)
	:m_pBeater(_pBeater)
{
}

_bool CTaunt_Beater::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (!m_pBeater->IsTaunt())
		return true;

	else
	{
		if (9 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(9);
		}
		return false;
	}
}

CTaunt_Beater * CTaunt_Beater::Create(CBeater * _pBeater)
{
	CTaunt_Beater* pInstance = new CTaunt_Beater(_pBeater);

	return pInstance;
}

void CTaunt_Beater::Free()
{
}
