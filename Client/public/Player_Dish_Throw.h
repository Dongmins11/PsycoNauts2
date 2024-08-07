#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Dish_Throw final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Dish_Throw)
private:
	explicit CPlayer_Dish_Throw();
	virtual ~CPlayer_Dish_Throw() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Frist_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END