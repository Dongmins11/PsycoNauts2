#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Walk final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Walk)
private:
	explicit CPlayer_Walk();
	virtual ~CPlayer_Walk() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Walk_to_Effect_Create(_double TimeDelta);
private:
	void		Setting_Blend_Speed();
private:
	_uint		m_iRootPreIndex = 0;
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END