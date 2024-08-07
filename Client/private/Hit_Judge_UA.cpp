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

	m_IsHit = m_pJudge->IsHit(); //�ǰݻ��¸� ��� ������Ʈ �� �� �̴�
	if (0 >= m_pJudge->Get_Hp() ||
		!m_IsHit &&
		m_IsRecovery)		//�ǰݻ��°� �ƴϸ� & ȸ���� �ٵ� ������ �� �����ž�
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
		else //���谡�Ǵ� �ִϸ��̼�
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
