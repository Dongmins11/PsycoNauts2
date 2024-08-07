#include "stdafx.h"
#include "..\public\PlatingPlate_Phase1.h"
#include "GameInstance.h"

CPlatingPlate_Phase1::CPlatingPlate_Phase1(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CPlatingPlate_Phase1::CPlatingPlate_Phase1(const CPlatingPlate_Phase1 & rhs)
	:CGameObject(rhs)
	,m_vecNaviData(rhs.m_vecNaviData)
{
}

HRESULT CPlatingPlate_Phase1::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_isCreated = true;
	Loading_PlateNaviData(TEXT("../../Data/Stage03/NaviPlating1.dat"));
	return S_OK;
}

HRESULT CPlatingPlate_Phase1::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = TEXT("Layer_Plating_Plate1");//objinfo.StrLayerName;
	m_StrModelName = TEXT("Prototype_Component_Model_Compt_Plating_Plate1");//objinfo.StrModelName;
	m_StrObjectFullName = TEXT("Prototype_GameObject_Plating_Plate1"); //objinfo.StrObjectFullName;
	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_PLATINGPLATE;//objinfo.iPX_Index;
	m_eLayerType = (LayerType)m_iPX_TYPE;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_float4x4 fWorldMatrix = STOREMATRIX(XMMatrixScaling(0.8f,0.8f,0.8f));
	//XMStoreFloat4x4(&fWorldMatrix, XMMatrixIdentity());// objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,  XMVectorSet(236.6f,1.55f,192.6f,1.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	
#ifdef _DEBUG
	//SetUp_RenderComponentForDebug();
#endif // _DEBUG

	//m_pTriggerCom[0]->Set_ShapeLayer(m_eLayerType);
	//m_pTriggerCom[1]->Set_ShapeLayer(m_eLayerType);

	CPhysXCollider::DESC tColDesc;
	tColDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColDesc.isSceneQuery = true;
	tColDesc.pGameObject = this;
	CNavMeshCollider::DESC tNavMeshDesc;
	tNavMeshDesc.tColliderDesc = tColDesc;
	tNavMeshDesc.vecPoints = m_vecNaviData;
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_Navigation"), TEXT("Com_NavigationCollider"), (CComponent**)&m_pColliderCom, &tNavMeshDesc)))
		return E_FAIL;

	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_PLANE);
	_matrix smatPviot = XMMatrixIdentity();//m_pTransformCom->Get_WorldMatrix();
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	SetUp_Triggers();
	return S_OK;
}

_int CPlatingPlate_Phase1::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
	{
		m_pColliderCom->Remove_Actor();
		return 0;
	}
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);
	
	return _int();
}

_int CPlatingPlate_Phase1::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
	{
		m_pColliderCom->Remove_Actor();
		return 0;
	}
	//for (_uint i = 0; i < 2; ++i)
	//{
	//	if (m_pTriggerCom[i])
	//	{
	//		m_pTriggerCom[i]->Update_Transform();
	//
	//	}
	//}

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	
#ifdef _DEBUG
	//for(_uint i=0;i<2;++i)
	//	m_pTriggerRenderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG
	return 0;
}

HRESULT CPlatingPlate_Phase1::Render()
{
 	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

#ifdef _DEBUG
	//for(_uint i=0;i<2;++i)
	//	m_pTriggerRenderCom[i]->Render(COLOR_GREEN);
#endif // _DEBUG

	return S_OK;
}

_bool CPlatingPlate_Phase1::Checking_FruitCooking(FruitType eFruitType, CookType eCookType, _uint* iCookingUINumber)
{
	_int iCheckFruit = -1;
	for (_uint i = 0; i < m_vecPlateFruits.size(); ++i)
	{
		if (m_vecPlateFruits[i] == eFruitType)
		{
			iCheckFruit = i;
			*iCookingUINumber = i;

		}
	}

	if (iCheckFruit < 0)
		return false;

	if (((_uint)m_vecCookTypes[iCheckFruit] & (2 << (_uint)eCookType)) == (2 << (_uint)eCookType))
	{
		if (eCookType != CookType::COOK_CUT && (_uint)m_vecCookTypes[iCheckFruit] >=24)
		{
			if (iCheckFruit == 1)
				*iCookingUINumber = m_vecPlateFruits.size();
			else
				*iCookingUINumber = m_vecPlateFruits.size() + 2;
		}
		return true;
	}

	return false;
}

HRESULT CPlatingPlate_Phase1::SetUp_Component()
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


	return S_OK;
}


HRESULT CPlatingPlate_Phase1::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 TransPoseWorldMat, ViewTransPoseMat, ProjectionTransPoseMat;

	XMStoreFloat4x4(&TransPoseWorldMat, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjectionTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &TransPoseWorldMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjectionTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlatingPlate_Phase1::Loading_PlateNaviData(const _tchar * pNaviDataFilePath)
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

	//	memcpy(NaviDesc.vPoint, vPoints, sizeof(_float3) *3);
	//	NaviDesc.iByOption = iOption;

		for (_uint i = 0; i < 3; ++i)
			vPoints[i] = STOREFLOAT3(XMLoadFloat3(&vPoints[i]) * 0.8f);

		m_vecNaviData.emplace_back(vPoints);
		//pGameInstance->Add_PointsToVector(vPoints);
		//m_vecPoints.emplace_back(vPoints);
	}

	CloseHandle(hFile);

	return S_OK;

}

