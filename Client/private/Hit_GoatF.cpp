#include "stdafx.h"
#include "..\public\Hit_GoatF.h"

CHit_GoatF::CHit_GoatF(CGoatFord * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CHit_GoatF::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pGoat->Get_Hp() ||
		!m_pGoat->IsHit() ||
		!m_pGoat->IsHitPhase())
		return true;

	if (_pModel->Get_IsAnimFinishied(6) &&
		m_IsHitStart)
	{
		m_IsHitStart = false;
		m_pGoat->Reset_HitState();
		return true;
	}

	else
	{
		if (!m_IsHitStart)
		{
			m_fScale += 0.05f;
			m_fHeadScale += 0.05f;
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(6);
			_pModel->BoneScailing("spine0", m_fScale);
			_pModel->BoneScailing("spine2", 1.f / m_fScale);
			_pModel->BoneScailing("Lf_clavicle", m_fScale);
			_pModel->BoneScailing("Rt_clavicle", m_fScale);
			_pModel->BoneScailing("Lf_shoulder", 1.f / m_fScale);
			_pModel->BoneScailing("Rt_shoulder", 1.f / m_fScale);
			_pModel->BoneScailing("spine0", m_fScale);
			_pModel->BoneScailing("head", m_fHeadScale);
			m_IsHitStart = true;
		}

		return false;
	}
}

void CHit_GoatF::ResetHit()
{
	m_IsHitStart = false;
}

CHit_GoatF * CHit_GoatF::Create(CGoatFord * _pGoat)
{
	CHit_GoatF* pInstance = new CHit_GoatF(_pGoat);

	return pInstance;
}

void CHit_GoatF::Free()
{
}
