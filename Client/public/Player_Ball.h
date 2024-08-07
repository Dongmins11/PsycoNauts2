#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Ball final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Ball)
private:
	explicit CPlayer_Ball();
	virtual ~CPlayer_Ball() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Set_BlendSpeed();
	HRESULT Ball_Idle_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	HRESULT Ball_Interaction_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	HRESULT Ball_Jump_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_uint		m_iRootPreIndex = 0;
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
	_bool		m_bEndCheck = false;
private:
	_double		m_lfBallIdleChangeTimeAcc = 0.0;
	_bool		m_bIdleChangeCheck = false;
private:
	_bool		m_bBackUpCheck = false;
	_bool		m_bJumpCheck = false;
	_bool		m_bLandCheck = false;
	_bool		m_bFallCheck = false;
	_double		m_lfJumpTimeAcc = 0.0;
	_float		m_fJumpPower = 0.f;
	_float		m_fBackUpY = 0.f;
	_float		m_fJumpValue = 0.f;
public:
	virtual void Free() override;

};

END