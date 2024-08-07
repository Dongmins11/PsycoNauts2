#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_Blender_Climbing_Jump final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_Blender_Climbing_Jump)
private:
	enum BLENDERJUMP { BLENDERJUMP_UP , BLENDERJUMP_END };
	
	enum CLIMBING_JUMP_STATE { CLIMBING_JUMP_START_Start,
							   CLIMBING_JUMP_STATE_Fall,
							   CLIMBING_JUMP_STATE_HandleInput, 
							   CLIMBING_JUMP_STATE_END };
	
private:
	explicit CPlayer_Blender_Climbing_Jump();
	virtual ~CPlayer_Blender_Climbing_Jump() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

private:
	void Setting_Initialize();
	void First_JumpStateCheck();
	void Fall_Animation();
	void Land_Animation();
	void Set_BlendSpeed(); 

	void Player_Move_NonMoveOffset(_double TimeDelta);
	void Blender_StartAnimation(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);


private:
	_bool	Blender_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Blender_to_Test(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool   Blender_ClimbingJump_to_Blender_Climbing(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool   Blender_ClimbingJump_to_Blender_ClimbingJumpEnd(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);

	void Jump_DOWN(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta); 


private:
	_bool  m_bJumpMenu[BLENDERJUMP_END];  /* Jump의 종류 */
	_bool  m_KeyUpCheck = false;
private:
	_bool	m_bInitialize = false;
	//_bool   m_bFirstCheck = false;
	_bool   m_bAnimation_STATE[CLIMBING_JUMP_STATE_END];
	_bool   m_bHadleInput = false;
private:
	_float		m_fJumpPower = 0.f;
	_uint		m_iRandomState = 0;

	_float3     m_vPlayerCurrentPostion = _float3(0.f, 0.f, 0.f);
	_float3     m_vPlayerNextEndPostion = _float3(0.f, 0.f, 0.f);

	_double     m_fLerpTimer = 0.0;
	_float      m_JumpHeight = 3.f; /* 다음 까지의 위치 */
	_double     m_fAccLerpTimer = 0.0001;
public:
	virtual void Free() override;
};

END