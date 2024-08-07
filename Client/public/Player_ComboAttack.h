#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_ComboAttack final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_ComboAttack)
private:
	explicit CPlayer_ComboAttack();
	virtual ~CPlayer_ComboAttack() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

public:
	void SetUp_RadianBlur(_uint iMaxCount);
private:
	_uint		m_iAttackCount = 0;
	_bool		m_bFirstCheck = false;
	_bool		m_bComboOne = false;
	_bool		m_bComboTwo = false;
	_bool		m_bComboThree = false;
private:
	_bool		m_bLeftHandSetCheck = false;
	_bool		m_bRightHandSetCheck = false;
private:
	_uint		m_iComboOne_AnimationStopCount = 0;
	_uint		m_iComboTwo_AnimationStopCount = 0;
	_uint		m_iComboThree_AnimationStopCount = 0;

private: /*For. Radian Blur*/
	_bool		m_bActiveRadianBlur = false;
	_uint		m_iRadianCount = 0;
public:
	virtual void Free() override;

};

END