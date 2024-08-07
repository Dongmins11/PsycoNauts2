#pragma once

#include "Base.h"
#include "Left_Hand_Anim.h"
#include "Player_LeftHand.h"
#include "Player.h"
#include "GameInstance.h"


BEGIN(Engine)
class CTransform;
class CModel;
class CCollider;
END


BEGIN(Client)
class CPlayer_LeftHand;
class CLeft_Hand_ParentAnim abstract : public CBase
{
protected:
	CLeft_Hand_ParentAnim();
	virtual ~CLeft_Hand_ParentAnim() = default;
public:
	virtual void HandleInput(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC& pLeftHandDesc, _double TimeDelta)PURE;
	virtual void Update(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC& pLeftHandDesc, _double TimeDelta)PURE;
	virtual void Reset_Anim(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC& pLeftHandDesc, _double TimeDelta)PURE;
	virtual void Set_Colider(CLeft_Hand_Anim* pLeftHandAnim)PURE;
public:
	void	SetUp_StateInfo(CPlayer_LeftHand* pLeftHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pColider, _uint iMyScene);

protected:
	CPlayer_LeftHand*	m_pLeftHand = nullptr;

protected:
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pAABBCom = nullptr;

protected:
	_uint				m_iCurScene = 0;

protected:
	_bool				m_bFirstCheck = false;
	_uint				m_iAnimIndex = 0;
	_double				m_lfTimeAcc = 0.0;
public:
	virtual void Free() override;
};

END