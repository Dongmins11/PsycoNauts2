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
		// �÷��̾ ������ ����Ʈ�ΰ�?
		_bool bClear = false;
		// ����Ʈ �ѹ�
		_uint iQuestNumber = 0;
		// ���� ����Ʈ �ѹ�
		_uint iLinkNumber = 0;
		// ����Ʈ ���� Npc �ѹ�
		_uint iNpcNumber = 0;
		// ����Ʈ ��ũ��Ʈ(��ȭâ �̹���)���� �ѹ�
		_uint iScriptStartNum = 0;
		// ����Ʈ ��ũ��Ʈ ���� �ѹ�
		_uint iScriptEndNum = 0;
		// �÷��̾� �ϴܿ� ��� ����Ʈ �̹��� �ѹ�
		_uint iQuestContentsDescNumber = 0;
		// ����Ʈ �̹��� ������
		_float2 fContentSizes = { 227.f,297.f };
		// �̿� ��
		_uint iEctNumber = 0;
		// ����Ʈ Ÿ��
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