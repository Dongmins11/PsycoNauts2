#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CAttack_Censor : public CBTNode
{
private:
	explicit CAttack_Censor(class CCensor* _pCensor);
	virtual ~CAttack_Censor() = default;

public:
	CGameObject* Get_Donut() { return m_pDonut; }

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor* m_pCensor = nullptr;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsDelayAttack = false;
	_bool		m_isFinishedAttack = false;
	_double		m_dDelayTime = 0.0;
	_uint		m_iAttackType = 0;
	_float		m_fDistance = 0.f;
	_bool		m_IsMove = false;
	_bool		m_IsRenderEffect = false;
	_bool		m_IsRenderModelEffect = false;
	_double		m_dSpeed = 1.0;

private:
	CGameObject* m_pDonut = nullptr;

public:
	void Reset_AttackState();
	void Clear_Effect();

private:
	void Reset();

private:
	CCensor::JUMPINFO	m_tJumpInfo;

public:
	static CAttack_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END