#include "stdafx.h"
#include "..\public\WallCollision.h"
#include "GameInstance.h"

CWallCollision::CWallCollision(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CWallCollision::CWallCollision(const CWallCollision & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWallCollision::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CWallCollision::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_STATIC;

	_float3 vScale = { 30.f,30.f,0.1f };
	_float3 vColPos = { 0.f,0.f,0.f };
	_float3 vObjScale = { 1.f,1.f,1.f };

	if (FAILED(SetUp_Component(vScale, vColPos, vObjScale)))
		return E_FAIL;

	m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-161.048f, 13.57f, 109.727f,1.f));
	return S_OK;
}

_int CWallCollision::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	return _int();
}

_int CWallCollision::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

	return _int();
}

HRESULT CWallCollision::Render()
{
	return S_OK;
}



HRESULT CWallCollision::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 2.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColliderDesc.isVisualization = true;
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

	return S_OK;
}

HRESULT CWallCollision::SetUp_ConstantTable()
{
	return S_OK;
}

CWallCollision * CWallCollision::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWallCollision*	pInstance = new CWallCollision(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CWallCollision");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWallCollision::Clone(void * pArg, _uint iCurrentScene)
{
	CWallCollision*	pInstance = new CWallCollision(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CWallCollision");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWallCollision::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}