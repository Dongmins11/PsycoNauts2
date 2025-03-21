#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_GrabAttack final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_GrabAttack)
private:
	explicit CPlayer_GrabAttack();
	virtual ~CPlayer_GrabAttack() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
public:
	virtual void Free() override;

};

END