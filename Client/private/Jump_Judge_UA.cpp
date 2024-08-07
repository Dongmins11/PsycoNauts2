#include "stdafx.h"
#include "..\public\Jump_Judge_UA.h"

CJump_Judge_UA::CJump_Judge_UA(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CJump_Judge_UA::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	return true;
}

CJump_Judge_UA * CJump_Judge_UA::Create(CJudge * _pJudge)
{
	CJump_Judge_UA* pInstance = new CJump_Judge_UA(_pJudge);

	return pInstance;
}

void CJump_Judge_UA::Free()
{
}
