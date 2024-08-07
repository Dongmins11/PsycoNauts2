#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CDead_Censor : public CBTNode
{
private:
	explicit CDead_Censor(class CCensor* _pCensor);
	virtual ~CDead_Censor() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

public:
	void Reset_DeadState();

private:
	CCensor* m_pCensor = nullptr;
	_bool m_IsReadyDead= false;
	_double m_dAcc = 0.0;
	_double m_dPower = 10.0;

public:
	static CDead_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END