#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CChase_Censor : public CBTNode
{
private:
	explicit CChase_Censor(class CCensor* _pCensor);
	virtual ~CChase_Censor() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor* m_pCensor = nullptr;

public:
	static CChase_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END