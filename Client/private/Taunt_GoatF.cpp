#include "stdafx.h"
#include "..\public\Taunt_GoatF.h"

CTaunt_GoatF::CTaunt_GoatF(CGoatFord * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CTaunt_GoatF::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
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

CTaunt_GoatF * CTaunt_GoatF::Create(CGoatFord * _pGoat)
{
	CTaunt_GoatF* pInstance = new CTaunt_GoatF(_pGoat);

	return pInstance;
}

void CTaunt_GoatF::Free()
{
}
