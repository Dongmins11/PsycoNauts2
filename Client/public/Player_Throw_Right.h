#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Throw_Right final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Throw_Right)
private:
	explicit CPlayer_Throw_Right();
	virtual ~CPlayer_Throw_Right() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Set_First_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Throw_Idle_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END