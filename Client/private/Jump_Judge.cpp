#include "stdafx.h"
#include "..\public\Jump_Judge.h"

CJump_Judge::CJump_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CJump_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	return true;
}

CJump_Judge * CJump_Judge::Create(CJudge * _pJudge)
{
	CJump_Judge* pInstance = new CJump_Judge(_pJudge);

	return pInstance;
}

void CJump_Judge::Free()
{
}
