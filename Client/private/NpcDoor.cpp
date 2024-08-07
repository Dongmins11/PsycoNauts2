#include "stdafx.h"
#include "..\public\NpcDoor.h"
#include "GameInstance.h"

CNpcDoor::CNpcDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CNpcDoor::CNpcDoor(const CNpcDoor & rhs)
	:CGameObject(rhs)
{
}

HRESULT CNpcDoor::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CNpcDoor::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_iPX_TYPE = objinfo.iPX_Index;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0);
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	Setting_MyCount();
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() + XMVectorSet(0.f, 5.f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	//m_pCollider->Set_ShapeLayer(LayerType::LAYERTYPE_PICK);
	return S_OK;
}

_int CNpcDoor::Tick(_double TimeDelta)
{
	if (true == m_bMove)
		MoveAnimation(TimeDelta);
	else
		m_pModelCom->Update(0.f);

	return _int();
}

_int CNpcDoor::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	//m_pCollider->Update_Transform();
	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CNpcDoor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CNpcDoor::Setting_Throwing(_fvector vDir)
{
}

void CNpcDoor::Setting_MyCount()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iMyCount = pGameInstance->Get_LayerSize(m_iCurrentScene, TEXT("Layer_NpcDoor"));

	RELEASE_INSTANCE(CGameInstance);
}

void CNpcDoor::MoveAnimation(_double TimeDelta)
{
	_double  AimtionTime = TimeDelta;
	
	if (true == m_pModelCom->Get_IsAnimFinishied(0))
	{
		AimtionTime = 0.0;
		m_Finsh = true;
	}
	if (false == m_Finsh)
		m_pModelCom->Lerp_Update(AimtionTime);

	//if (false == m_Finsh)
	//m_pModelCom->Lerp_Update(AimtionTime);
}

HRESULT CNpcDoor::SetUp_Component()
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

HRESULT CNpcDoor::SetUp_ConstantTable()
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

CNpcDoor * CNpcDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNpcDoor*	pInstance = new CNpcDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CMpcDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNpcDoor::Clone(void * pArg, _uint iCurrentScene)
{
	CNpcDoor*	pInstance = new CNpcDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CMpcDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNpcDoor::Free()
{
	__super::Free();

//	Safe_Release(m_pCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
