#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CHit_BadIdea : public CBTNode
{
private:
	explicit CHit_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CHit_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;
	_bool		m_IsHit = false;
	_bool		m_IsRecovery = true;
	_bool		m_IsRecoveryLoop = false;

public:
	static CHit_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END