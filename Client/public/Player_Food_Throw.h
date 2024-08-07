#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Food_Throw final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Food_Throw)
public:
	enum THROW_TYPE {
		THROW_TYPE_LEFT,
		THROW_TYPE_RIGHT,
		THROW_TYPE_END
	};

private:
	explicit CPlayer_Food_Throw();
	virtual ~CPlayer_Food_Throw() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	Set_First_State(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Look_Lerp(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Throw_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	_bool		m_bFirstCheck = false;
	_uint		m_iAttackCount = 0;
	THROW_TYPE	m_eTrowType = THROW_TYPE_END;
public:
	virtual void Free() override;

};

END