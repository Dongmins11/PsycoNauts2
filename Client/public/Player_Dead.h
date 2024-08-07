#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Dead final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Dead)
private:
	explicit CPlayer_Dead();
	virtual ~CPlayer_Dead() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	First_DeadMotionCheck(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Camera_Back_State(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	End_to_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	_bool		m_bFirstCheck = false;
	_bool		m_bFadeInCheck = false;
private:
	_uint		m_iPreUpperIndex = 0;
public:
	virtual void Free() override;

};

END