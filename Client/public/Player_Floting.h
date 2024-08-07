#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Floting final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Floting)
private:
	explicit CPlayer_Floting();
	virtual ~CPlayer_Floting() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	First_Check(CPlayer_AnimState * pPlayer_AnimState);
	void	Floating_LoopMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Floating_FallMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Floating_LandMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Floating_Land_to_idle_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Floating_Land_to_Run_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void	Effect_Create(CPlayer_AnimState * pPlayer_AnimState);
	void	Effect_Release(CPlayer_AnimState * pPlayer_AnimState);

private:
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
	_bool		m_bLoopCheck = false;
	_uint		m_iRootPreMotionIndex = 0;
	_double		m_lfFlotingTimeAcc = 0.0;

private:
	CGameObject*	m_pEffect = nullptr;

public:
	virtual void Free() override;

};

END