#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CJump_Censor : public CBTNode
{
private:
	explicit CJump_Censor(class CCensor* _pCensor);
	virtual ~CJump_Censor() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor* m_pCensor = nullptr;

public:
	static CJump_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END