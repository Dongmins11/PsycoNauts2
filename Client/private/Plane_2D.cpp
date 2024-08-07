#include "stdafx.h"
#include "..\public\Plane_2D.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Camera_Fly.h"

CPlane_2D::CPlane_2D(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPlane_2D::CPlane_2D(const CPlane_2D & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlane_2D::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlane_2D::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;//objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;//objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName; //objinfo.StrObjectFullName;
	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_2DMAP;//objinfo.iPX_Index;
	m_eLayerType = (LayerType)m_iPX_TYPE;

	Loading_NaviData(TEXT("../../Data/Navi_NewBook.dat"));

	//_float3 vColSize = objinfo.fCollider_Extent;
	//_float3 vColPos = objinfo.fCollider_Center;
	//_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(/*vColSize, vColPos, vObjScale*/)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);// objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	m_pNaviCollider->Update_TransformPhysX();

	m_isRender = false;

	return S_OK;
}

_int CPlane_2D::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	
	if (m_IsTriggerRender)
		m_isRender = true;
	else
		m_isRender = false;

	m_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_pNaviCollider->Update_TransformPhysX(XMLoadFloat4x4(&m_WorldMatrix));

	return _int();
}

_int CPlane_2D::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	//m_pNaviCollider->Update_Transform();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CPlane_2D::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if(m_isRender)
		m_pModelCom->Render(m_iRenderPass);
	
	return S_OK;
}

HRESULT CPlane_2D::Loading_NaviData(const _tchar * pNaviDataFilePath)
{
	HANDLE			hFile = 0;
	_uint			iCntPoint = 3;
	_ulong	dwByte = 0;
	hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	NAVIMESHDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

	_float3*	vPoints = nullptr;
	_uint			iOption = 0;

	vector<_float3*> vVec;
	while (true)
	{
		vPoints = new _float3[3];
		ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete_Array(vPoints);
			break;
		}
		m_vecNaviData.emplace_back(vPoints);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CPlane_2D::SetUp_Component(/*_float3 vColSize, _float3 vColPos, _float3 vObjScale*/)
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColDesc;
	tColDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColDesc.isSceneQuery = true;
	tColDesc.pGameObject = this;

	CNavMeshCollider::DESC tNavMeshDesc;
	tNavMeshDesc.tColliderDesc = tColDesc;
	tNavMeshDesc.vecPoints = m_vecNaviData;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_Navigation"), TEXT("Com_NavigationCollider"), (CComponent**)&m_pNaviCollider, &tNavMeshDesc)))
		return E_FAIL;

	m_pNaviCollider->Set_ShapeLayer(LayerType::LAYERTYPE_2DMAP);


	return S_OK;
}

HRESULT CPlane_2D::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CPlane_2D * CPlane_2D::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlane_2D*	pInstance = new CPlane_2D(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlane_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlane_2D::Clone(void * pArg, _uint iCurrentScene)
{
	CPlane_2D*	pInstance = new CPlane_2D(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CPlane_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlane_2D::Free()
{
	__super::Free();

	if (m_pNaviCollider)
	{
		m_pNaviCollider->Remove_Actor();
		Safe_Release(m_pNaviCollider);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
