#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CChase_Judge : public CBTNode
{
private:
	explicit CChase_Judge(class CJudge* _pJudge);
	virtual ~CChase_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool m_IsSpawned = false;

public:
	static CChase_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END