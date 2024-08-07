#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Dash_Attack final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Dash_Attack)
private:
	explicit CPlayer_Dash_Attack();
	virtual ~CPlayer_Dash_Attack() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void		Colider_Switch(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
private:
	_uint		m_iRootPreIndex = 0;
	_bool		m_bFirstCheck = false;
	_uint		m_iDashAttackCount = 0;
public:
	virtual void Free() override;

};

END