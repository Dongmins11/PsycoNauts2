#pragma once

#include "Player_ParentAnim.h"
#include "RailSlide.h"
BEGIN(Client)

class CPlayer_RailSlide final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_RailSlide)

	enum CHNAGE_ANIM
	{
		CHANGE_ANIM_JUMP, CHANGE_ANIM_END
	};
private:
	explicit CPlayer_RailSlide();
	virtual ~CPlayer_RailSlide() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;

public:/*For.Set*/
	void	Set_RailValues(_double dStartSpeed, _double dDefaultSpeed, _double dFastSpeed, _double dSlowSpeed, _double dNonFixJumpPower,
		_double dRailCollisionCheckTerm, _double dJumpCorrection, _double dStartJumpAccMax, _double dRayTrans, _double dJumpPower,
		_double dJumpFallSpeed, _double dLookSpeed, _double dMoveSpeed, _double dAdjustSpeed);

private:
	void	First_State(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Separation_Motion(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void	Set_BlendSpeed(_double TimeDelta);
	_bool	Set_GravityChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Hit_MotionChange(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool	Run_To_Ground_Slide(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
private:
	void		Effect_Create(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void		Effect_Release(CPlayer_AnimState * pPlayer_AnimState);

private:
	/*이동에 필요한 변수들 셋팅(Look,가운데 지점값, 충돌Rail체크)*/
	void		SetUp_Values(CPlayer_AnimState * pPlayer_AnimState,_double TimeDelta);
	void		Compute_AdjustDestPos(CRailSlide* pRailObject);
	_bool		RailRayCast(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);

	/*애니메이션 변환체크(점프 후 착지했을 때에 Rail충돌체크 , 점프 안하고 착지했을 때에 Rail충돌체크)*/
	_bool		ChangeAnim(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Fall(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	_bool		Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta);
	void		SetUp_IsCollideRailSlide(CPlayer_AnimState * pPlayer_AnimState);
	void		IsStartJump(_double TimeDelta);
	void		IsNotRailLand(CPlayer_AnimState * pPlayer_AnimState);

	/*Lerp*/
	void		Lerp_Speed(_double TimeDelta);
	void		Lerp_Look(_double TimeDelta);
	void		Lerp_AdjustPos(_double TimeDelta);

	/*이동*/
	void		Move(_double TimeDelta);
	void		Adjust_To_Center(_double TimeDelta);

	/*값 셋팅*/
	void		SetUp_Movement(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc);
	void		SetUp_Double_Jump_Escape(CPlayer_AnimState * pPlayer_AnimState);
	void		SetUp_Double_Jump_NonEscape(CPlayer_AnimState * pPlayer_AnimState);
	void		SetUp_Speed(_uint iKey,_double dSpeed);
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bStartCheck = false;
	_bool		m_bMotionCheck = false;

	_double		m_lfJumpTimeAcc = 0.0;
	_double		m_lfFallSpeed = 1.f;
	_float		m_fJumpPower = 0.f;
	_float		m_fBackUpY = 0.f;
	_float		m_fEndY = 0.f;
	_float		m_fJumpValue = 0.f;

private:/*For. Move*/
	_float3					m_vRailDir = _float3(0.f, 0.f, 0.f);
	_float3					m_vBackUpObjectPos = _float3(0.f, 0.f, 0.f);
	_bool					m_bAdjustDirRight = false;
	_bool					m_bAdjustStart = false;
	CRailSlide*				m_pBackUpRailObject = nullptr;
	_double					m_dRailTimeAcc = 0.0;
	_bool					m_bRailChangeSign = false;
	_bool					m_bStartJump = false;
	_double					m_dStartJumpAcc = 0.0;

	_double					m_dMoveSpeed = 1.0;
	_float3					m_vLook = _float3(0.f, 0.f, 0.f);
	_float3					m_vAdjustPos = _float3(0.f, 0.f, 0.f);
	_float3					m_vSourAdjustPos = _float3(0.f, 0.f, 0.f);
	_float3					m_vSourLook = _float3(0.f, 0.f, 0.f);

	_bool					m_bLerpSpeed = false;
	_bool					m_bLerpLook = false;
	_bool					m_bLerpAdjust = false;

	_double					m_dLerpSpeedRatio = 0.0;
	_double					m_dLerpLookRatio = 0.0;
	_double					m_dLerpAdjustRatio = 0.0;

	_double					m_dLerpSpeed_Speed = 1.0;
	_double					m_dLerpLook_Speed = 20.0;
	_double					m_dLerpAdjust_Speed = 20.0;

	_double					m_dDestSpeed = 0.0;
	_float3					m_vDestLook = _float3(0.f, 0.f, 0.f);
	_float3					m_vDestAdjustPos = _float3(0.f, 0.f, 0.f);

	_uint					m_iCurKey = 0;
	_uint					m_iCurAnimKey = 0;
	_bool					m_bIsJump = false;
	_bool					m_bIsNotRailLand = false;
	_uint					m_iCount = 0;
	_bool					m_bIsChangeLook = false;

	_float3					m_BackUpPos=_float3(0.f,0.f,0.f);

	_bool					m_bRay_ChangeRail = false;
	_double					m_dRayTimeAcc = 0.0;
	//Imgui 상수
	_double					m_dNonFixJumpPower = 1.5f;
	_double					m_dStartSpeed = 0.025;
	_double					m_dDefaultSpeed = 0.6;
	_double					m_dFastSpeed = 1.3;
	_double					m_dSlowSpeed = 0.3;
	_double					m_dRailCollisionCheckTerm = 2.0;
	_double					m_dJumpCorrection = 1.7;
	_double					m_dStartJumpAccMax = 0.1;
	_double					m_dRayTrans = 0.08;

	//++ 점프 파워, Fall 가속도, Fall Speed

	//Effect
	_bool					m_isEffectCreate = false;
public:
	virtual void Free() override;

};

END