#include "stdafx.h"
#include "..\public\Hit_Judge.h"

CHit_Judge::CHit_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CHit_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (!m_pJudge->IsArmed())
		return false;

	m_IsHit = m_pJudge->IsHit(); //피격상태를 계속 업데이트 할 것 이다
	if (0 >= m_pJudge->Get_Hp() ||
		!m_IsHit &&
		m_IsRecovery)		//피격상태가 아니며 & 회복이 다된 상태일 때 나갈거야
	{
		return true;
	}

	else
	{
		if (_pModel->Get_IsAnimFinishied(16) ||
			_pModel->Get_IsAnimFinishied(22) ||
			_pModel->Get_IsAnimFinishied(29) ||
			_pModel->Get_IsAnimFinishied(35))
		{
			m_IsRecovery = true;
			m_IsRecoveryLoop = false;
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(18);
			m_pJudge->Reset_HitState();
			return false;
		}
		if (!m_IsRecoveryLoop)
		{
			m_IsRecovery = false;
			if (CEnemy::ATTACKED::TYPE_END != m_pJudge->Get_Attacked_Type())
				m_IsRecoveryLoop = true;
			
			if (CEnemy::ATTACKED::TO == m_pJudge->Get_Attacked_Type())
			{
				if (36 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(36);
				}
			}
			else if (CEnemy::ATTACKED::PB == m_pJudge->Get_Attacked_Type())
			{
				if (17 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(17);
				}
			}
			else if (CEnemy::ATTACKED::B == m_pJudge->Get_Attacked_Type())
			{
				if (29 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(29);
				}
			}
			else
			{
				if (22 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(22);
				}
			}
		}
		else //연계가되는 애니메이션
		{
			if (_pModel->Get_IsAnimFinishied(17) &&
				16 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(16);
			}
			else if (_pModel->Get_IsAnimFinishied(36) &&
				17 != _pModel->Get_AnimIndex() &&
				16 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(35);
			}
		}

		return false;
	}
}

CHit_Judge * CHit_Judge::Create(CJudge * _pJudge)
{
	CHit_Judge* pInstance = new CHit_Judge(_pJudge);

	return pInstance;
}

void CHit_Judge::Free()
{
}
