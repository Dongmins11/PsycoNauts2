#pragma once

#include "BTNode.h"
#include "Censor_Burly.h"

BEGIN(Client)
class CJump_CensorB : public CBTNode
{
private:
	explicit CJump_CensorB(class CCensor_Burly* _pCensor);
	virtual ~CJump_CensorB() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor_Burly* m_pCensor = nullptr;

public:
	static CJump_CensorB* Create(CCensor_Burly* _pCensor);
	virtual void Free()override;
};

END