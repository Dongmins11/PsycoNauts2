#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_LedgeJump final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_LedgeJump)
private:
	explicit CPlayer_LedgeJump();
	virtual ~CPlayer_LedgeJump() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	First_Check_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Key_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Jump_To_StopMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Jump_To_Landing(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Land_To_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Land_To_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void	Animation_BlendCheck(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void	Jump_Reset(CPlayer_AnimState* pPlayer_AnimState,_double TimeDelta);
private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bJumpEndCheck = false;
private:
	_float		m_fJumpPower = 0.f;
public:
	virtual void Free() override;

};

END