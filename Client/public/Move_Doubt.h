#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CMove_Doubt : public CBTNode
{
private:
	explicit CMove_Doubt(class CDoubt* _pDoubt);
	virtual ~CMove_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;
	_double m_dSpeed = 1.0;

public:
	static CMove_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END