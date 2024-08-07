#include "stdafx.h"
#include "..\public\Left_Hand_Anim.h"
#include "GameInstance.h"
#include "Left_Hand_ParentAnim.h"
#include "Left_Hand_Default.h"
#include "Left_Hand_ChopA.h"
#include "Left_Hand_ChopC.h"
#include "Left_Hand_Throw_Idle.h"
#include "Left_Hand_Throw_Left.h"
#include "Left_Hand_Throw_Finished.h"


CLeft_Hand_Anim::CLeft_Hand_Anim()
{
}

HRESULT CLeft_Hand_Anim::NativeConstruct(CPlayer_LeftHand * pLeftHand, CTransform * pTransformCom, CModel * pModelCom, CCollider * pColider, _uint iMyScene)
{
	if (nullptr == pLeftHand || nullptr == pTransformCom || nullptr == pModelCom)
		return E_FAIL;

	m_pLeftHand = pLeftHand;

	m_pTransformCom = pTransformCom;
	m_pModelCom = pModelCom;
	m_pAABBcom = pColider;

	m_iCurScene = iMyScene;

	CLeft_Hand_Default::Get_Instance()->SetUp_StateInfo(m_pLeftHand, m_pTransformCom, m_pModelCom, m_pAABBcom, m_iCurScene);
	CLeft_Hand_ChopA::Get_Instance()->SetUp_StateInfo(m_pLeftHand, m_pTransformCom, m_pModelCom, m_pAABBcom, m_iCurScene);
	CLeft_Hand_ChopC::Get_Instance()->SetUp_StateInfo(m_pLeftHand, m_pTransformCom, m_pModelCom, m_pAABBcom, m_iCurScene);
	CLeft_Hand_Throw_Idle::Get_Instance()->SetUp_StateInfo(m_pLeftHand, m_pTransformCom, m_pModelCom, m_pAABBcom, m_iCurScene);
	CLeft_Hand_Throw_Left::Get_Instance()->SetUp_StateInfo(m_pLeftHand, m_pTransformCom, m_pModelCom, m_pAABBcom, m_iCurScene);
	CLeft_Hand_Throw_Finished::Get_Instance()->SetUp_StateInfo(m_pLeftHand, m_pTransformCom, m_pModelCom, m_pAABBcom, m_iCurScene);

	m_pLeftHandState = CLeft_Hand_Default::Get_Instance();

	return S_OK;
}

void CLeft_Hand_Anim::Set_LerpTrue()
{
	m_bAnimLerp = true;
}

void CLeft_Hand_Anim::SetUp_State(CLeft_Hand_ParentAnim * pAnimState)
{
	if (nullptr == pAnimState)
	{
		MSGBOX("Failed to Change SetupState : CLeft_Hand_Anim : Funtion");
		return;
	}
	m_pLeftHandState = pAnimState;
}

void CLeft_Hand_Anim::Update(_double TimeDelta)
{
	m_pLeftHandState->HandleInput(this, m_tLeftHandDesc, TimeDelta);

	LerpCheck();

	m_pLeftHandState->Update(this, m_tLeftHandDesc, TimeDelta);
}

void CLeft_Hand_Anim::LerpCheck()
{
	_bool bLerpCheck = false;

	if (true == m_bAnimLerp)
	{
		bLerpCheck = true;
		m_bAnimLerp = false;
	}

	if (true == m_bAnimLerp)
		m_pModelCom->Set_IsUpperLefp(bLerpCheck);
}

CLeft_Hand_Anim * CLeft_Hand_Anim::Create(CPlayer_LeftHand * pLeftHand, CTransform * pTransformCom, CModel * pModelCom, CCollider * pColider, _uint iMyScene)
{
	CLeft_Hand_Anim* pInstance = new CLeft_Hand_Anim;

	if (FAILED(pInstance->NativeConstruct(pLeftHand, pTransformCom, pModelCom, pColider, iMyScene)))
	{
		MSGBOX("Failed to Create : CLeft_Hand_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeft_Hand_Anim::Free()
{
	CLeft_Hand_Default::Destroy_Instance();
	CLeft_Hand_ChopA::Destroy_Instance();
	CLeft_Hand_ChopC::Destroy_Instance();
	CLeft_Hand_Throw_Idle::Destroy_Instance();
	CLeft_Hand_Throw_Left::Destroy_Instance();
	CLeft_Hand_Throw_Finished::Destroy_Instance();
}
