#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Player_LeftHand.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CCollider;
END


BEGIN(Client)
class CPlayer_LeftHand;
class CLeft_Hand_ParentAnim;
class CLeft_Hand_Anim final : public CBase
{
public:
	enum LEFT_HAND_STATE { LEFT_HAND_STATE_CHOPA, LEFT_HAND_STATE_CHOPC, LEFT_HAND_STATE_DISH_IDLE,
							LEFT_HAND_STATE_DISH_THROW, LEFT_HAND_STATE_DISH_FINISHED, LEFT_HAND_STATE_DISHEND, LEFT_HAND_STATE_END };
public:
	typedef struct tagLeftHandAnimDesc
	{
		_uint eType;
	}LEFTHANDDESC;
private:
	explicit CLeft_Hand_Anim();
	virtual ~CLeft_Hand_Anim() = default;
public:
	HRESULT NativeConstruct(CPlayer_LeftHand* pLeftHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pColider, _uint iMyScene);
	
public:	/* For. Set */
	void	Set_HandState(_uint iHandState) { m_iLeftHandState = iHandState; }
	void	Set_RenderCheck(_bool bRenderCheck) { m_bRenderCheck = bRenderCheck; }
	void	Set_ChangeCheck(_bool bChangeCheck) { m_bChangeCheck = bChangeCheck; }
	void	Set_LerpTrue();
	void	Set_TimeAcc(_double TimeAcc) { m_TimeAcc = TimeAcc; }
	void	Set_Dish_Grab(_bool bGrabCheck) { m_bDishGrab = bGrabCheck; }
	void	Set_Dish_Throw(_bool bDishThrow) { m_bDishThrow = bDishThrow; }
	void	Set_ThrowBoneCaculationCheck(_bool bThrowCaculationCheck) { m_bBoneThrowCaculationCheck = bThrowCaculationCheck; }
public: /* For. Get*/
	_uint	Get_Left_HandState() const { return m_iLeftHandState; }
	_bool	Get_RenderCheck() const { return m_bRenderCheck; }
	_bool	Get_ChangeCheck() const { return m_bChangeCheck; }
	_bool	Get_Dish_Grab() const { return m_bDishGrab; }
	_bool	Get_Dish_Throw() const { return m_bDishThrow; }
	_bool	Get_ThrowBoneCaculationCheck() const { return m_bBoneThrowCaculationCheck; }
	_double	Get_TimeAcc() const { return m_TimeAcc; }
public:
	void	SetUp_State(CLeft_Hand_ParentAnim* pAnimState);
	void	Update(_double TimeDelta);
private:	/*For LerpCheck */
	void	LerpCheck();
private: /* For. Com */
	CTransform* m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	CCollider*	m_pAABBcom = nullptr;

private:
	CPlayer_LeftHand*		m_pLeftHand = nullptr;
	CLeft_Hand_ParentAnim*	m_pLeftHandState = nullptr;

private:
	LEFTHANDDESC		m_tLeftHandDesc;
	_uint				m_iLeftHandState = LEFT_HAND_STATE_END;

private:	/* For.AnimLerpCheck */
	_bool				m_bAnimLerp = false;

private:	/* For.RenderCheck */
	_bool	m_bRenderCheck = false;
private:
	_bool	m_bChangeCheck = false;
private:
	_uint	m_iCurScene = 0;	
private:	/* ¿Ã∆Â∆Æ */
	_double	m_TimeAcc = 1.0;

private:	/* For. DishGrab */
	_bool	m_bDishGrab = false;
	_bool	m_bDishThrow = false;
private:	/* For.BoneMatrixCaculation */
	_bool	m_bBoneThrowCaculationCheck = false;
public:
	static CLeft_Hand_Anim* Create(CPlayer_LeftHand* pLeftHand,CTransform* pTransformCom,CModel* pModelCom,CCollider* pColider,_uint iMyScene);
	virtual void Free() override;
};

END