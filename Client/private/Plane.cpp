#include "stdafx.h"
#include "..\public\Plane.h"
//#include "PhysXCollider.h"
//#include "NavMeshCollider.h"
#include "GameInstance.h"

CPlane::CPlane(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CPlane::CPlane(const CPlane & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlane::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlane::NativeConstruct(void * pArg)
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 0.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	m_StrLayerName = TEXT("Layer_Plane");


	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pNaviCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PLANE);
	return S_OK;
}

_int CPlane::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	return 0;
}

_int CPlane::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return 0;
}

HRESULT CPlane::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlane::SetUp_Component()
{

	///* Com_Navigation */
	//CNavigation::NAVIDESC		NaviDesc;
	//ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));
	//
	//NaviDesc.iCurrentIndex = 0;
	//
	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	//	return E_FAIL;

	CPhysXCollider::DESC tColDesc;
	tColDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColDesc.isSceneQuery = true;
	tColDesc.pGameObject = this;
	//tColDesc.isKinematic = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CNavMeshCollider::DESC tNavMeshDesc;
	tNavMeshDesc.tColliderDesc = tColDesc;
	tNavMeshDesc.m_isPlane = true;
	vector<_float3> vVec = pGameInstance->Get_PointVector();
	RELEASE_INSTANCE(CGameInstance);

	for (_int i = 0; i < vVec.size(); i += 3)
	{
		_float3* pPoints = new _float3[3];
		pPoints[0] = vVec[i];
		pPoints[1] = vVec[i+1];
		pPoints[2] = vVec[i+2];
		tNavMeshDesc.vecPoints.emplace_back(pPoints);
	}

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_Navigation"), TEXT("Com_NavigationCollider"), (CComponent**)&m_pNaviCollider, &tNavMeshDesc)))
		return E_FAIL;

	m_pNaviCollider->Set_ShapeLayer(LayerType::LAYERTYPE_PLANE);
	_matrix smatPviot = XMMatrixTranslation(0.f, 0.1f, 0.f);
	//m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pNaviCollider->SetPivotMatrix(smatPviot);
	m_pNaviCollider->Update_TransformPhysX();
	m_pNaviCollider->Update_Transform();
	return S_OK;
}

HRESULT CPlane::SetUp_ConstantTable()
{
	return S_OK;
}


CPlane * CPlane::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlane*	pInstance = new CPlane(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlane::Clone(void * pArg, _uint iSceneIndex)
{
	CPlane*	pInstance = new CPlane(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlane::Free()
{
	__super::Free();

	if (m_pNaviCollider)
		m_pNaviCollider->Remove_Actor();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNaviCollider);
	Safe_Release(m_pNavigationCom);

}
