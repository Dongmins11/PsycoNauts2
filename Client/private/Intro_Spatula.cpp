#include "stdafx.h"
#include "..\public\Intro_Spatula.h"

CIntro_Spatula::CIntro_Spatula(CSpatula * _pSpatula)
	:m_pSpatula(_pSpatula)
{
}

_bool CIntro_Spatula::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
		return true;

	else
	{
		if (_pModel->Get_IsAnimFinishied(3)&&
			2!=_pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(2);
		}
		else if (_pModel->Get_IsAnimFinishied(2) &&
			!m_IsSpawned)
		{
			m_IsSpawned = true;
			m_pSpatula->Spawned(true);
			return true;
		}
		return false;
	}
}

CIntro_Spatula * CIntro_Spatula::Create(CSpatula * _pSpatula)
{
	CIntro_Spatula* pInstance = new CIntro_Spatula(_pSpatula);

	return pInstance;
}

void CIntro_Spatula::Free()
{
}
