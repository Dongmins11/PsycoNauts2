#include "stdafx.h"
#include "..\public\Run_BadIdea.h"

CRun_BadIdea::CRun_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CRun_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBadIdea->Get_Hp() ||
		100 == m_pBadIdea->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return false;
	}

	else
	{
		if (30 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(30);
		}
		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*2.f);
		return true;
	}
}

CRun_BadIdea * CRun_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CRun_BadIdea* pInstance = new CRun_BadIdea(_pBadIdea);

	return pInstance;
}

void CRun_BadIdea::Free()
{
}
