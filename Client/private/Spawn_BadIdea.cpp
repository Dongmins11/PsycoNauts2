#include "stdafx.h"
#include "..\public\Spawn_BadIdea.h"

CSpawn_BadIdea::CSpawn_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CSpawn_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
	{
		return true;
	}

	else
	{
		if (m_pBadIdea->IsGround()&&
			!m_IsLand) //생성이 공중에서 되기에 떨어지는 모션으로 
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(15);
			m_pBadIdea->Spawned(true);
			m_IsLand = true;
		}
		else if (_pModel->Get_IsAnimFinishied(15))
		{
			m_IsSpawned = true;
			return true;
		}
		return false;
	}
}

CSpawn_BadIdea * CSpawn_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CSpawn_BadIdea* pInstance = new CSpawn_BadIdea(_pBadIdea);

	return pInstance;
}

void CSpawn_BadIdea::Free()
{
}
