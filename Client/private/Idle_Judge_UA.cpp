#include "stdafx.h"
#include "..\public\Idle_Judge_UA.h"

CIdle_Judge_UA::CIdle_Judge_UA(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CIdle_Judge_UA::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (20.f > m_pJudge->Get_Distance() ||
		0 >= m_pJudge->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (60 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(60);
		}
		return false;
	}
}

CIdle_Judge_UA * CIdle_Judge_UA::Create(CJudge * _pJudge)
{
	CIdle_Judge_UA* pInstance = new CIdle_Judge_UA(_pJudge);

	return pInstance;
}

void CIdle_Judge_UA::Free()
{
}
