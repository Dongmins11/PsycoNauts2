#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CIdle_BadIdea : public CBTNode
{
private:
	explicit CIdle_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CIdle_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;
	_bool m_IsSpawned = false;

public:
	static CIdle_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END