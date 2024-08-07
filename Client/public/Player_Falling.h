#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Falling final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Falling)
private:
	explicit CPlayer_Falling();
	virtual ~CPlayer_Falling() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	_bool		Land_to_Trampoline(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	_uint		m_iRootPreIndex = 0;
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END