HRESULT CPlatingPlate_Phase1::SetUp_Triggers()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix mat = XMMatrixScaling(4.f, 3.f, 4.f) * XMMatrixIdentity();//XMMatrixTranslation(-4.f, 1.f, 0.f);
	_vector LookVec = mat.r[2];
	LookVec = XMVectorSetW(LookVec, (_float)(2 << (_uint)CookType::COOK_BOIL));
	mat.r[2] = LookVec;
	mat.r[3] = XMVectorSet(236.5f,3.f, 197.5f, (_float)FruitType::FRUIT_EGG);
	
	m_vecPlateFruits.emplace_back(FruitType::FRUIT_EGG);
	m_vecCookTypes.emplace_back((2 << (_uint)CookType::COOK_BOIL));

	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STAGE03, TEXT("Prototype_GameObject_FruitTrigger"), SCENE_STAGE03, TEXT("Layer_PlateTrigger"), m_pFruitCollider,&mat);

	mat = XMMatrixScaling(4.f, 3.f, 4.f) * XMMatrixIdentity();//XMMatrixTranslation(-4.f, 1.f, 0.f);
	LookVec = mat.r[2];
	LookVec = XMVectorSetW(LookVec, (2 << (_uint)CookType::COOK_CUT)+ (2 << (_uint)CookType::COOK_BAKE));
	mat.r[2] = LookVec;
	mat.r[3] = XMVectorSet(240.5f,3.f,190.f, (_float)FruitType::FRUIT_BREAD);
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STAGE03, TEXT("Prototype_GameObject_FruitTrigger"), SCENE_STAGE03, TEXT("Layer_PlateTrigger"), m_pFruitCollider, &mat);

	m_vecPlateFruits.emplace_back(FruitType::FRUIT_BREAD);
	m_vecCookTypes.emplace_back((2 << (_uint)CookType::COOK_CUT) + (2 << (_uint)CookType::COOK_BAKE));


	mat = XMMatrixScaling(4.f, 3.f, 4.f) * XMMatrixIdentity();//XMMatrixTranslation(-4.f, 1.f, 0.f);
	LookVec = mat.r[2];
	LookVec = XMVectorSetW(LookVec, (2 << (_uint)CookType::COOK_BLEND));
	mat.r[2] = LookVec;
	mat.r[3] = XMVectorSet(233.f,3.f,190.f, (_float)FruitType::FRUIT_STRAWBERRY);
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STAGE03, TEXT("Prototype_GameObject_FruitTrigger"), SCENE_STAGE03, TEXT("Layer_PlateTrigger"), m_pFruitCollider, &mat);

	m_vecPlateFruits.emplace_back(FruitType::FRUIT_STRAWBERRY);
	m_vecCookTypes.emplace_back((2 << (_uint)CookType::COOK_BLEND));


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

#ifdef _DEBUG
HRESULT CPlatingPlate_Phase1::SetUp_RenderComponentForDebug()
{
	//_float3 vScales = m_pTransformCom->Get_ScaleFloat3();
	///* For.Com_TriggerRender */
	//CCollider::COLLIDERDESC		ColliderDesc;
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//
	//ColliderDesc.vCenter = _               
	//ColliderDesc.vExtents = _float3(2.75f * vScales.x, .5f* vScales.y, 5.f * vScales.z);
	//ColliderDesc.fRadius = 1.f;
	//
	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_TriggerRender0"), (CComponent**)&m_pTriggerRenderCom[0], &ColliderDesc)))
	//	return E_FAIL;
	//
	//
	//ColliderDesc.vCenter = _float3(4.f, 1.0f, 0.0f);
	//ColliderDesc.vExtents = _float3(2.75f * vScales.x, .5f* vScales.y, 5.f * vScales.z);
	//ColliderDesc.fRadius = 1.f;
	//
	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_TriggerRender1"), (CComponent**)&m_pTriggerRenderCom[1], &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}
#endif

CPlatingPlate_Phase1 * CPlatingPlate_Phase1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlatingPlate_Phase1*	pInstance = new CPlatingPlate_Phase1(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlatingPlate_Phase1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlatingPlate_Phase1::Clone(void * pArg, _uint iCurrentScene)
{
	CPlatingPlate_Phase1*	pInstance = new CPlatingPlate_Phase1(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CPlatingPlate_Phase1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlatingPlate_Phase1::Free()
{
	__super::Free();

	
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	m_vecCookTypes.clear();
	m_vecPlateFruits.clear();
	//for (_uint i = 0; i < 2; ++i)
	//{
	//	if (m_pTriggerCom[i])
	//		m_pTriggerCom[i]->Remove_Actor();
	//}

#ifdef _DEBUG
	//for (_uint i = 0; i < 2; ++i)
	//{
	//	Safe_Release(m_pTriggerRenderCom[i]);
	//}
#endif // _DEBUG
	//for (_uint i = 0; i < 2; ++i)
	//{
	//	Safe_Release(m_pTriggerCom[i]);
	//}
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
