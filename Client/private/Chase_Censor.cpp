#include "stdafx.h"
#include "..\public\Chase_Censor.h"

CChase_Censor::CChase_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CChase_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp() ||
		(6 <= _pModel->Get_AnimIndex() && 7 >= _pModel->Get_AnimIndex()) ||
		9 == _pModel->Get_AnimIndex() ||
		12 == _pModel->Get_AnimIndex() ||
		15 == _pModel->Get_AnimIndex() ||
		16 == _pModel->Get_AnimIndex())		//10�̸��� �Ÿ��� �������� Idle���¸� ����ž� �þ߰� 10�� �ȿ� �����־�� true
	{
		return true;
	}
	else
	{
		m_pCensor->Update_LookVector(TimeDelta);
		return true;
	}

}


CChase_Censor * CChase_Censor::Create(CCensor * _pCensor)
{
	CChase_Censor* pInstance = new CChase_Censor(_pCensor);

	return pInstance;
}

void CChase_Censor::Free()
{
	__super::Free();
}
