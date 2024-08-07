#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Slider final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Slider)
public:
	enum EFFECT_TYPE { TYPE_LEFT, TYPE_RIGHT, TYPE_END };
private:
	explicit CPlayer_Slider();
	virtual ~CPlayer_Slider() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	First_Check_AnimMotion(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Slide_Loop_MotionChange(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
	_bool		m_bSlideCheck = false;
public:
	virtual void Free() override;

};

END