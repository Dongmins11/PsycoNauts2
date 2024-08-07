#include "stdafx.h"
#include "..\public\EndingUI.h"
#include "GameInstance.h"
#include "QuestManager.h"
CEndingUI::CEndingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CEndingUI::CEndingUI(const CEndingUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEndingUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEndingUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_matrix mat = *(_matrix*)pArg;
	_float4x4 fmat = STOREMATRIX(mat);
	m_iRenderPass = fmat.m[3][3];
	fmat.m[3][3] = 1.f;
	m_pTransformCom->Set_WorldMatrix(fmat);
	m_pTransformCom->Scaling(_float3(2.5f, 2.5f, 2.5f));
	return S_OK;
}

_int CEndingUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	return _int();
}

_int CEndingUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CEndingUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEndingUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_ENDING, TEXT("Prototype_Component_Texture_EndingUI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CEndingUI::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	_matrix scale = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(scale * m_pTransformCom->Get_WorldMatrix()));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
		
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, m_iRenderPass);
	
	m_pVIBufferCom->Render(11);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CEndingUI * CEndingUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEndingUI*	pInstance = new CEndingUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CEndingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEndingUI::Clone(void * pArg, _uint iCurrentScene)
{
	CEndingUI*	pInstance = new CEndingUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CEndingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEndingUI::Free()
{
	__super::Free();
	
	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
