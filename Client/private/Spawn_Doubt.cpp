#include "stdafx.h"
#include "..\public\Spawn_Doubt.h"

CSpawn_Doubt::CSpawn_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CSpawn_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
 	if (m_IsSpawned)
	{
		return true;
	}

	else
	{
		m_dAccTime += TimeDelta;

		if (4.0 < m_dAccTime)
		{
			m_IsSpawned = true;
			m_pDoubt->Spawned(true);
		}

		return false;
	}
}

CSpawn_Doubt * CSpawn_Doubt::Create(CDoubt * _pDoubt)
{
	CSpawn_Doubt* pInstance = new CSpawn_Doubt(_pDoubt);

	return pInstance;
}

void CSpawn_Doubt::Free()
{
	__super::Free();
}
