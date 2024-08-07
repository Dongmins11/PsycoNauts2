#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CIdle_Doubt : public CBTNode
{
private:
	explicit CIdle_Doubt(class CDoubt* _pDoubt);
	virtual ~CIdle_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;

public:
	static CIdle_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END