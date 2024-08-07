#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Player_AnimState.h"
#include "Player.h"
#include "GameInstance.h"
#include "Player_LeftHand.h"
#include "Player_RightHand.h"
#include "Left_Hand_Anim.h"
#include "Right_Hand_Anim.h"
#include "PlayerTrigger_Parkour.h"


BEGIN(Engine)
class CTrasnform;
class CModel;
class CCollider;
class CNavigation;
class CSphereCollider;
END

BEGIN(Client)
class CPlayer_LeftHand;
class CPlayer_RightHand;
class CPlayer_ParentAnim abstract : public CBase
{
public:
	enum LOOK_STATE {
		LOOK_FRONT, LOOK_BACK,
		LOOK_RIGHT, LOOK_RIGHT_UP, LOOK_RIGHT_DOWN,
		LOOK_LEFT, LOOK_LEFT_UP, LOOK_LEFT_DOWN,
		LOOK_END
	};
protected:
	explicit CPlayer_ParentAnim();
	virtual ~CPlayer_ParentAnim() = default;
public:
	void Set_StateInfo(CPlayer* pPlayer, CTransform* pPlayerTransform, CModel* pPlayerModel, CSphereCollider* pPlayerCollider, CNavigation* pPlayerNavigation, CCharacterController* pCharacterController, CPlayer_LeftHand* pLeftHand, CPlayer_RightHand* pRightHand, CPlayerTrigger_Parkour* pTriggerParkour, _uint iScene);
public:
	virtual void HandleInput(CPlayer_AnimState* pPlayer_AnimState, CPlayer_AnimState::ANIMDESC& pAnimKeyDesc, _double TimeDelta)PURE;
	virtual void Update(CPlayer_AnimState* pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC& pAnimKeyDesc, _double TimeDelta)PURE;
	virtual void Reset_Anim(CPlayer_AnimState* pPlayer_AnimState)PURE;
	virtual void Dead_Reset(CPlayer_AnimState* pPlayer_AnimState)PURE;

protected: /* For.Move Direction Check */
	virtual void Handle_Movement(const CPlayer_AnimState::ANIMDESC& pAnimKeyDesc);
	virtual void Move_Acceleration(_double TimeDelta);
protected: /* For. Player Look_Lerp */
	void SetUp_Look(_double TimeDelta);
	void SetUp_SlideLook(_double TimeDelta);
	void SetUp_Cam_to_LookAt(_double TimeDelta);
	void Reverse_Look(_double TimeDelta);

	_vector Lerp_Roation(_double TimeDelta);
	_vector Lerp_Slide_Rotation(_double TimeDelta);
	_vector Compute_DirVector();
	_vector Compute_Slide_DirVector();

protected: /* For. Player_Move */
	virtual void Player_Move(_double TimeDelta);
	virtual void Player_Move_NonMoveOffset(_double TimeDelta);
	virtual void Player_Look_Reverse_Move(_double TimeDelta);


protected:	/* For. Navigation */
	virtual HRESULT SetUp_HeightOnNavigation();

public:		/* For. Player_Accelecrtaion */
	void	Set_PlayerMoveAccelecrtaion(_float fMoveValue) { m_fMoveOffset = fMoveValue; }
	void	Set_PlayerMoveValue(_float fMoveValue) { m_lfMoveSpeed = fMoveValue; }
	void	Set_MoveOffset_ChangeCheck(_bool bMoveOffsetChangeCheck) { m_bMoveOffsetCheck = bMoveOffsetChangeCheck; }
public:		/* For.Player_LookState */
	void	Set_LookState(LOOK_STATE eLookState) { m_eCurrentLookState = eLookState; }

public:		/* For.GravityCheck*/
	_bool	Get_GravityCheck() const { return m_bGravityCheck; }

public:
	_bool	Get_AttackCheck() const { return m_bAttackCheck; }


protected:	/* For.Component OR GameObject */
	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pTransform = nullptr;
	CModel*			m_pModel = nullptr;
	CNavigation*	m_pNavigation = nullptr;
	CCollider*		m_pCollider = nullptr;
	CSphereCollider*		m_pColliderCom = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;
	CPlayerTrigger_Parkour*	m_pTrigger_Parkour = nullptr;
	_uint			m_iScene = 0;

protected:	// RayCast TEST
	CGameObject*		m_pHitObject = nullptr;


protected: /* For.Hand*/
	CPlayer_LeftHand*	m_pLeftHand = nullptr;
	CPlayer_RightHand*	m_pRightHand = nullptr;

protected: /* For.LookState */
	LOOK_STATE			m_eCurrentLookState = LOOK_END;
	LOOK_STATE			m_ePreLookState = LOOK_END;

protected: /* For. RotationLerp */
	_float3				m_vSourRotation;
	_float3				m_vDestRotation;
	_float3				m_vDestBackUp;
	_float				m_fRatio = 0.f;
	_float				m_fTime = 0.f;
	_double				m_lfLookLerpSpeed = 0.0;
protected: /* For. MoveValue */
	_double				m_lfMoveSpeed = 0.0;

protected:	/*For.AnimIndex*/
	_uint				m_iAnimIndex = 0;
	_uint				m_iUpperIndex = 0;
	_uint				m_iRootIndex = 0;
	_uint				m_iBottomIndex = 0;

protected:	/* For.AnimSpeed */
	_double				m_lfAnimSpeed = 0;
	_double				m_lfUpperAnimSpeed = 0;
	_double				m_lfRootAnimSpeed = 0;
	_double				m_lfBottomAnimSpeed = 0;

protected:	/* For.RotationLerpCheck */
	_bool				m_bIsLefp = false;
	_bool				m_bIsLerpCheck = false;

protected: /* For.Move_Acceleator*/
	_bool				m_bIsMoveOn = false;
protected:	/*For.Check_Collection*/
	_bool				m_bMoveCheck = false;
	_bool				m_bReverse = false;
protected:	/*For. TimeDelta */
	_double				m_lfTimeAcc = 0.0;
	_double				m_lfUpperTimeAcc = 0.0;
	_double				m_lfRootTimeAcc = 0.0;
	_double				m_lfBottomTimeAcc = 0.0;
protected:		/*For.MoveOffset */
	_float				m_fMoveOffset = 0.f;
	_bool				m_bMoveOffsetCheck = false;

protected:
	_bool				m_bGravityCheck = false;
	_bool				m_bFirstRotationCheck = false;
	_bool				m_bAttackCheck = false;

public:
	virtual void Free() override;
};
END
