#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_WalkStop final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_WalkStop)
private:
	explicit CPlayer_WalkStop();
	virtual ~CPlayer_WalkStop() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void		Setting_Blend_Speed();
private:
	_uint		m_iRootPreIndex = 0;
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END