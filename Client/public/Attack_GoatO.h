#pragma once
#include "BTNode.h"
#include "GoatOtto.h"

BEGIN(Client)
class CAttack_GoatO : public CBTNode
{
private:
	explicit CAttack_GoatO(class CGoatOtto* _pGoat);
	virtual ~CAttack_GoatO() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

public:
	void		Reset_AttackState();

private:
	CGoatOtto* m_pGoat = nullptr;
	_double		m_dDelayTime = 0.0;
	_double		m_dPukeDelayTime = 0.0;
	_double		m_dTelegraphLoopTime = 0.0;
	_double		m_dStrikeLoopTime = 0.0;
	_uint		m_iAttackType = 0;
	_bool		m_IsDelayAttack = false;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsLoopFinished1 = false;
	_bool		m_IsLoopFinished2 = false;
	_bool		m_IsReadyAttack = false;
	_bool		m_IsAttack = false;
	_bool		m_IsRecovery = false;
	_bool		m_IsPukeDelay = false;
	_bool		m_IsShoot[4] = { false, false, false ,false, };


public:
	static CAttack_GoatO* Create(CGoatOtto* _pGoat);
	virtual void Free()override;
};

END
