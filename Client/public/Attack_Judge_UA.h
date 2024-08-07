#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CAttack_Judge_UA : public CBTNode
{
private:
	explicit CAttack_Judge_UA(class CJudge* _pJudge);
	virtual ~CAttack_Judge_UA() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

public:
	void		Reset_AttackState();

private:
	CJudge* m_pJudge = nullptr;
	CGameObject* m_pBook = nullptr;
	_bool		m_IsRecovery = false;
	_bool		m_IsAttack = false;
	_bool		m_IsAttackReady = false;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsDelayAttack = false;
	_bool		m_IsNearAttack = false;
	_bool		m_IsFarAttack = false;
	_float		m_fSpeed = 0.f;
	_double		m_dDelayTime = 0.0;

public:
	static CAttack_Judge_UA* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END