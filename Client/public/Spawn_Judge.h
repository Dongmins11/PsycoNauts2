#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CSpawn_Judge : public CBTNode
{
private:
	explicit CSpawn_Judge(class CJudge* _pJudge);
	virtual ~CSpawn_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool m_IsReadySpawn = false;
	_bool m_IsSpawned = false;
	_bool m_IsCreateInfo = false;
	_bool m_IsCamStart = false;
	_bool m_IsScream = false;

public:
	static CSpawn_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END