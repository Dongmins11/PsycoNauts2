#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)
class CBTNode abstract : public CBase
{
protected:
	CBTNode();
	virtual ~CBTNode() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* pController = nullptr) = 0;

public:
	virtual void Free()override;
};

END