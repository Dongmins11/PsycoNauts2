#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CRun_Censor : public CBTNode
{
private:
	explicit CRun_Censor(class CCensor* _pCensor);
	virtual ~CRun_Censor() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor* m_pCensor = nullptr;

public:
	static CRun_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END