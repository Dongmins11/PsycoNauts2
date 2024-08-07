#pragma once

#include "BTNode.h"
#include "Censor_Burly.h"

BEGIN(Client)
class CWalk_CensorB : public CBTNode
{
private:
	explicit CWalk_CensorB(class CCensor_Burly* _pCensor);
	virtual ~CWalk_CensorB() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor_Burly* m_pCensor = nullptr;

public:
	static CWalk_CensorB* Create(CCensor_Burly* _pCensor);
	virtual void Free()override;
};

END