#include "stdafx.h"
#include "..\public\Chase_Doubt.h"

CChase_Doubt::CChase_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CChase_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 > m_pDoubt->Get_Hp())		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야 시야각 10도 안에 들어와있어야 true
	{
		return true;
	}

	else
	{
		m_pDoubt->Update_LookVector(TimeDelta);
		return true;
	}
}

CChase_Doubt * CChase_Doubt::Create(CDoubt * _pDoubt)
{
	CChase_Doubt* pInstance = new CChase_Doubt(_pDoubt);

	return pInstance;
}

void CChase_Doubt::Free()
{
	__super::Free();
}
