#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Jump final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Jump)
public:
	enum JUMP_CHECK
	{
		JUMP_LEFT, JUMP_RIGHT, JUMP_NOMAL, JUMP_CHECK_END
	};
	enum JUMP_STATE
	{
		ONE_JUMP_STATE, DOUBLE_JUMP_STATE, JUMP_STATE_END
	};
private:
	explicit CPlayer_Jump();
	virtual ~CPlayer_Jump() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

public: /* 내가 블렌더점프에서 온 녀석인거를 알려줄 불변수세팅*/
	void	Set_BlendJumpTest(_bool bTset) { m_bBlendJump = bTset; }
	void	Set_GroundSlide(_bool bGroundSlide) { m_bGroundSlide = bGroundSlide; }


private:
	void		Hold(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Double_Jump(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Fall(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Land(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	_bool		Jump_to_Ledge(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump_to_Slider(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump_to_DownAttack(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Fall_While_Jumping(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Land_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Land_to_Run(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Fall_to_GroundSlide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	//준표
	_bool		Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Land_to_RailSlide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	//성준
	_bool		Jump_to_Blender(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump_to_Cloud(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	//형석
	_bool		Jump_to_Bouncer(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump_to_KitchenButton(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void		Slider_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Ledge_Reset(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Set_BlendSpeed();
	void		 First_JumpStateCheck();
	void		Random_JumpStateCheck();
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
private:
	_double		m_lfJumpMoveValue = 0.0;

private:
	_bool		m_bBlendJump = false; //블렌드 점프에서왔다~
	_bool		m_bGroundSlide = false;

public:
	virtual void Free() override;

};

END