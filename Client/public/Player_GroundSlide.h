#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_GroundSlide final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_GroundSlide)
private:
	explicit CPlayer_GroundSlide();
	virtual ~CPlayer_GroundSlide() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	First_MotionCheck();
	void	Player_Move_LookUpdate(CPlayer_AnimState * pPlayer_AnimState,const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc,_double TimeDelta);
	_bool	Key_SetUp_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Ground_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

	void	Effect_Create();
private:
	_float	m_fLookFixValueX = 0.f;
	_float	m_fLookFixValueY = 0.f;
	_float	m_fLookFixValueZ = 0.f;
	_bool	m_bFristCheck = false;

	CGameObject* m_pEffect = nullptr;
public:
	virtual void Free() override;

};

END