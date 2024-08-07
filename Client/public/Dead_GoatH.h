#pragma once
#include "BTNode.h"
#include "GoatHollis.h"

BEGIN(Client)
class CDead_GoatH : public CBTNode
{
private:
	explicit CDead_GoatH(class CGoatHollis* _pGoat);
	virtual ~CDead_GoatH() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatHollis* m_pGoat = nullptr;
	_bool		m_IsReadyDead = false;
	_float		m_fScale = 1.75f;

public:
	static CDead_GoatH* Create(CGoatHollis* _pGoat);
	virtual void Free()override;
};

END
