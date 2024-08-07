#include "stdafx.h"
#include "..\public\Spawn_GoatF.h"

CSpawn_GoatF::CSpawn_GoatF(CGoatFord * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CSpawn_GoatF::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
		return true;
	else
	{
		if (8 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(8);
		}

		if (8 == _pModel->Get_AnimIndex())
		{
			if (0.f >XMVectorGetY(_pTransform->Get_Position()) &&
				!m_IsArrived)
				_pTransform->Go_ByAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);

			if (0.f < XMVectorGetY(_pTransform->Get_Position()))
			{
				m_IsArrived = true;
				_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(_pTransform->Get_Position()), 0.f, XMVectorGetZ(_pTransform->Get_Position()), 0.f));
			}
		}

		if (m_pGoat->IsReady() &&
			!m_IsSpawned)
		{
			m_IsSpawned = true;
			m_pGoat->Spawned(true);
			return false;
		}
		return false;
	}
}

CSpawn_GoatF * CSpawn_GoatF::Create(CGoatFord * _pGoat)
{
	CSpawn_GoatF* pInstance = new CSpawn_GoatF(_pGoat);

	return pInstance;
}

void CSpawn_GoatF::Free()
{
}
