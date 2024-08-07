#include "stdafx.h"
#include "..\public\Idle_BadIdea.h"

CIdle_BadIdea::CIdle_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CIdle_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBadIdea->Get_Hp() ||
		20.f > m_pBadIdea->Get_Distance())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (22 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(22);
		}
		return false;
	}
}

CIdle_BadIdea * CIdle_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CIdle_BadIdea* pInstance = new CIdle_BadIdea(_pBadIdea);

	return pInstance;
}

void CIdle_BadIdea::Free()
{
}
