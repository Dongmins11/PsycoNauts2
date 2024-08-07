#include "stdafx.h"
#include "..\public\EnviromentTrigger.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Fruit.h"

CEnviromentTrigger::CEnviromentTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CEnviromentTrigger::CEnviromentTrigger(const CEnviromentTrigger & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEnviromentTrigger::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}


HRESULT CEnviromentTrigger::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;


	_matrix WorldMatrix;

	if (nullptr != pArg)
		WorldMatrix = *(_matrix*)pArg;

	/* For.WorldMatrix */
	XMStoreFloat4x4(&m_WorldMatrix,WorldMatrix);

	_float4 TriggerPosition;
	XMStoreFloat4(&TriggerPosition, WorldMatrix.r[3]);

	WorldMatrix.r[3] = XMVectorSet(TriggerPosition.x, TriggerPosition.y, TriggerPosition.z, 1.f); /*  Postion */
	m_eTriggerType = (EnviromentTriggerType)((_uint)TriggerPosition.w); // COOK : 10 10

	_float4 TriggerColliderCheck;
	XMStoreFloat4(&TriggerColliderCheck, WorldMatrix.r[0]);

	WorldMatrix.r[0] = XMVectorSet(TriggerColliderCheck.x, TriggerColliderCheck.y, TriggerColliderCheck.z, 0.f); 
	_float4 TriggerColliderRange;
	XMStoreFloat4(&TriggerColliderRange, WorldMatrix.r[1]);
	WorldMatrix.r[1] = XMVectorSet(TriggerColliderRange.x, TriggerColliderRange.y, TriggerColliderRange.z, 0.f); 

	if ((_bool)TriggerColliderCheck.w)
	{
		/* Sphere À¯¹«*/

		//SphereRange = TriggerColliderRange.w; 
	}
	_float4 TriggerGroupIndex;
	XMStoreFloat4(&TriggerGroupIndex, WorldMatrix.r[2]);
	WorldMatrix.r[2] = XMVectorSet(TriggerGroupIndex.x, TriggerGroupIndex.y, TriggerGroupIndex.z, 0.f);
	m_iCnt = (_uint)TriggerGroupIndex.w;


	m_eLayerType = LayerType::LAYERTYPE_TRIGGER;
//	m_eTriggerType = EnviromentTriggerType::ETTYPE_PARKOUR;
	_float3 _vScale = GET_SCALEFROMMAT(WorldMatrix);
	//_float3 vColSize = { 1.f, 0.25f, 0.25f };
	_float3 vColPos = { 0.f,0.f,0.f };//STOREFLOAT3(WorldMatrix.r[3]);//objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(_vScale) *2.f;
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = 2.f;
	_float3 vObjScale = { 1.f,1.f,1.f };//GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(_vScale, vColPos, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	_matrix matWorld = XMMatrixTranslation(TriggerPosition.x, TriggerPosition.y, TriggerPosition.z);
	for(_uint i=0;i<3;++i)
		matWorld.r[i] = XMVector3Normalize(WorldMatrix.r[i]);
	XMStoreFloat4x4(&fWorldMatrix, matWorld);
	//XMLoadFloat4x4(&WorldMatrix)
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;


#ifdef _DEBUG
	CCollider::COLLIDERDESC		ColliderDesc;

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vExtents = _float3(_vScale.x/2.f, _vScale.y / 2.f, _vScale.z / 2.f);//MULTI_FLOAT3(vColSize, 0.5f);//GET_SCALEFROMMAT(m_pTransformCom->Get_WorldMatrix()); // _float3(2.5f, 0.25f, 0.25f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pRenderCollider, &ColliderDesc)))
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}

_int CEnviromentTrigger::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}
	return 0;
}

_int CEnviromentTrigger::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;
	if (!m_isRender)
		return 0;


#ifdef _DEBUG
	m_pRenderCollider->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return 0;
}

HRESULT CEnviromentTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

#ifdef _DEBUG
	if (0 == (_uint)m_eTriggerType) /* Parkour  */
		m_pRenderCollider->Render(COLOR_RED);
	if (1 == (_uint)m_eTriggerType) /* LOBOTO   */
		m_pRenderCollider->Render(COLOR_BULE);
	if (2 == (_uint)m_eTriggerType) /* Close  */
		m_pRenderCollider->Render(COLOR_PURPLE);
	if (3 == (_uint)m_eTriggerType)/* Curling */
		m_pRenderCollider->Render(COLOR_ORANGE);
	if (4 == (_uint)m_eTriggerType)/* TutoMsg */
		m_pRenderCollider->Render(COLOR_YELLOW);
	if (5 == (_uint)m_eTriggerType)
		m_pRenderCollider->Render(COLOR_MINT);
	if (6 == (_uint)m_eTriggerType)
		m_pRenderCollider->Render(COLOR_DeepPink);
	if (8 == (_uint)m_eTriggerType)
		m_pRenderCollider->Render(COLOR_BurlyWood);

#endif // _DEBUG

	
	return S_OK;
}


void CEnviromentTrigger::OnTriggerEnter(CCollisionObject & collision)
{
	
}

void CEnviromentTrigger::OnTriggerStay(CCollisionObject & collision)
{
}

void CEnviromentTrigger::OnTriggerExit(CCollisionObject & collision)
{
}

void CEnviromentTrigger::OnControllerCollisionHit(CCollisionObject & collision)
{
	OutputDebugString(TEXT("Player Trigger Enter\n"));
}



HRESULT CEnviromentTrigger::SetUp_Component(_float3 vColSize, _float3 vColPos,_float3 vObjScale)
{
	m_vColPos = vColPos;
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
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

		tColliderDesc.isSceneQuery = true;


		CBoxCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = vColSize; //MULTI_FLOAT3(vColSize, 2.0);

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
		m_pColliderCom->SetPivotMatrix(smatPviot);
		m_pColliderCom->Set_ShapeLayer(m_eLayerType);


		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

	return S_OK;
}

HRESULT CEnviromentTrigger::SetUp_ConstantTable()
{

	return S_OK;
}


CEnviromentTrigger * CEnviromentTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEnviromentTrigger*	pInstance = new CEnviromentTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CEnviromentTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnviromentTrigger::Clone(void * pArg, _uint iSceneIndex)
{
	CEnviromentTrigger*	pInstance = new CEnviromentTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CEnviromentTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnviromentTrigger::Free()
{
	__super::Free();

	if(m_pColliderCom)
		m_pColliderCom->Remove_Actor();

#ifdef _DEBUG
	Safe_Release(m_pRenderCollider);
#endif
	Safe_Release(m_pSphereColliderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
