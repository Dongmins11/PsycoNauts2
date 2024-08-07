#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CAttack_Doubt : public CBTNode
{
private:
	explicit CAttack_Doubt(class CDoubt* _pDoubt);
	virtual ~CAttack_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsDealyAttack = false;
	_bool		m_IsUnArmed = false;
	_double		m_dDelayTime = 0.0;

public:
	static CAttack_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END