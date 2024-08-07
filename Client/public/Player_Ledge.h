#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Ledge final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Ledge)
private:
	explicit CPlayer_Ledge();
	virtual ~CPlayer_Ledge() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void		Set_FirstCheck(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		Set_Loop_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		GrabDown_To_Falling(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Ledge_to_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	_vector		SetUp_LerpPosition(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	_vector		SetUp_LerpLook(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		SetUp_LerpState(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);

private:/*For. Camera*/
	void	Set_Signal_Ledge();
	void	Set_Signal_Exit_Up();
	void	Set_Signal_Exit_Down();

private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bIsLerpState = false;
	_bool		m_bFirstCheck = false;
private:
	_bool		m_bFirstLerpCheck = false;
	_float4		m_vLerpPosition = {};
	_float4		m_vTriggerPosition = {};
private:
	_uint		m_iTestTickCount = 0;
public:
	virtual void Free() override;

};

END