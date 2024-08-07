#include "stdafx.h"
#include "..\public\Jump_BadIdea.h"

CJump_BadIdea::CJump_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CJump_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBadIdea->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		/*if (6 != _pModel->Get_AnimIndex()&& //Slided 벽면 충돌 체크 되면 수정
		7!= _pModel->Get_AnimIndex())
		_pModel->Set_Animation(6);
		if (_pModel->Get_IsAnimFinishied(6) &&
		7 != _pModel->Get_AnimIndex())
		_pModel->Set_Animation(7);*/
		return false;
	}
}

CJump_BadIdea * CJump_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CJump_BadIdea* pInstance = new CJump_BadIdea(_pBadIdea);

	return pInstance;
}

void CJump_BadIdea::Free()
{
}
