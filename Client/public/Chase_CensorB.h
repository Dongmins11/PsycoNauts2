#pragma once

#include "BTNode.h"
#include "Censor_Burly.h"

BEGIN(Client)
class CChase_CensorB : public CBTNode
{
private:
	explicit CChase_CensorB(class CCensor_Burly* _pCensor);
	virtual ~CChase_CensorB() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor_Burly* m_pCensor = nullptr;

public:
	static CChase_CensorB* Create(CCensor_Burly* _pCensor);
	virtual void Free()override;
};

END