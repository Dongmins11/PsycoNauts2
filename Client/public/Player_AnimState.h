#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"

#define MAX_KEY 5

BEGIN(Engine)
class CTrasnform;
class CModel;
class CCollider;
class CNavigation;
class CSphereCollider;
END


BEGIN(Client)
class CPlayer_ParentAnim;
class CPlayer_LeftHand;
class CPlayer_RightHand;
class CPlayer_AnimState final : public CBase
{
public:
	enum KEY_STATE { KEY_PRESING, KEY_UP, KEY_DOWN, KEY_END };
public:
	typedef struct tagAnimDesc
	{
		_uint iKey[MAX_KEY];
		_uint eKeyState[MAX_KEY];
		_uint iKeyCount;
		_uint iOnceKey;
		_uint eOneceKeyState;
	}ANIMDESC;

private:
	explicit CPlayer_AnimState();
	virtual ~CPlayer_AnimState() = default;
public:
	HRESULT NativeConstruct(CPlayer* pPlayer, CTransform* pPlayerTrasnform, CModel* pPlayerModel, CSphereCollider* pPlayerColider, CNavigation* pPlayerNavigation, CCharacterController* pCharacterController, CPlayer_LeftHand* pLeftHand, CPlayer_RightHand* pRightHand, CPlayerTrigger_Parkour* pTriggerParkour, _uint iScene);

public:	/* For LerpCheck */
	void	Set_AnimLerpCheck(_bool bLerpCheck) { m_bAnimLerp = bLerpCheck; }
	void	Set_UpperLerpCheck(_bool bLerpCheck) { m_bUpperAnimLerp = bLerpCheck; }
	void	Set_RootLerpCheck(_bool bLerpCheck) { m_bRootAnimLerp = bLerpCheck; }
	void	Set_BottomLerpCheck(_bool bLerpCheck) { m_bBottomAnimLerp = bLerpCheck; }
	void	Set_AllLerpCheckTrue();
	void	Set_RootBottomLerpCheckTrue();
	void	Set_OneLerpCheckTrue();

public:
	void	SetUp_State(CPlayer_ParentAnim* pAnimState);
	void	Update(_double TimeDelta);

public:		/* For.Grab_PhysX */
	void	Grab_RayCast_Ready(_double TimeDelta);
	void	Grab_RayCast_Loop(_double TimeDelta);
	void	Grab_RayCasting();

			/* For.Cloud_PhysX */
	_bool	Cloud_RayCast_Ready(_double TimeDelta);



private:	/* For.GrabPosSet */
	void	Set_Position_RightHand(_double TimeDelta);
private:
	void	Player_Jump(_double TimeDelta);
	void	Player_Gravity(_double TimeDelta);

public:		/* For.Colider */
	void	Set_Coliderenable(_bool IsCheck) { m_IsColiderOn = IsCheck; }
	_bool	Get_ColiderEnable() const { return m_IsColiderOn; }

public:		/* For.Player_DeadCheck*/
	void	Player_Dead(_double TimeDelta);

public:		/* For.Player_HitCheck */
	void	Player_Hit(_bool* bHitCheck, _double TimeDelta);

public:	/*For. Hit_Set */
	void	Set_HitCheck(_bool bHitCheck) { m_bHitCheck = bHitCheck; }

public: /*For. Hit_Get */
	_bool	Get_HitCheck() const { return m_bHitCheck; }
	const _bool Get_HitingCheck() const { return m_IsHiting; }

public: /*For. Hit*/
	void	Hiting_RimLight(_double TimeDelta);

public:/*For. Attack_Get*/
	_bool	Get_AttackCheck() const;
	_double Get_AttackMoveSpeed() const { return m_lfAttackMoveSpeed; }
	_double Get_DashAttackMoveSpeed() const { return m_lfDashAttackMoveSpeed; }

public:	/*For. Attack_Set*/
	void	Set_AttackMoveSpeed(_double lfAttackSpeed) { m_lfAttackMoveSpeed = lfAttackSpeed; }
	void	Set_DashAttackMoveSpeed(_double lfDashAttackSpeed) { m_lfDashAttackMoveSpeed = lfDashAttackSpeed; }
	
public:	/*For. Gravity_Set*/
	void	Set_FloatingSpeed(_float fFloatingSpeed) { m_fFloatingSpeed = fFloatingSpeed; }

public:	/*For. Gravity_Get*/
	_bool	Get_GravityCheck() const { return m_bGravityCheck; }
	_bool	Get_FloatingCheck() const { return m_bFloatingCheck; }

public:		/*For Walking_Check */
	void	Set_WalkingCheck(_bool IsWalk) { m_bWalkingCheck = IsWalk; }
	_bool	Get_WalkingCheck() const { return m_bWalkingCheck; }

public:		/*For.GrabCheck_Set */
	void	Set_GrabCheck(_bool bGrabCheck) { m_bGrabCheck = bGrabCheck; }
	void	Set_GrabSignalCheck(_bool bGrabSignalCheck) { m_bGrabSignalCheck = bGrabSignalCheck; }
	void	Set_GrabAttack(_bool bGrabAttackCheck) { m_bGrabAttack = bGrabAttackCheck; }
	void	Set_GrabBack(_bool bGrabBackCheck) { m_bGrabBack = bGrabBackCheck; }
	void	Set_GrabSuccess(_bool bGrabSuccessCheck) { m_bGrabSuccess = bGrabSuccessCheck; }

public:		/*For.GrabCheck_Get */
	_bool	Get_GrabCheck() const { return m_bGrabCheck; }
	_bool	Get_GrabAttackCheck() const { return m_bGrabAttack; }
	_bool	Get_GrabBackCheck() const { return m_bGrabBack; }
	_bool	Get_GrabSuccessCheck() const { return m_bGrabSuccess; }

public:		/*For. Jump_Set*/
	void	Set_JumpCheck(_bool bJumpCheck) { m_bJumpCheck = bJumpCheck; }
	void	Set_DoubleJumpCheck(_bool bDoubleJumpCheck) { m_bDoubleJumpCheck = bDoubleJumpCheck; }
	void	Set_DownAttackCheck(_bool bDownAttackCheck) { m_bDownAttackCheck = bDownAttackCheck; }
	void	Set_LedgeJumpCheck(_bool bLedgeJumpCheck) { m_bLedgeJumpCheck = bLedgeJumpCheck; }
	//준표
	void	Set_TrampolineJumpCheck(_bool bTrampolineJumpCheck) { m_bTrampolineJumpCheck = bTrampolineJumpCheck; }
	void	Set_TrampolineTriggerCheck(_bool bCheck) { m_bIsCollideTrampolineTrigger = bCheck; }
	void	Set_RailSlideCheck(_bool bCheck) { m_bRailSlideCheck = bCheck; }

