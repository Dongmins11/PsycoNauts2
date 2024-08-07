#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_PullSwitch_Move final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_PullSwitch_Move)
private:
	explicit CPlayer_PullSwitch_Move();
	virtual ~CPlayer_PullSwitch_Move() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Set_MoveDirection(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	_bool	Key_Check_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END