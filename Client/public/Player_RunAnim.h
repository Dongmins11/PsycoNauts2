#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_RunAnim final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_RunAnim)
private:
	explicit CPlayer_RunAnim();
	virtual ~CPlayer_RunAnim() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	First_State(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Effect_Create();
	void	Separation_Motion(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Set_GravityChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Hit_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Run_To_Ground_Slide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Run_To_Bouncer(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Run_To_Blender(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Run_To_Cloud(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	void	Run_to_Effect_Create(_double TimeDelta);
public:
	void	Set_BlendSpeed(_double TimeDelta);
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bEffectSetting = false;
	_bool		m_bStartCheck = false;
	_bool		m_bMotionCheck = false;
	_bool		m_bSoundRunCheck[6] = { false, };
public:
	virtual void Free() override;

};

END