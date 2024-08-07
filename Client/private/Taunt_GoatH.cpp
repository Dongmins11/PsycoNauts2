#include "stdafx.h"
#include "..\public\Taunt_GoatH.h"

CTaunt_GoatH::CTaunt_GoatH(CGoatHollis * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CTaunt_GoatH::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
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

CTaunt_GoatH * CTaunt_GoatH::Create(CGoatHollis * _pGoat)
{
	CTaunt_GoatH* pInstance = new CTaunt_GoatH(_pGoat);

	return pInstance;
}

void CTaunt_GoatH::Free()
{
}
