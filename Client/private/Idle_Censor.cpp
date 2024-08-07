#include "stdafx.h"
#include "..\public\Idle_Censor.h"

CIdle_Censor::CIdle_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CIdle_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp() ||
		20.f > m_pCensor->Get_Distance())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (3 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(3);
		}
		return false;
	}
}

CIdle_Censor * CIdle_Censor::Create(CCensor * _pCensor)
{
	CIdle_Censor* pInstance = new CIdle_Censor(_pCensor);

	return pInstance;
}

void CIdle_Censor::Free()
{
	__super::Free();
}
