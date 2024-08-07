#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CDead_BadIdea : public CBTNode
{
private:
	explicit CDead_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CDead_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;
	_bool m_IsReadyDead = false;

public:
	static CDead_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END