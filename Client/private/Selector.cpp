#include "stdafx.h"
#include "..\Public\Selector.h"


CSelector::CSelector()
	:CCompositeNode()
{
}

_bool CSelector::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	for (auto node : GetChildren())
	{
		if (node->Invoke(TimeDelta, _pModel, _pTransform, _pController))
			return true;
	}
	return false;
}

CSelector * CSelector::Create()
{
	CSelector* pInstance = new CSelector();

	return pInstance;
}

void CSelector::Free()
{
}


