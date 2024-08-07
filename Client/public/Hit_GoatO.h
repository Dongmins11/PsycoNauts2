#pragma once
#include "BTNode.h"
#include "GoatOtto.h"

BEGIN(Client)
class CHit_GoatO : public CBTNode
{
private:
	explicit CHit_GoatO(class CGoatOtto* _pGoat);
	virtual ~CHit_GoatO() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;
	void		ResetHit();

private:
	CGoatOtto* m_pGoat = nullptr;
	_bool		m_IsHitStart = false;
	_float      m_fScale = 1.f;
	_float      m_fHeadScale = 1.f;

public:
	static CHit_GoatO* Create(CGoatOtto* _pGoat);
	virtual void Free()override;
};

END
