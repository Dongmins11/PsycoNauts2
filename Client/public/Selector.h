#pragma once

#include "CompositeNode.h"

BEGIN(Client)
class CSelector final : public CCompositeNode
{
protected:
	CSelector();
	virtual ~CSelector() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* pController = nullptr)override;

public:
	static CSelector* Create();
	virtual void Free()override;
};

END