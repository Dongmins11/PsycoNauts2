#include "stdafx.h"
#include "..\public\Run_Censor.h"

CRun_Censor::CRun_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CRun_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp() ||
		100 == m_pCensor->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return false;
	}

	else
	{
		if (5 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(5);
		}
		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*4.f);
		return true;
	}
}

CRun_Censor * CRun_Censor::Create(CCensor * _pCensor)
{
	CRun_Censor* pInstance = new CRun_Censor(_pCensor);

	return pInstance;
}

void CRun_Censor::Free()
{
	__super::Free();
}
