#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Blender_Climbing final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Blender_Climbing)
private:
	enum BLENDER_MOVE { BLENDER_MOVE_LEFT , BLENDER_MOVE_RIGHT, BLENDER_MOVE_UP , BLENDER_MOVE_DOWN , BLENDER_MOVE_JUMP, BLENDER_MOVE_END };

private:
	explicit CPlayer_Blender_Climbing();
	virtual ~CPlayer_Blender_Climbing() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void	First_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Enter_Motion_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);


private:
	void	Blender_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Blender_to_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Blender_to_Test(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

	
private:
	void Belnder_Move_DirSetting(BLENDER_MOVE playerMove);


	void Blender_LOOK_Change(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void Blender_ILDE(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void Blender_MOVE(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	

private:
	_bool		m_bFristCheck = false;
	_bool		m_bEnterMotionCheck = false;
	_uint		m_iPreRootIndex = 0;
	_float4      m_MoveDir;

public:
	virtual void Free() override;

};

END