#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CWalk_BadIdea : public CBTNode
{
private:
	explicit CWalk_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CWalk_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;

public:
	static CWalk_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END