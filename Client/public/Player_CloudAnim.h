#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_CloudAnim final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_CloudAnim)
private:
	enum MOVESTATE{ MOVESTATE_START_POSTION , MOVESTATE_END_POSTION  ,MOVESTATE_END};
	enum LOOKLURP { LOOK_LURP_START, LOOK_LURP_ENDING,LOOK_LURP_END};

private:
	explicit CPlayer_CloudAnim();
	virtual ~CPlayer_CloudAnim() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	First_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Enter_Motion_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	//void	Cloud_to_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	//_bool	Cloud_to_Test(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool Player_CloudAnim_To_Player_CloudAnim_End(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

private:
	void	Player_MovePostion(CPlayer_AnimState * pPlayer_AnimState,_double TimeDelta);

private:
	_bool		m_bFristCheck = false;
	_bool		m_bEnterMotionCheck = false;
	_bool		m_bLookLurp[LOOK_LURP_END]; /* 시작과 끝 보간을 위함 */
	_uint		m_iPreRootIndex = 0;
	
	_double		AccTimer = 0.0;
	_double		m_MoveTimer = 0.0;

	//_double		Look_AccTimer = 0.0;
	_double		Look_Timer = 0.0;


	_float4     m_Move[MOVESTATE_END]; /* 이동을 위함*/

	_float4		m_vUpStart;
	_float4		m_vUpEnd;

	_bool		m_bLerpCheck = true;
	
public:
	virtual void Free() override;

};

END