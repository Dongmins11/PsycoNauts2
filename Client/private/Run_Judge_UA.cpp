#include "stdafx.h"
#include "..\public\Run_Judge_UA.h"

CRun_Judge_UA::CRun_Judge_UA(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CRun_Judge_UA::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pJudge->Get_Hp() ||
		100 == m_pJudge->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return false;
	}

	else
	{
		if (10.f < m_pJudge->Get_Distance() && (73 != _pModel->Get_AnimIndex()))
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(73);
		}
		else if (6.f > m_pJudge->Get_Distance() && (74 != _pModel->Get_AnimIndex()))
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(74);
		}

		if (73 == _pModel->Get_AnimIndex())
		{
			switch (_pModel->Get_AnimationCurrentKeyFrame())
			{
			case 0:
				m_fSpeed = 8.F;
				break;
			case 1:
				m_fSpeed = 8.F;
				break;
			case 2:
				m_fSpeed = 7.F;
				break;
			case 3:
				m_fSpeed = 4.6F;
				break;
			case 4:
				m_fSpeed = 0.F;
				break;
			case 5:
				m_fSpeed = 0.F;
				break;
			case 6:
				m_fSpeed = 0.F;
				break;
			case 7:
				m_fSpeed = 0.F;
				break;
			case 8:
				m_fSpeed = 4.6F;
				break;
			case 9:
				m_fSpeed = 7.F;
				break;
			case 10:
				m_fSpeed = 7.F;
				break;
			case 11:
				m_fSpeed = 7.F;
				break;
			case 12:
				m_fSpeed = 7.F;
				break;
			case 13:
				m_fSpeed = 7.F;
				break;
			case 14:
				m_fSpeed = 7.F;
				break;
			case 15:
				m_fSpeed = 3.F;
				break;
			case 16:
				m_fSpeed = 0.F;
				break;
			case 17:
				m_fSpeed = 3.F;
				break;
			case 18:
				m_fSpeed = 3.F;
				break;
			case 19:
				m_fSpeed = 7.F;
				break;
			case 20:
				m_fSpeed = 7.F;
				break;
			case 21:
				m_fSpeed = 7.F;
				break;
			case 22:
				m_fSpeed = 4.6F;
				break;
			case 23:
				m_fSpeed = 4.6F;
				break;
			}
		}
		else  if (74 == _pModel->Get_AnimIndex())
		{
			switch (_pModel->Get_AnimationCurrentKeyFrame())
			{
			case 0:
				m_fSpeed = 4.5F;
				break;
			case 1:
				m_fSpeed = 4.5F;
				break;
			case 2:
				m_fSpeed = 4.5F;
				break;
			case 3:
				m_fSpeed = 0.F;
				break;
			case 4:
				m_fSpeed = 0.F;
				break;
			case 5:
				m_fSpeed = 0.F;
				break;
			case 6:
				m_fSpeed = 2.5F;
				break;
			case 7:
				m_fSpeed = 2.5F;
				break;
			case 8:
				m_fSpeed = 4.5F;
				break;
			case 9:
				m_fSpeed = 4.5F;
				break;
			case 10:
				m_fSpeed = 4.5F;
				break;
			case 11:
				m_fSpeed = 6.F;
				break;
			case 12:
				m_fSpeed = 3.75F;
				break;
			case 13:
				m_fSpeed = 4.5F;
				break;
			case 14:
				m_fSpeed = 3.75F;
				break;
			case 15:
				m_fSpeed = 3.F;
				break;
			case 16:
				m_fSpeed = 2.5F;
				break;
			case 17:
				m_fSpeed = 2.5F;
				break;
			case 18:
				m_fSpeed = 2.5F;
				break;
			case 19:
				m_fSpeed = 1.875F;
				break;
			case 20:
				m_fSpeed = 3.75F;
				break;
			case 21:
				m_fSpeed = 4.5F;
				break;
			case 22:
				m_fSpeed = 3.75F;
				break;
			case 23:
				m_fSpeed = 4.5F;
				break;
			}
		}
		if (73 == _pModel->Get_AnimIndex())
			_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*m_fSpeed);
		else if (74 == _pModel->Get_AnimIndex())
			_pTransform->Add_Velocity(CTransform::STATE_LOOK, -(_float)TimeDelta*m_fSpeed);

		return true;
	}
}

CRun_Judge_UA * CRun_Judge_UA::Create(CJudge * _pJudge)
{
	CRun_Judge_UA* pInstance = new CRun_Judge_UA(_pJudge);

	return pInstance;
}

void CRun_Judge_UA::Free()
{
}
