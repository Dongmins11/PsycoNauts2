#include "stdafx.h"
#include "..\public\Walk_BadIdea.h"

CWalk_BadIdea::CWalk_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CWalk_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBadIdea->Get_Hp() ||
		100 != m_pBadIdea->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (37 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(37);
		}
		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta);
		return true;
	}
}

CWalk_BadIdea * CWalk_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CWalk_BadIdea* pInstance = new CWalk_BadIdea(_pBadIdea);

	return pInstance;
}

void CWalk_BadIdea::Free()
{
}
