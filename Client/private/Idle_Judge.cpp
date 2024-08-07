#include "stdafx.h"
#include "..\public\Idle_Judge.h"

CIdle_Judge::CIdle_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CIdle_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (20.f > m_pJudge->Get_Distance() ||
		0 >= m_pJudge->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (18 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(18);
		}
		return false;
	}
}

CIdle_Judge * CIdle_Judge::Create(CJudge * _pJudge)
{
	CIdle_Judge* pInstance = new CIdle_Judge(_pJudge);

	return pInstance;
}

void CIdle_Judge::Free()
{
}
