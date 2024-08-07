#pragma once

#include "Base.h"
#include "Right_Hand_Anim.h"
#include "Player_RightHand.h"
#include "CollisionObject.h"
#include "GameInstance.h"


BEGIN(Engine)
class CTransform;
class CModel;
class CCollider;
END


BEGIN(Client)
class CPlayer_RightHand;
class CRight_Parent_Anim abstract : public CBase
{
protected:
	CRight_Parent_Anim();
	virtual ~CRight_Parent_Anim() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim* pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC& pRightHandDesc, _double TimeDelta)PURE;
	virtual void Update(CRight_Hand_Anim* pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC& pRightHandDesc, _double TimeDelta)PURE;
	virtual void Reset_Anim(CRight_Hand_Anim* pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC& pRightHandDesc, _double TimeDelta)PURE;
	virtual void Set_Colider(CRight_Hand_Anim* pRightHandAnim)PURE;
public:
	void	SetUp_StateInfo(CPlayer_RightHand* pRightHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pRenderColider, CSphereCollider* pSphereColider, _uint iMyScene);

protected:
	virtual void SetUp_BoneTargetMatrix(const char* pBoneName);
	virtual void Bone_MatrixCalculation(_double TimeDelta);

protected:
	CPlayer_RightHand*	m_pRightHand = nullptr;

protected:
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pRenderColiderCom = nullptr;
	CSphereCollider*	m_pColliderCom = nullptr;
protected:
	_uint				m_iCurScene = 0;

protected:
	_bool				m_bFirstCheck = false;
	_uint				m_iAnimIndex = 0;
	_double				m_lfTimeAcc = 0.0;

protected:	/* For.Grab_Bone_Matrix*/
	const _float4x4*		m_pMyWorldMatrix;
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;
	const _float4x4*		m_pTargetWorldMatrix;

protected: /* For.*/
	_float4					m_vDistance;


public:
	virtual void Free() override;
};

END