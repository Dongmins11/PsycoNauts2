#pragma once
#include "BTNode.h"
#include "GoatHollis.h"

BEGIN(Client)
class CTaunt_GoatH : public CBTNode
{
private:
	explicit CTaunt_GoatH(class CGoatHollis* _pGoat);
	virtual ~CTaunt_GoatH() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatHollis* m_pGoat = nullptr;
	_double		m_dDelayTime = 0.0;

public:
	static CTaunt_GoatH* Create(CGoatHollis* _pGoat);
	virtual void Free()override;
};

END
