#include "stdafx.h"
#include "..\public\QuestManager.h"
#include <fstream>
#include "Quest.h"
#include "NormalQuest.h"
#include "GameInstance.h"
#include "Player.h"
IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

void CQuestManager::Load_QuestDatas(const _tchar * pFilePath)
{
	wifstream fin;
	fin.open(pFilePath);
	_uint iCount = 0;
	_tchar* szBuff = TEXT("");
	m_Quests.resize(10);

	if (!fin.fail())
	{
		while (true)
		{	
			CQuest::QUESTDESC tQuestDesc;

			_uint iType = 0;
			//m_modelPrototypeList
			fin >> tQuestDesc.bClear>> tQuestDesc.iQuestNumber >> tQuestDesc.iLinkNumber >> tQuestDesc.iNpcNumber 
				>> tQuestDesc.iScriptStartNum >> tQuestDesc.iScriptEndNum>> tQuestDesc.iQuestContentsDescNumber >> iType >> tQuestDesc.iEctNumber;
			tQuestDesc.eQuestType = (CQuest::QUSETTYPE)iType;


			CQuest* pQuest = nullptr;
			pQuest = new CNormalQuest(tQuestDesc);
			if (pQuest)
			{
				m_Quests[tQuestDesc.iQuestNumber] = pQuest;
			}

			if (fin.eof())
			{
				break;
			}
		}
		fin.close();
	}

}

CQuest * CQuestManager::Setting_QuestToNpc(_uint iQuestNumber)
{
	return Find_Quest(iQuestNumber);
}

void CQuestManager::Setting_SelectedQuest(_uint iNumber)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	for (_uint i = 0; i < 3; ++i)
	{
		CQuestUI* pUI = (CQuestUI*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestUI"), i);
		if (!pUI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		if (pUI->Get_UIState() == CQuestUI::UISTATE_TAB)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
		if (pPlayer->Get_QuestArr(i) == -1)
			continue;

		if (i == iNumber)
		{
			pUI->Set_ViewZ(2);
			pUI->Set_UIState(CQuestUI::UISTATE_SELECTED);
		}
		else
		{
			pUI->Set_ViewZ(0);
			pUI->Set_UIState(CQuestUI::UISTATE_IDLE);
		}

	}
	RELEASE_INSTANCE(CGameInstance);
}

void CQuestManager::Setting_TabQuest(_uint iNumber)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CQuestUI* pUI = (CQuestUI*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestUI"), iNumber);
	if (!pUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (pPlayer->Get_QuestArr(iNumber) == -1)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if(pUI->Get_UIState() == CQuestUI::UISTATE_TAB)
		pUI->Set_UIState(CQuestUI::UISTATE_SELECTED);
	else
		pUI->Set_UIState(CQuestUI::UISTATE_TAB);

	RELEASE_INSTANCE(CGameInstance);
}

void CQuestManager::Clear_Quest(_uint iQuestNumber)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	_int iFinishQuestArrNumber = -1;
	m_iSaveArrNumber = -1;

	for (_uint i = 0; i < 3; ++i)
	{
		if (pPlayer->Get_QuestArr(i) == iQuestNumber)
		{
			iFinishQuestArrNumber = i;
			pPlayer->Reset_PlayerQuest(i);
			break;
		}

	}
	if(iFinishQuestArrNumber == -1)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CQuestUI* pUI = (CQuestUI*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestUI"), iFinishQuestArrNumber);
	if (!pUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_iSaveArrNumber = iFinishQuestArrNumber;
	pUI->Set_IsFinished(true);

	RELEASE_INSTANCE(CGameInstance);

}

void CQuestManager::Sorting_Quest()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	CQuestUI* pUI[3] = { nullptr };
	for(_uint i=0;i<3;++i)
		pUI[i] = (CQuestUI*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestUI"), i);

	RELEASE_INSTANCE(CGameInstance);

	if (m_iSaveArrNumber != 2 && m_iSaveArrNumber != -1)
	{
		if (m_iSaveArrNumber == 0)
		{
			if(pPlayer->Get_QuestArr(1) == -1 && pPlayer->Get_QuestArr(2) == -1)
				pUI[0]->Set_UIState(CQuestUI::UISTATE_START);
			if (pPlayer->Get_QuestArr(1) != -1)
			{
				pUI[0]->Set_UIState(CQuestUI::UISTATE_IDLE);
				pUI[1]->Set_IsFinished(true);
				pUI[1]->Set_MoveX(true);
			}
			if (pPlayer->Get_QuestArr(2) != -1)
				pUI[2]->Set_IsFinished(true);
			m_iSaveArrNumber = -1;
		}
		else if (m_iSaveArrNumber == 1)
		{
			if (pPlayer->Get_QuestArr(1) != -1)
			{
				pUI[1]->Set_IsFinished(true);
				pUI[1]->Set_MoveX(true);
			}
			if (pPlayer->Get_QuestArr(2) != -1)
				pUI[2]->Set_IsFinished(true);
			m_iSaveArrNumber = -1;
		}
	}
}

void CQuestManager::SettingQuestUI(_uint iQuestArrNumber)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	_int iQuestNumber = pPlayer->Get_QuestArr(iQuestArrNumber);

	if (iQuestNumber == -1)
		iQuestNumber = 0;

	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestUI"), iQuestArrNumber)->Set_RenderPass(iQuestNumber);

	RELEASE_INSTANCE(CGameInstance);

}

_uint CQuestManager::Battle_MonsterCnt(_uint iQuestNumber, _bool AddCnt)
{

	CQuest* pQuest = Find_Quest(iQuestNumber);

	if (pQuest->Get_QuestDesc().eQuestType != 1 || pQuest == nullptr)
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);
	_bool isCorrect = false;
	for (_uint i = 0; i < 3; ++i)
	{
		if (pPlayer->Get_QuestArr(i) == iQuestNumber)
		{
			isCorrect = true;
		}
	}

	if (!isCorrect)
		return 0;

	if(AddCnt)
		pQuest->Add_BattleCnt();

	return pQuest->Get_BattleCnt();
}

CQuest * CQuestManager::Find_Quest(_uint iQuestNumber)
{
	return m_Quests[iQuestNumber];
}

void CQuestManager::Free()
{
	for (auto& quest : m_Quests)
	{
		if(quest != nullptr)
			Safe_Release(quest);

		quest = nullptr;
	}
	m_Quests.clear();
}
