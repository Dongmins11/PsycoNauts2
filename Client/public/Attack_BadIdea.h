#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CAttack_BadIdea : public CBTNode
{
private:
	explicit CAttack_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CAttack_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsDealyAttack = false;
	_bool		m_IsNearAttack = false;
	_bool		m_IsFarAttack = false;
	_bool		m_IsThrow = false;
	_bool		m_IsBite = false;
	_double		m_dDelayTime = 0.0;

public:
	static CAttack_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END