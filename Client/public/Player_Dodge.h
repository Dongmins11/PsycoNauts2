#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Dodge final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Dodge)
private:
	explicit CPlayer_Dodge();
	virtual ~CPlayer_Dodge() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	First_Check_AnimMotion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Dodge_Loop_MotionCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Falling_MotionCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Landing_MotionCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	_bool	Landing_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Landing_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Dodge_to_Slider(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Dodge_to_Double_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Dodge_to_Ledge_Grab(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void	Ledge_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
public:
	virtual void Free() override;
};

END