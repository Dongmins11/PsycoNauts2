#include "stdafx.h"
#include "..\public\BadIdea_Spawner.h"
#include "GameInstance.h"

CBadIdea_Spawner::CBadIdea_Spawner(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBadIdea_Spawner::CBadIdea_Spawner(const CBadIdea_Spawner & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBadIdea_Spawner::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBadIdea_Spawner::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4		WorldMatrix;

	if (nullptr != pArg)
		WorldMatrix = *(_float4x4*)pArg;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	WorldMatrix._42 -= 1.f;
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pModelCom->Set_Animation(0);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BadIdeaSpawn"),
		SCENE_STATIC, TEXT("Effect_BadIdeaSpawn"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Effect_Call_WorldPos(m_iCurrentScene, TEXT("Layer_BadIdeaSpawn"), m_pTransformCom->Get_Position() + XMVectorSet(0.f, 2.5f, 0.f, 0.f), true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return -1;
	}

	pGameInstance->Setting_Effect_Speed(m_iCurrentScene, TEXT("Layer_BadIdeaSpawn"), TEXT("Com_Option"), 0.5);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CBadIdea_Spawner::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pModelCom->Lerp_Update(TimeDelta);

	if (m_pModelCom->Get_IsAnimFinishied(0)&&
		!m_IsSpawned)
	{
		m_IsSpawned = true;
		m_pModelCom->Set_IsAnimLefp(true);
		m_pModelCom->Set_Animation(1);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pModelCom->Set_IsAnimLefp(true);
		pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BadIdea"), m_iCurrentScene, TEXT("Layer_BadIdea"), &m_pTransformCom->Get_WorldFloat4x4());
		RELEASE_INSTANCE(CGameInstance);
		m_isDestroy = true;
	}

	return _int();
}

_int CBadIdea_Spawner::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CBadIdea_Spawner::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}




HRESULT CBadIdea_Spawner::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_BadIdeaSpawner"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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

HRESULT CBadIdea_Spawner::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	_float fLightRange = 30.f;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
	{
		m_vRGBA = m_pRendererCom->Get_RimLightColorFloat4();
		m_fRimPower = m_pRendererCom->Get_RimLightPower();
	}
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));

	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float4(-1.f, -1.f, -1.f, 0.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBadIdea_Spawner * CBadIdea_Spawner::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBadIdea_Spawner*	pInstance = new CBadIdea_Spawner(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBadIdea_Spawner::Clone(void * pArg, _uint iCurrentScene)
{
	CBadIdea_Spawner*	pInstance = new CBadIdea_Spawner(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBadIdea_Spawner::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
