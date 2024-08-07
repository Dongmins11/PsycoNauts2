#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CChase_BadIdea : public CBTNode
{
private:
	explicit CChase_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CChase_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;

public:
	static CChase_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END