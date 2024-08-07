#pragma once
#include "BTNode.h"
#include "GoatHollis.h"

BEGIN(Client)
class CHit_GoatH : public CBTNode
{
private:
	explicit CHit_GoatH(class CGoatHollis* _pGoat);
	virtual ~CHit_GoatH() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;
	void		ResetHit();

private:
	CGoatHollis* m_pGoat = nullptr;
	_bool		m_IsHitStart = false;
	_float      m_fScale = 1.f;
	_float      m_fHeadScale = 1.f;

public:
	static CHit_GoatH* Create(CGoatHollis* _pGoat);
	virtual void Free()override;
};

END
