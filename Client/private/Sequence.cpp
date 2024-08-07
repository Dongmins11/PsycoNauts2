#include "stdafx.h"
#include "..\Public\Sequence.h"


CSequence::CSequence()
	:CCompositeNode()
{
}

bool CSequence::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	for (auto node : GetChildren())
	{
		if (!node->Invoke(TimeDelta, _pModel, _pTransform, _pController))
			return false;
	}
	return true;
}

CSequence * CSequence::Create()
{
	CSequence* pInstance = new CSequence();

	return pInstance;
}

void CSequence::Free()
{
	__super::Free();
}