	void	Set_FallCheck(_bool bFallCheck) { m_bFallCheck = bFallCheck; }
	void	Set_LandCheck(_bool bLandCheck) { m_bLandCheck = bLandCheck; }
	void	Set_JumpPower(_float fJumpPower) { m_fJumpPower = fJumpPower; }
	void	Set_JumpValue(_float fJumpValue) { m_fJumpTimeValue = fJumpValue; }
	void	Set_BackUpY(_float fPreBackUpJumpY) { m_fBackUpY = fPreBackUpJumpY; }
	void	Set_FallSpeed(_double  bFallSpeed) { m_lfFallSpeed = bFallSpeed; }
	void	Set_JumpTimeAcc(_double lfJumpTimeAcc) { m_lfJumpTimeAcc = lfJumpTimeAcc; }
	void	Set_Fall_AccelerationSpeed(_double lfFallSppedValue) { m_lfFallAcceleration = lfFallSppedValue; }
	void	Set_JumpAcceleration(_double JumpAcceleration) { m_lfJumpAcceleration = JumpAcceleration; }

public:		/*For. Jump_Get*/
	_bool	Get_LandCheck() const { return m_bLandCheck; }
	_bool	Get_FallCheck() const { return m_bFallCheck; }
	_bool	Get_JumpCheck() const { return m_bJumpCheck; }
	_bool	Get_DoubleJumpCheck() const { return m_bDoubleJumpCheck; }
	_bool	Get_DownAttackCheck() const { return m_bDownAttackCheck; }
	_bool	Get_LedgeJumpCheck() const { return m_bLedgeJumpCheck; }
	//준표
	_bool	Get_TrampolineJumpCheck() const { return m_bTrampolineJumpCheck; }
	_bool	Get_TrampolineTriggerCheck() const { return m_bIsCollideTrampolineTrigger; }
	_bool	Get_RailSlideCheck() const { return m_bRailSlideCheck; }
private:	/* For.StateNomal */
	void	Key_Set(_double TimeDelta);
	_bool	Key_Up(_uint _key);
	void	Lerp_Check();

public:	/* For.Slider */
	void	PhysX_Slider();
	void	Set_Slider(_bool IsSlider) { m_isSliding = IsSlider; }
	void	Set_SlideRayCastCheck(_bool IsSlider) { m_isSlidingCheck = IsSlider; }
	void	Set_SliderTriggerCheck(_bool IsSlider) { m_isSlidingTriggerCheck = IsSlider; }

	_bool	Get_Slider() const { return m_isSliding; }
	_bool	Get_SlideRayCastCheck() const { return m_isSlidingCheck; }
	_bool	Get_SliderTriggerCheck()const { return m_isSlidingTriggerCheck; }
public: /*For.Ledge */
	void	Set_LedgeCheck(_bool bLedgeCheck) { m_bLedgeCheck = bLedgeCheck; }
	_bool	Get_LedgeCheck() const { return m_bLedgeCheck; }

public:/*For. Check Animstate*/
	_bool	Check_JumpAnimstate();

public:/*For. CollisionObject*/
	void Set_CollisionObject(CGameObject* pGameObject);
	CGameObject* Get_CollisionObject() { return m_pCollisionObject; }

