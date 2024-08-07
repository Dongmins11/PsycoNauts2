#include "stdafx.h"
#include "..\public\Chase_Judge.h"

CChase_Judge::CChase_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CChase_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pJudge->Get_Hp() ||
		(3 <= _pModel->Get_AnimIndex() && 8 >= _pModel->Get_AnimIndex()) ||
		(13 <= _pModel->Get_AnimIndex() && 15 >= _pModel->Get_AnimIndex()) ||
		(3 <= _pModel->Get_AnimIndex() && 8 >= _pModel->Get_AnimIndex()) ||
		52 == _pModel->Get_AnimIndex() ||
		55 == _pModel->Get_AnimIndex())		//10�̸��� �Ÿ��� �������� Idle���¸� ����ž� �þ߰� 10�� �ȿ� �����־�� true
	{
		return true;
	}

	else
	{
		m_pJudge->Update_LookVector(TimeDelta);
		return true;
	}
}

CChase_Judge * CChase_Judge::Create(CJudge * _pJudge)
{
	CChase_Judge* pInstance = new CChase_Judge(_pJudge);

	return pInstance;
}

void CChase_Judge::Free()
{
}
