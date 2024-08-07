#include "stdafx.h"
#include "..\public\Stolen_Judge.h"

CStolen_Judge::CStolen_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CStolen_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (!m_pJudge->IsArmed()&&!m_IsStolen)
	{
		if (81 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(81);
		}
		if (_pModel->Get_IsAnimFinishied(81))
			m_IsStolen = true;
		return false;
	}
	if (0 >= m_pJudge->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}
	return true;
}

CStolen_Judge * CStolen_Judge::Create(CJudge * _pJudge)
{
	CStolen_Judge* pInstance = new CStolen_Judge(_pJudge);

	return pInstance;
}

void CStolen_Judge::Free()
{
}
