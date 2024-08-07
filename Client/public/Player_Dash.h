#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Dash final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Dash)
private:
	explicit CPlayer_Dash();
	virtual ~CPlayer_Dash() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bFirstDashCheck = false;
	_bool		m_bDashAttackCheck = false;
public:
	virtual void Free() override;

};

END