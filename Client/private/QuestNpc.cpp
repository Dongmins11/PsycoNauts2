#include "stdafx.h"
#include "..\public\QuestNpc.h"
#include "GameInstance.h"


CQuestNpc::CQuestNpc(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CQuestNpc::CQuestNpc(const CQuestNpc & rhs)
	:CGameObject(rhs)
{
}

HRESULT CQuestNpc::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestNpc::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;


	//OBJINFO	objinfo;
	//if (nullptr != pArg)
	//	objinfo = *(OBJINFO*)pArg;
	//
	//m_iRenderPass = objinfo.iRenderPass;
	//
	//m_StrLayerName = objinfo.StrLayerName;
	//m_StrModelName = objinfo.StrModelName;
	//m_StrObjectFullName = objinfo.StrObjectFullName;
	////m_iPX_TYPE = objinfo.iPX_Index;
	//m_eLayerType = (LayerType)objinfo.iPX_Index;
	//
	//_float3 vColSize = objinfo.fCollider_Extent;
	//_float3 vColPos = objinfo.fCollider_Center;
	//m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	//if (m_fRangeForFrustum <= 0.f)
	//	m_fRangeForFrustum = 1.f;
	//_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);
	//
	//
	//if (FAILED(SetUp_Component(objinfo.fCollider_Extent, objinfo.fCollider_Center, vObjScale)))
	//	return E_FAIL;
	//
	//m_pModelCom->Set_Animation(0);
	//_float4x4 fWorldMatrix;
	//XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	//m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	//
	//m_iRenderPass = 0;
	//m_iRenderPassBackUp = m_iRenderPass;
	//
	//m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_NPC);
	return S_OK;
}

_int CQuestNpc::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	//{
	//	if (!m_isRender && m_pColliderCom)
	//	{
	//		m_pColliderCom->Add_Actor();
	//	}
	//	m_isRender = true;
	//}
	//else
	//{
	//	if (m_isRender && m_pColliderCom)
	//	{
	//		m_pColliderCom->Remove_Actor();
	//	}
	//	m_isRender = false;
	//}
	//
	//
	//RELEASE_INSTANCE(CGameInstance);
	//
	//if (!m_isRender)
	//	return 0;
	//
	//m_pModelCom->Update(TimeDelta);
	//
	//if (m_pColliderCom)
	//{
	//	m_pColliderCom->Update_TransformPhysX();
	//}
	return 0;
}

_int CQuestNpc::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	if (!m_isRender)
		return 0;

	return 0;
}

HRESULT CQuestNpc::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CQuestNpc::OnTriggerEnter(CCollisionObject & collision)
{
}

void CQuestNpc::OnTriggerExit(CCollisionObject & collision)
{
}

HRESULT CQuestNpc::SetUp_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	return __super::SetUp_Component(iSceneIndex, pPrototypeTag, pComponentTag, ppOut, pArg);
}

HRESULT CQuestNpc::SetUp_ConstantTable()
{
	return S_OK;
}

CQuestNpc * CQuestNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CQuestNpc*	pInstance = new CQuestNpc(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CQuestNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CQuestNpc::Clone(void * pArg, _uint iCurrentScene)
{
	CQuestNpc*	pInstance = new CQuestNpc(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CQuestNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuestNpc::Free()
{
	__super::Free();

	//if (m_pColliderCom)
	//	m_pColliderCom->Remove_Actor();
	//
	//Safe_Release(m_pColliderCom);
	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pRendererCom);
	//Safe_Release(m_pModelCom);
}
