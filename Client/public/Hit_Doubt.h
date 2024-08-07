#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CHit_Doubt : public CBTNode
{
private:
	explicit CHit_Doubt(class CDoubt* _pDoubt);
	virtual ~CHit_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;
	_bool		m_IsHit = false;
	_bool		m_IsRecovery = true;
	_bool		m_IsRecoveryLoop = false;

public:
	static CHit_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END