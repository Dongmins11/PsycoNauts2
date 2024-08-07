#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Player_LeftHand.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CCollider;
class CSphereCollider;
END


BEGIN(Client)
class CPlayer_RightHand;
class CRight_Parent_Anim;
class CRight_Hand_Anim final : public CBase
{
public:
	enum RIGHT_HAND_STATE { RIGHT_HAND_STATE_CHOPB, RIGHT_HAND_STATE_GRAB, RIGHT_HAND_STATE_GRAB_ATTACK, RIGHT_HAND_STATE_GRAB_PULLING,
							RIGHT_HAND_STATE_DISH_IDLE,RIGHT_HAND_STATE_DISH_THROW, RIGHT_HAND_STATE_DISH_FINISIHED, RIGHT_HAND_STATE_DISH_END,
							RIGHT_HAND_STATE_END };
public:
	typedef struct tagRightHandAnimDesc
	{
		_uint eType;
	}RIGHTHANDDESC;
private:
	explicit CRight_Hand_Anim();
	virtual ~CRight_Hand_Anim() = default;
public:
	HRESULT NativeConstruct(CPlayer_RightHand* pRightHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pRenderColider, CSphereCollider* pSphereColider, _uint iMyScene);
	
public:	/* For. Set */
	void	Set_HandState(_uint iHandState) { m_iRightHandState = iHandState; }
	void	Set_RenderCheck(_bool bRenderCheck) { m_bRenderCheck = bRenderCheck; }
	void	Set_ChangeCheck(_bool bChangeCheck) { m_bChangeCheck = bChangeCheck; }
	void	Set_BoneCaculationCheck(_bool bCaculationCheck) { m_bBoneCaculationCheck = bCaculationCheck; }
	void	Set_ThrowBoneCaculationCheck(_bool bThrowCaculationCheck) { m_bBoneThrowCaculationCheck = bThrowCaculationCheck; }
	void	Set_LerpTrue();
	void	Set_TimeAcc(_double TimeAcc) { m_TimeAcc = TimeAcc; }
	void	Set_PullingSuccessCheck(_bool bPullingSuccessCheck) { m_bPullingCheck = bPullingSuccessCheck; }
	void	Set_Dish_Grab(_bool bGrabCheck) { m_bDishGrab = bGrabCheck; }
	void	Set_Dish_Throw(_bool bDishThrow) { m_bDishThrow = bDishThrow; }
public: /* For. Get*/
	_uint	Get_Right_HandState() const { return m_iRightHandState; }
	_bool	Get_RenderCheck() const { return m_bRenderCheck; }
	_bool	Get_ChangeCheck() const { return m_bChangeCheck; }
	_bool	Get_BoneCaculationCheck() const { return m_bBoneCaculationCheck; }
	_bool	Get_ThrowBoneCaculationCheck() const { return m_bBoneThrowCaculationCheck; }
	_bool	Get_PullingSuccessCheck() const { return m_bPullingCheck; }
	_bool	Get_Dish_Grab() const { return m_bDishGrab; }
	_bool	Get_Dish_Throw() const { return m_bDishThrow; }
	_double	Get_TimeAcc() const { return m_TimeAcc; }
public:
	void	SetUp_State(CRight_Parent_Anim* pAnimState);
	void	Update(_double TimeDelta);
private:	/*For LerpCheck */
	void	LerpCheck();
private: /* For. Com */
	CTransform* m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	CCollider*	m_pRenderColidercom = nullptr;
	CSphereCollider* m_pColliderCom = nullptr;
private:
	CPlayer_RightHand*		m_pRightHand = nullptr;
	CRight_Parent_Anim*		m_pRightHandState = nullptr;

private:
	RIGHTHANDDESC		m_tRightHandDesc;
	_uint				m_iRightHandState = RIGHT_HAND_STATE_END;

private:	/* For.AnimLerpCheck */
	_bool				m_bAnimLerp = false;

private:	/* For.RenderCheck */
	_bool	m_bRenderCheck = false;
private:	/* For.StateChange*/
	_bool	m_bChangeCheck = false;
private:	/* For. BoneCaculation*/
	_bool	m_bBoneCaculationCheck = false;
	_bool	m_bBoneThrowCaculationCheck = false;
private:	/* For. PullingCheck */
	_bool	m_bPullingCheck = false;
private:	/* For. DishGrab */
	_bool	m_bDishGrab = false;
	_bool	m_bDishThrow = false;

private:
	_uint	m_iCurScene = 0;
private:	/* ¿Ã∆Â∆Æ */
	_double	m_TimeAcc = 1.0;
public:
	static CRight_Hand_Anim* Create(CPlayer_RightHand* pRightHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pRenderColider, CSphereCollider* pSphereColider, _uint iMyScene);
	virtual void Free() override;
};

END