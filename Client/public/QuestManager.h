#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Engine_Defines.h"
#include "Quest.h"
#include "QuestUI.h"
BEGIN(Client)

class CQuestManager : public CBase
{
	DECLARE_SINGLETON(CQuestManager)
private:
	CQuestManager();
	virtual ~CQuestManager() = default;

public:
	void Load_QuestDatas(const _tchar* pFilePath);
	CQuest* Setting_QuestToNpc(_uint iQuestNumber);
	void Setting_SelectedQuest(_uint iNumber);
	void Setting_TabQuest(_uint iNumber);
	void Clear_Quest(_uint iQuestNumber);
	void Sorting_Quest();
	void SettingQuestUI(_uint iQuestNumber);
	_uint Battle_MonsterCnt(_uint iQuestNumber,_bool AddCnt = false);
private:
	CQuest* Find_Quest(_uint iQuestNumber);
private:
	vector<CQuest*> m_Quests;
	typedef vector<CQuest*> QUESTS;

private:
	_int m_iSaveArrNumber = -1;

public:
	virtual void Free() override;
};

END