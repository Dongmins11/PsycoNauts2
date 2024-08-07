#include "stdafx.h"
#include "..\public\Idle_CensorB.h"

CIdle_CensorB::CIdle_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CIdle_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp() ||
		20.f > m_pCensor->Get_Distance())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
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

CIdle_CensorB * CIdle_CensorB::Create(CCensor_Burly * _pCensor)
{
	CIdle_CensorB* pInstance = new CIdle_CensorB(_pCensor);

	return pInstance;
}

void CIdle_CensorB::Free()
{
	__super::Free();
}
