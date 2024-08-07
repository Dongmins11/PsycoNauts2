#include "stdafx.h"
#include "..\public\NormalQuest.h"


CNormalQuest::CNormalQuest(QUESTDESC & tDesc)
	: CQuest()
{
	m_tQuestDesc = tDesc;
	m_iMaxBattleCnt = m_tQuestDesc.iEctNumber;
}

CNormalQuest::~CNormalQuest()
{
	Free();
}

void CNormalQuest::Free()
{
	__super::Free();
}


