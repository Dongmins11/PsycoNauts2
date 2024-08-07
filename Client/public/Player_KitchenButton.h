#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_KitchenButton final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_KitchenButton)

#define DEFAULT_JUMP_POWER 10.f

public:

private:
	explicit CPlayer_KitchenButton();
	virtual ~CPlayer_KitchenButton() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	First_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Key_Check_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Fall_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Land_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	void	Update_Player_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

	_bool	RepeatJump(CPlayer_AnimState* pPlayer_AnimState);
	void	Start_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	SetUp_One_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);

	_bool	KitchenButton_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	KitchenButton_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	void	LandTimeCheck(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);

private:
	void	Jump_Initialize(CPlayer_AnimState * pPlayer_AnimState);


private:
	_bool		m_bFirstCheck = false;
	_bool		m_bBeforeJumping = false;

	_uint		m_iJumpCount = 0;
	_float		m_fJumpPower = 0.f;

	_bool		m_bRepeat = false;
	_bool		m_bLandCheck = false;
	_double		m_dLandTimeAcc = 0.0;

	_vector		vArrivePoint = XMVectorZero();

	_uint		m_iRootPreIndex = 0;
	_bool		m_bLandChecking = false;
public:
	virtual void Free() override;

};

END