	void Set_CloudObject(CGameObject* pGameObject);
	CGameObject* Get_CloudObject(){ return m_pCloudHitObject; }

public:/*For. Rail*/
	void	Set_RailValues(_double dStartSpeed, _double dDefaultSpeed, _double dFastSpeed, _double dSlowSpeed, _double dNonFixJumpPower,
		_double dRailCollisionCheckTerm, _double dJumpCorrection, _double dStartJumpAccMax, _double dRayTrans, _double dJumpPower,
		_double dJumpFallSpeed, _double dLookSpeed, _double dMoveSpeed, _double dAdjustSpeed);


private:
	CPlayer*				m_pPlayer = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModel = nullptr;
	CCollider*				m_pColider = nullptr;
	CNavigation*			m_pNavigation = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;
	CPlayerTrigger_Parkour*	m_pTrigger_Parkour = nullptr;
	CSphereCollider*		m_pColliderCom = nullptr;

protected:	// 준표 Collision Object
	CGameObject*			m_pCollisionObject = nullptr;
	/* 성준 Cloud */
	CGameObject*			m_pCloudHitObject = nullptr;



	// RayCast TEST
	_bool				m_isTargetingOn = false;
	CGameObject*		m_pHitObject = nullptr;
	CGameObject*		m_pThrowTargetObject = nullptr;
	_double				m_lfTargetTimeAcc = 0.0;
	_double				m_lfTargetTimeMax = 1.0;
private:
	CPlayer_LeftHand*	m_pLeftHand = nullptr;
	CPlayer_RightHand*	m_pRightHand = nullptr;

private:
	_uint				m_iScene = 0;
	_uint				m_iKeyStack = 0;
private:
	KEY_STATE			m_eKeyState = KEY_END;
	ANIMDESC			m_tAnimDesc;

private:	/* For.AnimLerpCheck */
	_bool				m_bAnimLerp = false;
	_bool				m_bUpperAnimLerp = false;
	_bool				m_bRootAnimLerp = false;
	_bool				m_bBottomAnimLerp = false;

private:	/* For.KeyCheck */
	_byte				m_byKeyState[256];
	_bool				m_bKeyState[256] = {};
private:
	CPlayer_ParentAnim*	m_pPlayer_AnimState = nullptr;

private:	/* For.Jump */
	_bool				m_bJumpCheck = false;
	_bool				m_bDoubleJumpCheck = false;
	_bool				m_bDownAttackCheck = false;
	_bool				m_bLedgeJumpCheck = false;
	//준표
	_bool				m_bTrampolineJumpCheck = false;
	_bool				m_bIsCollideTrampolineTrigger = false;
	_bool				m_bRailSlideCheck = false;


	_bool				m_bFallCheck = false;
	_bool				m_bLandCheck = false;
	_float				m_fBackUpY = 0.f;
	_float				m_fJumpTimeValue = 0.f;
	_float				m_fJumpPower = 0.f;
	_double				m_lfJumpTimeAcc = 0.0;
	_double				m_lfFallSpeed = 0.0;
	_double				m_lfFallAcceleration = 0.0;
	// 형석
	_double				m_lfJumpAcceleration = 0.0;
private:	/* For.Gravity */
	_bool				m_bGravityCheck = false;
	_bool				m_bFloatingCheck = false;
	_float				m_fGravitySpeed = 0.f;
	_float				m_fGravitySpeedOffset = 0.f;
	_float				m_fFloatingSpeed = 0.f;
	_float				m_fMaxSpeed = 0.f;

private:	/* For.Grab */
	_bool				m_bGrabCheck = false;
	_bool				m_bGrabSignalCheck = false;
	_bool				m_bGrabAttack = false;
	_bool				m_bGrabBack = false;
	_bool				m_bGrabSuccess = false;

private:	/* For Walk OR Run */
	_bool				m_bWalkingCheck = false;

private:	/* For.Hit*/
	_bool				m_bHitCheck = false;
	_bool				m_IsHiting = false;
	_uint				m_iHitCount = 0;
	_double				m_lfHitResetTimeAcc = 0.0;

private:	/* For.Slider */
	_bool				m_isSliding = false;
	_bool				m_isSlidingCheck = false;
	_bool				m_isSlidingTriggerCheck = false;
private:	/* For.LedgeCheck */
	_bool				m_bLedgeCheck = false;

private:	/* For.Attack */
	_double				m_lfAttackMoveSpeed = 0.0;
	_double				m_lfDashAttackMoveSpeed = 0.0;

private:	/* For.Colider */
	_bool				m_IsColiderOn = false;

private:	/* For.RimLight*/
	_double				m_lfRimLightTimeAcc = 0.0;
	_bool				m_IsRimLightEnable = false;
public:
	static CPlayer_AnimState*	Create(CPlayer* pPlayer, CTransform* pPlayerTrasnform, CModel* pPlayerModel, CSphereCollider* pPlayerColider, CNavigation* pPlayerNavigation, CCharacterController* pCharacterController, CPlayer_LeftHand* pLeftHand, CPlayer_RightHand* pRightHand, CPlayerTrigger_Parkour* pTriggerParkour, _uint iMyLevel);
	virtual void Free() override;

};

END

