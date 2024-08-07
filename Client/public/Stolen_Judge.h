#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CStolen_Judge : public CBTNode
{
private:
	explicit CStolen_Judge(class CJudge* _pJudge);
	virtual ~CStolen_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool	m_IsStolen = false;

public:
	static CStolen_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END