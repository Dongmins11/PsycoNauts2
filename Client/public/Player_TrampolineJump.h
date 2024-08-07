#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_TrampolineJump final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_TrampolineJump)

#define BASIC_JUMP_POWER 20.f
#define SUPER_JUMP_POWER 35.f
#define ADJUST_SPEED 0.022f
public:
	enum JUMP_CHECK
	{
		JUMP_LEFT, JUMP_RIGHT, JUMP_NOMAL, JUMP_CHECK_END
	};
	enum JUMP_STATE
	{
		ONE_JUMP_STATE, DOUBLE_JUMP_STATE, JUMP_STATE_END
	};
	enum CHNAGE_ANIM
	{
		CHANGE_ANIM_IDLE,CHANGE_ANIM_RUN,CHANGE_ANIM_JUMP,CHANGE_ANIM_LEDGE,CHANGE_ANIM_SLIDER,CHANGE_ANIM_DOWNATTACK, CHANGE_ANIM_END
	};

private:
	explicit CPlayer_TrampolineJump();
	virtual ~CPlayer_TrampolineJump() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void		Hold(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Double_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Fall(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Land(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		RepeatJump(CPlayer_AnimState * pPlayer_AnimState);
	_bool		Jump_to_Ledge(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump_to_Slider(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump_to_DownAttack(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Fall_While_Jumping(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Land_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Land_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void		Slider_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Ledge_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Set_BlendSpeed();
	void		 First_JumpStateCheck();
	void		Random_JumpStateCheck();
	void		Start_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Adjust_StartPos(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		SetUp_Double_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		SetUp_One_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		ChangeAnim(CHNAGE_ANIM eChangeAnim, CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	JUMP_STATE	m_eJumpState = JUMP_STATE_END;
	JUMP_CHECK	m_eJumpCheck = JUMP_CHECK_END;
private:
	_uint		m_iRootPreIndex = 0;
private:
	_uint		m_iRandomState = 0;
	_uint		m_iBackUpJumpState = 0;
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
private:
	_bool		m_bJumpCheck = false;
	_bool		m_bDoubleJumpCheck = false;
	_uint		m_iJumpCount = 0;
private:
	_bool		m_bBackUpCheck = false;
	_bool		m_bLandCheck = false;
	_bool		m_bFallCheck = false;
	_double		m_lfJumpTimeAcc = 0.0;
	_double		m_lfFallSpeed = 1.f;
	_float		m_fJumpPower = 0.f;
	_float		m_fBackUpY = 0.f;
	_float		m_fEndY = 0.f;
	_float		m_fJumpValue = 0.f;
	_double		m_dStartLandTimeAcc = 0.0;
	_bool		m_bBeforeJumping = false;
	_bool		m_bAdjustStartPos = false;
	_bool		m_bJustJumping = false;
public:
	virtual void Free() override;

};

END