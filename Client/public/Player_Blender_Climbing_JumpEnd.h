#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Blender_Climbing_JumpEnd final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Blender_Climbing_JumpEnd)
private:
	explicit CPlayer_Blender_Climbing_JumpEnd();
	virtual ~CPlayer_Blender_Climbing_JumpEnd() = default;

public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	_bool		m_bFirstCheck = false;
	_float		m_fJumpPower = 0.f;
	_uint		m_iRootPreIndex = 0;
public:
	virtual void Free() override;


};

END