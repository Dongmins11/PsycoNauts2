#include "stdafx.h"
#include "..\public\Right_Hand_Anim.h"
#include "GameInstance.h"
#include "Right_Parent_Anim.h"
#include "Right_Hand_Default.h"
#include "Right_Hand_ChopB.h"
#include "Right_Hand_Grap.h"
#include "PullingGrab.h"
#include "Right_Hand_Throw_Idle.h"
#include "Right_Hand_Throw_Right.h"
#include "Right_Hand_Throw_Finished.h"

CRight_Hand_Anim::CRight_Hand_Anim()
{
}

HRESULT CRight_Hand_Anim::NativeConstruct(CPlayer_RightHand* pRightHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pRenderColider, CSphereCollider* pSphereColider, _uint iMyScene)
{
	if (nullptr == pRightHand || nullptr == pTransformCom || nullptr == pModelCom)
		return E_FAIL;

	m_pRightHand = pRightHand;

	m_pTransformCom = pTransformCom;
	m_pModelCom = pModelCom;
	m_pRenderColidercom = pRenderColider;
	m_pColliderCom = pSphereColider;

	m_iCurScene = iMyScene;

	CRight_Hand_Default::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);
	CRight_Hand_ChopB::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);
	CRight_Hand_Grap::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);
	CPullingGrab::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);
	CRight_Hand_Throw_Finished::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);
	CRight_Hand_Throw_Idle::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);
	CRight_Hand_Throw_Right::Get_Instance()->SetUp_StateInfo(pRightHand, m_pTransformCom, m_pModelCom, m_pRenderColidercom, m_pColliderCom, m_iCurScene);

	m_pRightHandState = CRight_Hand_Default::Get_Instance();

	return S_OK;
}

void CRight_Hand_Anim::Set_LerpTrue()
{
	m_bAnimLerp = true;
}

void CRight_Hand_Anim::SetUp_State(CRight_Parent_Anim * pAnimState)
{
	if (nullptr == pAnimState)
	{
		MSGBOX("Failed to Change SetupState : CRight_Hand_ParentAnim : Funtion");
		return;
	}
	m_pRightHandState = pAnimState;
}

void CRight_Hand_Anim::Update(_double TimeDelta)
{
	m_pRightHandState->HandleInput(this, m_tRightHandDesc, TimeDelta);

	LerpCheck();

	m_pRightHandState->Update(this, m_tRightHandDesc, TimeDelta);
}

void CRight_Hand_Anim::LerpCheck()
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

CRight_Hand_Anim * CRight_Hand_Anim::Create(CPlayer_RightHand* pRightHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pRenderColider, CSphereCollider* pSphereColider, _uint iMyScene)
{
	CRight_Hand_Anim* pInstance = new CRight_Hand_Anim;

	if (FAILED(pInstance->NativeConstruct(pRightHand, pTransformCom, pModelCom, pRenderColider, pSphereColider, iMyScene)))
	{
		MSGBOX("Failed to Create : CRight_Hand_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRight_Hand_Anim::Free()
{
	CRight_Hand_Default::Destroy_Instance();
	CRight_Hand_ChopB::Destroy_Instance();
	CRight_Hand_Grap::Destroy_Instance();
	CPullingGrab::Destroy_Instance();
	CRight_Hand_Throw_Finished::Destroy_Instance();
	CRight_Hand_Throw_Idle::Destroy_Instance();
	CRight_Hand_Throw_Right::Destroy_Instance();
}
