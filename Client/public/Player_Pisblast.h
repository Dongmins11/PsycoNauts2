#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Pisblast final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Pisblast)
private:
	explicit CPlayer_Pisblast();
	virtual ~CPlayer_Pisblast() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void		First_KeyCheck_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Create_BlastBullet();
	void		Cam_To_Look_At(_double TimeDelta);
	void		Charge_Move_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_bool		Quick_Fire_to_RunMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Fire_To_Idle_Motion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Move_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_bool		m_bKeyCheck = false;
	_bool		m_bChargeCheck = false;
	_bool		m_bEndCheck = false;
	_bool		m_bCreateCheck = false;
private:
	_double		m_lfTimeAcc = 0.0;
private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bRootHoldCheck = false;
	_uint		m_iCount = 0;
	_bool		m_bFirstCheck = false;
public:
	virtual void Free() override;

};

END