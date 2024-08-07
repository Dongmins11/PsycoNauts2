#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_DownAttack final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_DownAttack)
private:
	explicit CPlayer_DownAttack();
	virtual ~CPlayer_DownAttack() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Set_Reset_Jump(CPlayer_AnimState * pPlayer_AnimState);
private:
	void	Colider_Switch(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
private:
	void	First_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Start_Motion_Move(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Loop_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	JumpOffset_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	JumpOffset_Falling(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	JumpOffset_Land(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	KeyCheck_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_bool	Land_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Land_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bKeyCheck = false;
	_bool		m_bFirstCheck = false;
	_bool		m_bIsDownAttack = false;
	_bool		m_bIsJump = false;
private:
	_bool		m_bDownAttackGravityCheck = false;
	_bool		m_bMoveCheck = false;
	_bool		m_bLandCheck = false;

public:
	virtual void Free() override;

};

END