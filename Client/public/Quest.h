#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

class CQuest : public CBase
{
public:
	enum QUSETTYPE { QT_NORMAL, QT_BATTLE, QT_CONVERSATION, QT_END };

public:
	typedef struct QuestDesc
	{
		// 플레이어가 종료한 퀘스트인가?
		_bool bClear = false;
		// 퀘스트 넘버
		_uint iQuestNumber = 0;
		// 다음 퀘스트 넘버
		_uint iLinkNumber = 0;
		// 퀘스트 보유 Npc 넘버
		_uint iNpcNumber = 0;
		// 퀘스트 스크립트(대화창 이미지)시작 넘버
		_uint iScriptStartNum = 0;
		// 퀘스트 스크립트 종료 넘버
		_uint iScriptEndNum = 0;
		// 플레이어 하단에 띄울 퀘스트 이미지 넘버
		_uint iQuestContentsDescNumber = 0;
		// 퀘스트 이미지 사이즈
		_float2 fContentSizes = { 227.f,297.f };
		// 이외 값
		_uint iEctNumber = 0;
		// 퀘스트 타입
		QUSETTYPE eQuestType = QT_END;
	}QUESTDESC;

protected:
	explicit CQuest();
	virtual ~CQuest() = default;

public:
	const QUESTDESC Get_QuestDesc() const { return m_tQuestDesc; }
	const _uint Get_BattleCnt() const { return m_iBattleCnt; }
	void Add_BattleCnt() { m_iBattleCnt += 1; }
protected:
	_bool	m_isClear = false;
	_uint	m_iBattleCnt = 0;
	_uint	m_iMaxBattleCnt = 0;
	QUESTDESC m_tQuestDesc;

public:
	virtual void Free() override;
};

END