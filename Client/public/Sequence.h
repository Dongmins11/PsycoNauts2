#pragma once

#include "CompositeNode.h"

BEGIN(Client)
class CSequence final : public CCompositeNode
{
protected:
	CSequence();
	virtual ~CSequence() = default;

public:
	virtual bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* pController = nullptr) override;

public:
	static CSequence* Create();
	virtual void Free()override;

};

END