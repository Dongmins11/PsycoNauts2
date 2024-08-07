#include "stdafx.h"
#include "..\public\Spawn_Censor.h"

CSpawn_Censor::CSpawn_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CSpawn_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
	{
		return true;
	}

	else
	{
		if (m_pCensor->IsGround()) // 착지했는지 확인
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(2);
			m_IsSpawned = true;
			return false;
		}
		if (_pModel->Get_IsAnimFinishied(0)) //생성이 공중에서 되기에 떨어지는 모션으로 
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(1);
			m_pCensor->Set_Gravity(true);
			m_pCensor->Spawned(true);
		}
		return false;
	}
}

CSpawn_Censor * CSpawn_Censor::Create(CCensor * _pCensor)
{
	CSpawn_Censor* pInstance = new CSpawn_Censor(_pCensor);

	return pInstance;
}

void CSpawn_Censor::Free()
{
	__super::Free();
}
