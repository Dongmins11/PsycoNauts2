#include "stdafx.h"
#include "..\public\Chase_BadIdea.h"

CChase_BadIdea::CChase_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CChase_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBadIdea->Get_Hp())		//10�̸��� �Ÿ��� �������� Idle���¸� ����ž� �þ߰� 10�� �ȿ� �����־�� true
	{
		return true;
	}

	else
	{
		m_pBadIdea->Update_LookVector(TimeDelta);
		return true;
	}
}

CChase_BadIdea * CChase_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CChase_BadIdea* pInstance = new CChase_BadIdea(_pBadIdea);

	return pInstance;
}

void CChase_BadIdea::Free()
{
}
