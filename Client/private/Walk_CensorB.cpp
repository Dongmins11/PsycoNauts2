#include "stdafx.h"
#include "..\public\Walk_CensorB.h"

CWalk_CensorB::CWalk_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CWalk_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp() ||
		100 != m_pCensor->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (44 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(44);
		}
		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta);
		return true;
	}
}

CWalk_CensorB * CWalk_CensorB::Create(CCensor_Burly * _pCensor)
{
	CWalk_CensorB* pInstance = new CWalk_CensorB(_pCensor);

	return pInstance;
}

void CWalk_CensorB::Free()
{
	__super::Free();
}
