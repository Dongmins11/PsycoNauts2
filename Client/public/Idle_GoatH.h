#pragma once
#include "BTNode.h"
#include "GoatHollis.h"

BEGIN(Client)
class CIdle_GoatH : public CBTNode
{
private:
	explicit CIdle_GoatH(class CGoatHollis* _pGoat);
	virtual ~CIdle_GoatH() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatHollis* m_pGoat = nullptr;

public:
	static CIdle_GoatH* Create(CGoatHollis* _pGoat);
	virtual void Free()override;
};

END
