#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CIdle_Judge : public CBTNode
{
private:
	explicit CIdle_Judge(class CJudge* _pJudge);
	virtual ~CIdle_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;

public:
	static CIdle_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END