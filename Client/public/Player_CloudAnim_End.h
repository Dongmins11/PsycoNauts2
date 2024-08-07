#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_CloudAnim_End final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_CloudAnim_End)

private:
	explicit CPlayer_CloudAnim_End();
	virtual ~CPlayer_CloudAnim_End() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	First_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Enter_Motion_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);


private:
	_bool	Player_CloudAnim_End_TO_Player_CloudAnim(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	
private:
	_bool		m_bFristCheck = false;
	_bool		m_bEnterMotionCheck = false;
	_bool		m_bNext_CloudCheck = false;
	_uint		m_iPreRootIndex = 0;


public:
	virtual void Free() override;
};

END