#include "stdafx.h"
#include "..\public\PlayerTrigger_Parkour.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "EnviromentTrigger.h"

CPlayerTrigger_Parkour::CPlayerTrigger_Parkour(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CPlayerTrigger(pDevice, pDeviceContext)
{
}

CPlayerTrigger_Parkour::CPlayerTrigger_Parkour(const CPlayerTrigger_Parkour & rhs)
	: CPlayerTrigger(rhs)
{
}

HRESULT CPlayerTrigger_Parkour::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerTrigger_Parkour::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_PLAYER_TRIGGER;
	m_eTriggerType = PlayerTriggerType::PTTYPE_CHECKHEAD;
	_float3 vColSize = { 0.7f, 0.6f, 1.f };
	m_vColPos = { 0.f,1.3f,0.0f };//objinfo.fCollider_Center;
	
	_float3 vObjScale = { 1.f,1.f,1.f };//GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(vColSize, m_vColPos, vObjScale)))
		return E_FAIL;


	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, XMMatrixTranslation(m_vColPos.x, m_vColPos.y, m_vColPos.z));

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	return S_OK;
}

_int CPlayerTrigger_Parkour::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Get_IsRender())
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CPlayerTrigger_Parkour::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Get_IsRender())
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (m_pColliderCom)
		m_pColliderCom->Update_Transform();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CPlayerTrigger_Parkour::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	//m_pRenderCollider->Render(COLOR_ORANGE);
#endif // _DEBUG

	return S_OK;
}

void CPlayerTrigger_Parkour::OnTriggerEnter(CCollisionObject & collision)
{
	//
	switch (collision.Get_Object()->Get_ObjectLayerType())
	{
	case LayerType::LAYERTYPE_TRIGGER:
	{
		/* 수정 필요 */
		if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_PARKOUR)
		{
			// 여기서 트리거에 체킹됐을때 해당 트리거의 값을 바꿔줌
			OutputDebugStringW(TEXT("OnTriggerEnter - 플레이어 파쿠르 트리거가 Enter됌\n"));

			TriggerEnter_ForPlayerLedge(collision);
		}
		break;
	}
	default:
		break;
	}
}
//
//void CPlayerTrigger_Parkour::OnTriggerStay(CCollisionObject & collision)
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
void CPlayerTrigger_Parkour::OnTriggerExit(CCollisionObject & collision)
{
	switch (collision.Get_Object()->Get_ObjectLayerType())
	{
	case LayerType::LAYERTYPE_TRIGGER:
	{
		/* 수정 필요 */
		if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_PARKOUR)
		{
			// 여기서 트리거에 체킹됐을때 해당 트리거의 값을 바꿔줌
			OutputDebugStringW(TEXT("OnTriggerEnter - 플레이어 파쿠르 트리거가 Exit됌\n"));
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
			if (nullptr == pPlayer)
				return;
			pPlayer->Set_IsParkour(false);

			//TriggerEnter_ValueReset();

			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	default:
		break;
	}
}

void CPlayerTrigger_Parkour::OnControllerCollisionHit(CCollisionObject & collision)
{
}


void CPlayerTrigger_Parkour::Set_Position(_fvector vPos)
{
	if (!m_pTransformCom || !m_pPlayerTransform)
		return;

	_vector vec = XMVector3TransformCoord(XMLoadFloat3(&m_vColPos), XMLoadFloat4x4(m_pPlayerTransform));
	_matrix matTrigger = XMMatrixTranslationFromVector(XMLoadFloat3(&m_vColPos));
	_float4x4 TriggerMatrix = STOREMATRIX(matTrigger * XMLoadFloat4x4(m_pPlayerTransform));
	m_pTransformCom->Set_WorldMatrix(TriggerMatrix);

#ifdef _DEBUG
	m_pRenderCollider->Update(matTrigger * XMLoadFloat4x4(m_pPlayerTransform));
#endif // _DEBUG

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();
}

void CPlayerTrigger_Parkour::Set_Matrix(const _float4x4 * pMatPtr)
{
	m_pPlayerTransform = pMatPtr;
}

HRESULT CPlayerTrigger_Parkour::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

		tColliderDesc.isSceneQuery = true;

		CSphereCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = vColSize;

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		m_pColliderCom->Set_ShapeLayer(m_eLayerType);

		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

	CCollider::COLLIDERDESC		ColliderDesc;
	//_float3 vColSize = { 0.5f, 0.1f, 1.f };
	//m_vColPos = { 0.f,2.f,0.5f };//objinfo.fCollider_Center;

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vExtents = _float3(0.35f, 0.3f, 0.5f);
	ColliderDesc.fRadius = 1.f;

#ifdef _DEBUG


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pRenderCollider, &ColliderDesc)))
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}

