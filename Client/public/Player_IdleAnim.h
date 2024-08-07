
#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_IdleAnim final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_IdleAnim)
private:
	explicit CPlayer_IdleAnim();
	virtual ~CPlayer_IdleAnim() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
public:
	void	Walk_to_Run_SettingCheck(_bool bSettingCheck) { m_bSettingCheck = bSettingCheck; }
private:
	void	Reset_Check();
	void	Set_BlendSpeed(_double TimeDelta);
	void	Set_RandomIdleMotion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Grab_RayCastingLoop(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	_bool	Idle_to_StateChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Set_GravityChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Hit_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Idle_To_Bouncer(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Idle_To_Cloud(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	_bool		m_bSettingCheck = false;
	_bool		m_bRandomChangeCheck = false;
	_uint		m_iRandomValue = 0;
	_double		m_lfChangeTime = 0.0;

private:/*Imgui에서 조작할 때 공격 막기 위해서*/
	_bool		m_bGara = true;

public:
	virtual void Free() override;

};

END