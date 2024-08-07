#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CWalk_Judge : public CBTNode
{
private:
	explicit CWalk_Judge(class CJudge* _pJudge);
	virtual ~CWalk_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_float	m_fSpeed = 0.f;

public:
	static CWalk_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END