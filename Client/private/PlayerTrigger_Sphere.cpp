#include "stdafx.h"
#include "..\public\PlayerTrigger_Sphere.h"
#include "GameInstance.h"
#include "CollisionObject.h"

CPlayerTrigger_Sphere::CPlayerTrigger_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CPlayerTrigger(pDevice, pDeviceContext)
{
}

CPlayerTrigger_Sphere::CPlayerTrigger_Sphere(const CPlayerTrigger_Sphere & rhs)
	: CPlayerTrigger(rhs)
{
}

HRESULT CPlayerTrigger_Sphere::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerTrigger_Sphere::NativeConstruct(void * pArg)
{
	
	m_eLayerType = LayerType::LAYERTYPE_PLAYER_TRIGGER;

	_float3 vColSize = { 5.f, 5.f, 5.f };
	_float3 vColPos = { 0.f,0.f,0.f };//objinfo.fCollider_Center;
	
	_float3 vObjScale = { 1.f,1.f,1.f };//GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(vColSize, vColPos, vObjScale)))
		return E_FAIL;


	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z));

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	return S_OK;
}

_int CPlayerTrigger_Sphere::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	return _int();
}

_int CPlayerTrigger_Sphere::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_pColliderCom)
		m_pColliderCom->Update_Transform();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CPlayerTrigger_Sphere::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerTrigger_Sphere::OnTriggerEnter(CCollisionObject & collision)
{
	//OutputDebugStringW(TEXT("OnTriggerEnter - PlayerTrigger_Sphere\n"));
	switch (collision.Get_Object()->Get_ObjectLayerType())
	{
	case LayerType::LAYERTYPE_STATIC:
	case LayerType::LAYERTYPE_PICK:
	{
		collision.Get_Object()->Set_ActorActive(true);
		break;
	}
	default:
		break;
	}
}
//
//void CPlayerTrigger_Sphere::OnTriggerStay(CCollisionObject & collision)
//{
//	//switch (collision.Get_Object()->Get_ObjectLayerType())
//	//{
//	//case LayerType::LAYERTYPE_STATIC:
//	//case LayerType::LAYERTYPE_PICK:
//	//{
//	//	if(!collision.Get_Object()->Get_IsRender())
//	//		collision.Get_Object()->Set_IsRender(true);
//	//	break;
//	//}
//	//default:
//	//	break;
//	//}
//}
//
void CPlayerTrigger_Sphere::OnTriggerExit(CCollisionObject & collision)
{
	switch (collision.Get_Object()->Get_ObjectLayerType())
	{
	case LayerType::LAYERTYPE_STATIC:
	case LayerType::LAYERTYPE_PICK:
	{
		collision.Get_Object()->Set_ActorActive(false);
		break;
	}
	default:
		break;
	}
}

void CPlayerTrigger_Sphere::OnControllerCollisionHit(CCollisionObject & collision)
{
}


void CPlayerTrigger_Sphere::Set_Position(_fvector vPos)
{
	if (!m_pTransformCom)
		return;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();
}

HRESULT CPlayerTrigger_Sphere::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	m_vColPos = vColPos;
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom,nullptr)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		tColliderDesc.isVisualization = true;
		tColliderDesc.isTrigger = true;
		tColliderDesc.pGameObject = this;
		CSphereCollider::DESC tSphereColliderDesc;
		tSphereColliderDesc.tColliderDesc = tColliderDesc;
		tSphereColliderDesc.vScale = vColSize;

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tSphereColliderDesc)))
			return E_FAIL;

		m_pColliderCom->Set_ShapeLayer(m_eLayerType);

		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}
	return S_OK;
}

CPlayerTrigger_Sphere * CPlayerTrigger_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerTrigger_Sphere*	pInstance = new CPlayerTrigger_Sphere(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayerTrigger_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerTrigger_Sphere::Clone(void * pArg, _uint iCurrentScene = 0)
{
	CPlayerTrigger_Sphere*	pInstance = new CPlayerTrigger_Sphere(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayerTrigger_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerTrigger_Sphere::Free()
{
	__super::Free();
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
