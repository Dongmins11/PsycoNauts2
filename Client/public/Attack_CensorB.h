#pragma once

#include "BTNode.h"
#include "Censor_Burly.h"

BEGIN(Client)
class CAttack_CensorB : public CBTNode
{
private:
	explicit CAttack_CensorB(class CCensor_Burly* _pCensor);
	virtual ~CAttack_CensorB() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	void Reset();

public:
	void Reset_AttackState();

private:
	CCensor_Burly* m_pCensor = nullptr;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsDealyAttack = false;
	_bool		m_IsRecovery = false;
	_bool		m_IsAttack = false;
	_bool		m_IsAttackReady1 = false;
	_bool		m_IsAttackReady2 = false;
	_bool		m_IsMove = false;
	_bool		m_IsAttackSure = false;
	_bool		m_IsRenderEffect = false;
	_bool		m_IsPunch[CCensor_Burly::HAND_END];
	_float		m_fDistance = 0.f;
	_double		m_dSpeed = 1.0;
	_double		m_dExhaustTime = 0.0;
	_double		m_dDelayTime = 0.0;
	_double		m_dSpawnDelayTime = 0.0;
	_uint		m_iAttackType = 0;

private:
	CCensor_Burly::JUMPINFO	m_tJumpInfo;

public:
	static CAttack_CensorB* Create(CCensor_Burly* _pCensor);
	virtual void Free()override;
};
 
END