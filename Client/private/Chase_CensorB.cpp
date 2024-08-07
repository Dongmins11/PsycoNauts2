#include "stdafx.h"
#include "..\public\Chase_CensorB.h"

CChase_CensorB::CChase_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CChase_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp())		//10�̸��� �Ÿ��� �������� Idle���¸� ����ž� �þ߰� 10�� �ȿ� �����־�� true
	{
		return true;
	}

	else if (2 == _pModel->Get_AnimIndex() ||
		8 < _pModel->Get_AnimIndex() &&
		13 > _pModel->Get_AnimIndex() ||
		13 <= _pModel->Get_AnimIndex() &&
		15 >= _pModel->Get_AnimIndex() ||
		20 == _pModel->Get_AnimIndex() ||
		21 == _pModel->Get_AnimIndex() ||
		42 == _pModel->Get_AnimIndex() ||
		43 == _pModel->Get_AnimIndex())
		return true;

	else
	{
		m_pCensor->Update_LookVector(TimeDelta);
		return true;
	}

}


CChase_CensorB * CChase_CensorB::Create(CCensor_Burly * _pCensor)
{
	CChase_CensorB* pInstance = new CChase_CensorB(_pCensor);

	return pInstance;
}

void CChase_CensorB::Free()
{
	__super::Free();
}
