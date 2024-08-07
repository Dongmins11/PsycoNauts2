#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CRun_BadIdea : public CBTNode
{
private:
	explicit CRun_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CRun_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;

public:
	static CRun_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END