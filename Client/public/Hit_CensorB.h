#pragma once

#include "BTNode.h"
#include "Censor_Burly.h"

BEGIN(Client)
class CHit_CensorB : public CBTNode
{
private:
	explicit CHit_CensorB(class CCensor_Burly* _pCensor);
	virtual ~CHit_CensorB() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor_Burly* m_pCensor = nullptr;
	_bool		m_IsHit = false;
	_bool		m_IsRecovery = true;
	_bool		m_IsRecoveryLoop = false;

public:
	static CHit_CensorB* Create(CCensor_Burly* _pCensor);
	virtual void Free()override;
};

END