#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CJump_Judge : public CBTNode
{
private:
	explicit CJump_Judge(class CJudge* _pJudge);
	virtual ~CJump_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool m_IsSpawned = false;

public:
	static CJump_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END