#include "stdafx.h"
#include "..\public\Walk_Censor.h"

CWalk_Censor::CWalk_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CWalk_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (4 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(4);
		}
		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta);
		return true;
	}
}

CWalk_Censor * CWalk_Censor::Create(CCensor * _pCensor)
{
	CWalk_Censor* pInstance = new CWalk_Censor(_pCensor);

	return pInstance;
}

void CWalk_Censor::Free()
{
	__super::Free();
}
