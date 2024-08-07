#pragma once
#include "BTNode.h"
#include "GoatFord.h"

BEGIN(Client)
class CHit_GoatF : public CBTNode
{
private:
	explicit CHit_GoatF(class CGoatFord* _pGoat);
	virtual ~CHit_GoatF() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;
	void		ResetHit();

private:
	CGoatFord* m_pGoat = nullptr;
	_bool		m_IsHitStart = false;
	_float      m_fScale = 1.f;
	_float      m_fHeadScale = 1.f;

public:
	static CHit_GoatF* Create(CGoatFord* _pGoat);
	virtual void Free()override;
};

END
