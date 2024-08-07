#include "stdafx.h"
#include "..\public\Spawn_CensorB.h"

CSpawn_CensorB::CSpawn_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CSpawn_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
	{
		return true;
	}

	else
	{
		if (_pModel->Get_IsAnimFinishied(22))
		{
			m_IsSpawned = true;
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(18);
			return true;
		}
		if (m_pCensor->IsGround() &&
			22 != _pModel->Get_AnimIndex()) // 착지했는지 확인
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(22);
			m_pCensor->Spawned(true);
			return false;
		}
		if (20 <= _pModel->Get_AnimationCurrentFrame(39)) //생성이 공중에서 되기에 떨어지는 모션으로 
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(23);
			m_pCensor->Set_Gravity(true);
		}
		return false;
	}
}

CSpawn_CensorB * CSpawn_CensorB::Create(CCensor_Burly * _pCensor)
{
	CSpawn_CensorB* pInstance = new CSpawn_CensorB(_pCensor);

	return pInstance;
}

void CSpawn_CensorB::Free()
{
	__super::Free();
}
