#include "stdafx.h"
#include "..\public\Hit_Judge_UA.h"

CHit_Judge_UA::CHit_Judge_UA(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CHit_Judge_UA::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_pJudge->IsArmed())
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
		if (_pModel->Get_IsAnimFinishied(58) ||
			_pModel->Get_IsAnimFinishied(64) ||
			_pModel->Get_IsAnimFinishied(79))
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
				if (80 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(80);
				}
			}
			else if (CEnemy::ATTACKED::PB == m_pJudge->Get_Attacked_Type())
			{
				if (59 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(59);
				}
			}
			else
			{
				if (64 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(64);
				}
			}
		}
		else //연계가되는 애니메이션
		{
			if (_pModel->Get_IsAnimFinishied(59) &&
				58 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(58);
			}
			else if (_pModel->Get_IsAnimFinishied(80) &&
				80 == _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(79);
			}
		}

		return false;
	}
}

CHit_Judge_UA * CHit_Judge_UA::Create(CJudge * _pJudge)
{
	CHit_Judge_UA* pInstance = new CHit_Judge_UA(_pJudge);

	return pInstance;
}

void CHit_Judge_UA::Free()
{
}