void CPlayerTrigger_Parkour::TriggerEnter_ForPlayerLedge(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	CTransform* pPlayerTransform = (CTransform*)pPlayer->Get_Components(L"Com_Transform");
	if (nullptr == pPlayer || nullptr == pPlayerTransform)
		return;
	pPlayer->Set_IsParkour(true);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	if (nullptr == pTriggerObj)
		return;

	_float4 vPlayerPositionfloat;
	_float4 vTriggerPositionfloat;

	_matrix TriggerWorldMat = pTriggerObj->Get_WorldMatrix();

	_vector vRight = pTriggerObj->Get_WorldMatrix().r[0];
	_vector vUp = pTriggerObj->Get_WorldMatrix().r[1];
	_vector vLook = pTriggerObj->Get_WorldMatrix().r[2];
	_vector vPosition = pTriggerObj->Get_WorldMatrix().r[3];

	_matrix			TriggerTransformationMatrix;
	_matrix			PivotMatrix;
	_vector			TriggerScale;
	_vector			TiggerRight, TriggerUp, TriggerLook;


	TriggerScale = XMVectorSet(XMVectorGetX(XMVector3Length(pTriggerObj->Get_WorldMatrix().r[0])), XMVectorGetY(XMVector3Length(pTriggerObj->Get_WorldMatrix().r[1])), XMVectorGetZ(XMVector3Length(pTriggerObj->Get_WorldMatrix().r[2])), 1.f);

	TiggerRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(TriggerScale);
	TriggerUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(TriggerScale);
	TriggerLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(TriggerScale);

	TriggerWorldMat.r[0] = TiggerRight;
	TriggerWorldMat.r[1] = TriggerUp;
	TriggerWorldMat.r[2] = TriggerLook;

	XMStoreFloat4(&vPlayerPositionfloat, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&vTriggerPositionfloat, pTriggerObj->Get_WorldMatrix().r[3]);

	_float Test = 0.f;


	if (1 == XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2]))))
	{
		m_bLookChecking[TRIGGER_LOOK_STATE::LOOK_RIGHT] = true;
		vPlayerPositionfloat = _float4(vPlayerPositionfloat.x, vTriggerPositionfloat.y - 0.75f, vTriggerPositionfloat.z, 1.f);
	}
	Test = XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2])));



	TriggerWorldMat *= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	if (1 == XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2]))))
	{
		m_bLookChecking[TRIGGER_LOOK_STATE::LOOK_BACK] = true;
		vPlayerPositionfloat = _float4(vTriggerPositionfloat.x, vTriggerPositionfloat.y - 0.75f, vPlayerPositionfloat.z, 1.f);
	}
	Test = XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2])));



	TriggerWorldMat *= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	if (1 == XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2]))))
	{
		m_bLookChecking[TRIGGER_LOOK_STATE::LOOK_LEFT] = true;
		vPlayerPositionfloat = _float4(vPlayerPositionfloat.x, vTriggerPositionfloat.y - 0.75f, vTriggerPositionfloat.z, 1.f);
	}
	Test = XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2])));



	TriggerWorldMat *= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	if (0.3 <= XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2]))) && 0.4 >= XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2]))))
	{
		m_bLookChecking[TRIGGER_LOOK_STATE::LOOK_FRONT] = true;
		vPlayerPositionfloat = _float4(vTriggerPositionfloat.x, vTriggerPositionfloat.y - 0.73f, vPlayerPositionfloat.z, 1.f);
	}
	Test = XMVectorGetZ(XMVector4Dot(XMVector4Normalize(vLook), XMVector4Normalize(TriggerWorldMat.r[2])));



	XMStoreFloat4(&m_vPosition, XMLoadFloat4(&vPlayerPositionfloat) - (XMVector3Normalize(vLook) * 0.14));
	
	XMStoreFloat4(&m_vStartPosition, vPosition - (vRight/2.f));
	
	XMStoreFloat4(&m_vEndPosition, vPosition + (vRight/2.f));

	XMStoreFloat3(&m_vLookDir, vLook);

	XMStoreFloat3(&m_vRightMoveDir, XMVector3Normalize(vRight));
	
	XMStoreFloat3(&m_vLeftMoveDir, -XMVector3Normalize(vRight));


	RELEASE_INSTANCE(CGameInstance);


}

void CPlayerTrigger_Parkour::TriggerEnter_ValueReset()
{
	for (_uint i = 0; i < TRIGGER_LOOK_STATE::LOOK_END; ++i)
		m_bLookChecking[i] = false;

	m_vPosition = {};
	m_vStartPosition = {};
	m_vEndPosition = {};
	m_vLookDir = {};
	m_vRightMoveDir = {};
	m_vLeftMoveDir = {};
}



CPlayerTrigger_Parkour * CPlayerTrigger_Parkour::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerTrigger_Parkour*	pInstance = new CPlayerTrigger_Parkour(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayerTrigger_Parkour");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerTrigger_Parkour::Clone(void * pArg, _uint iCurrentScene = 0)
{
	CPlayerTrigger_Parkour*	pInstance = new CPlayerTrigger_Parkour(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayerTrigger_Parkour");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerTrigger_Parkour::Free()
{
	__super::Free();
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();
	
#ifdef _DEBUG
	Safe_Release(m_pRenderCollider);
#endif // _DEBUG

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
