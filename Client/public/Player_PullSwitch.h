#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_PullSwitch final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_PullSwitch)
private:
	explicit CPlayer_PullSwitch();
	virtual ~CPlayer_PullSwitch() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	First_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Enter_to_Idle_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Lerp_LookChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_vector Lerp_Look_State(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
private:
	_bool	Key_Check_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	PullSwitch_to_IdleChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bIsLerpState = false;
	_uint		m_iTickCount = 0;
public:
	virtual void Free() override;

};

END