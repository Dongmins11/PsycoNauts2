#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_StopAnim final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_StopAnim)
private:
	explicit CPlayer_StopAnim();
	virtual ~CPlayer_StopAnim() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	_bool	Key_Motion_Change(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

public:
	_bool		m_bFirstCheck = false;
	_bool		m_bHandCheck = false;
public:
	virtual void Free() override;

};

END