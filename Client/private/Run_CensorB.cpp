#include "stdafx.h"
#include "..\public\Run_CensorB.h"

CRun_CensorB::CRun_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CRun_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp() ||
		100 == m_pCensor->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return false;
	}


	else
	{
		if (36 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(36);
		}
		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*3.f);
		return true;
	}
}

CRun_CensorB * CRun_CensorB::Create(CCensor_Burly * _pCensor)
{
	CRun_CensorB* pInstance = new CRun_CensorB(_pCensor);

	return pInstance;
}

void CRun_CensorB::Free()
{
	__super::Free();
}
