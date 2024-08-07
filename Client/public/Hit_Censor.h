#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CHit_Censor : public CBTNode
{
private:
	explicit CHit_Censor(class CCensor* _pCensor);
	virtual ~CHit_Censor() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor* m_pCensor = nullptr;
	_bool		m_IsHit = false;
	_bool		m_IsRecovery = true;
	_bool		m_IsRecoveryLoop = false;

	_bool		m_isEffectCreate = false;
public:
	static CHit_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END