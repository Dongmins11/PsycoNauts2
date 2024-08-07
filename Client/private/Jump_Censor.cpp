#include "stdafx.h"
#include "..\public\Jump_Censor.h"

CJump_Censor::CJump_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CJump_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
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

CJump_Censor * CJump_Censor::Create(CCensor * _pCensor)
{
	CJump_Censor* pInstance = new CJump_Censor(_pCensor);

	return pInstance;
}

void CJump_Censor::Free()
{
	__super::Free();
}
