#include "stdafx.h"
#include "..\public\Taunt_GoatO.h"

CTaunt_GoatO::CTaunt_GoatO(CGoatOtto * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CTaunt_GoatO::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pGoat->Get_Hp())
		return true;

	if (!m_pGoat->IsTaunt())
		return true;

	else 
	{
		if (15 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(15);
		}
		return false;
	}
}

CTaunt_GoatO * CTaunt_GoatO::Create(CGoatOtto * _pGoat)
{
	CTaunt_GoatO* pInstance = new CTaunt_GoatO(_pGoat);

	return pInstance;
}

void CTaunt_GoatO::Free()
{
}
