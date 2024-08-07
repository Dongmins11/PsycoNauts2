#pragma once

#include "BTNode.h"
#include "Judge.h"
BEGIN(Client)
class CAttack_Judge : public CBTNode
{
private:
	explicit CAttack_Judge(class CJudge* _pJudge);
	virtual ~CAttack_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

public:
	void		Reset_AttackState();

private:
	CJudge*		m_pJudge = nullptr;
	_bool		m_IsRecovery = false;
	_bool		m_IsAttack = false;
	_bool		m_IsAttackReady = false;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsDelayAttack = false;
	_bool		m_IsNearAttack = false;
	_bool		m_IsFarAttack = false;
	_float		m_fSpeed = 0.f;
	_uint		m_iAttackType = 0;
	_double		m_dDelayTime = 0.0;
	_bool		m_IsColliderOn = false;

	CGameObject* m_pTrail = nullptr;
public:
	static CAttack_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END