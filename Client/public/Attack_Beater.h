#pragma once
#include "BTNode.h"
#include "Beater.h"

BEGIN(Client)
class CAttack_Beater : public CBTNode
{
private:
	explicit CAttack_Beater(class CBeater* _pBeater);
	virtual ~CAttack_Beater() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;
	void	Reset_AttackState();

private:
	CBeater* m_pBeater = nullptr;
	_double		m_dDelayTime = 0.0;
	_uint		m_iAttackType = 0;
	_bool		m_IsDelayAttack = false;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsReadyAttack = false;
	_bool		m_IsAttack = false;
	_bool		m_IsRecovery = false;
	_bool		m_IsRenderEffect = false;
	_bool		m_IsDestroyEffect = false;
	_bool		m_IsThrow[5] = { false };


public:
	static CAttack_Beater* Create(CBeater* _pBeater);
	virtual void Free()override;
};

END
