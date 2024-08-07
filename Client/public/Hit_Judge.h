#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CHit_Judge : public CBTNode
{
private:
	explicit CHit_Judge(class CJudge* _pJudge);
	virtual ~CHit_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool		m_IsHit = false;
	_bool		m_IsRecovery = true;
	_bool		m_IsRecoveryLoop = false;

public:
	static CHit_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END