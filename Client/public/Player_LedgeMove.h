#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_LedgeMove final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_LedgeMove)
private:
	explicit CPlayer_LedgeMove();
	virtual ~CPlayer_LedgeMove() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void		Set_FirstCheck(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Set_MoveDirection(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Set_Move_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Set_Key_AnimationChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bLeftMoveCheck = false;
	_bool		m_bRightMoveCheck = false;
public:
	virtual void Free() override;

};

END