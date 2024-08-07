#include "stdafx.h"
#include "..\public\Idle_GoatH.h"

CIdle_GoatH::CIdle_GoatH(CGoatHollis * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CIdle_GoatH::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if(0>=m_pGoat->Get_Hp())
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

CIdle_GoatH * CIdle_GoatH::Create(CGoatHollis * _pGoat)
{
	CIdle_GoatH* pInstance = new CIdle_GoatH(_pGoat);

	return pInstance;
}

void CIdle_GoatH::Free()
{
}
