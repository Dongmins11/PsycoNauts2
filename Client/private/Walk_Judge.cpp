#include "stdafx.h"
#include "..\public\Walk_Judge.h"

CWalk_Judge::CWalk_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CWalk_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pJudge->Get_Hp() ||
		100 != m_pJudge->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (44 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(44);
		}

		switch (_pModel->Get_AnimationCurrentKeyFrame())
		{
		case 0:
			m_fSpeed = 5.F;
			break;
		case 1:
			m_fSpeed = 5.F;
			break;
		case 2:
			m_fSpeed = 2.5F;
			break;
		case 3:
			m_fSpeed = 2.5F;
			break;
		case 4:
			m_fSpeed = 2.5F;
			break;
		case 5:
			m_fSpeed = 2.5F;
			break;
		case 6:
			m_fSpeed = 0.F;
			break;
		case 7:
			m_fSpeed = 0.F;
			break;
		case 8:
			m_fSpeed = 0.F;
			break;
		case 9:
			m_fSpeed = 0.F;
			break;
		case 10:
			m_fSpeed = -1.25F;
			break;
		case 11:
			m_fSpeed = -1.25F;
			break;
		case 12:
			m_fSpeed = -2.5F;
			break;
		case 13:
			m_fSpeed = 7.F;
			break;
		case 14:
			m_fSpeed = 7.F;
			break;
		case 15:
			m_fSpeed = 7.F;
			break;
		case 16:
			m_fSpeed = 2.5F;
			break;
		case 17:
			m_fSpeed = 5.F;
			break;
		case 18:
			m_fSpeed = 5.F;
			break;
		case 19:
			m_fSpeed = 3.25F;
			break;
		case 20:
			m_fSpeed = 2.5F;
			break;
		case 21:
			m_fSpeed = 2.5F;
			break;
		case 22:
			m_fSpeed = 2.5F;
			break;
		case 23:
			m_fSpeed = 0.F;
			break;
		case 24:
			m_fSpeed = 0.F;
			break;
		case 25:
			m_fSpeed = 0.F;
			break;
		case 26:
			m_fSpeed = 0.F;
			break;
		case 27:
			m_fSpeed = 0.F;
			break;
		case 28:
			m_fSpeed = 0.F;
			break;
		case 29:
			m_fSpeed = 0.F;
			break;
		case 30:
			m_fSpeed = 0.F;
			break;
		case 31:
			m_fSpeed = 0.F;
			break;
		case 32:
			m_fSpeed = 9.F;
			break;
		case 33:
			m_fSpeed = 7.F;
			break;
		case 34:
			m_fSpeed = 5.F;
			break;
		case 35:
			m_fSpeed = 5.F;
			break;
		case 36:
			m_fSpeed = 0.F;
			break;
		}

		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*m_fSpeed);

		_tchar sz[MAX_PATH] = L"";
		swprintf_s(sz, L"%d", _pModel->Get_AnimationCurrentKeyFrame());
		SetWindowText(g_hWnd, sz);

		return true;
	}
}

CWalk_Judge * CWalk_Judge::Create(CJudge * _pJudge)
{
	CWalk_Judge* pInstance = new CWalk_Judge(_pJudge);

	return pInstance;
}

void CWalk_Judge::Free()
{
}
