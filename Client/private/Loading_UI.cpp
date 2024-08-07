#include "stdafx.h"
#include "..\public\Loading_UI.h"
#include "GameInstance.h"

CLoading_UI::CLoading_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CLoading_UI::CLoading_UI(const CLoading_UI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CLoading_UI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_UI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	
	// Spinner Size
	m_fSpinnerSize = 150.f;

	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	return S_OK;
}

_int CLoading_UI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	// Spinner
	m_dTimeAcc -= TimeDelta * SPINNERSPEED;

	//if (m_IsLoad)
	//{
		m_fTimeAcc += (_float)TimeDelta;
		m_dTemp += TimeDelta;

		_float fTemp = 0.f;
		m_dTemp /= MAXFRAME / 2.f;
		if (1.f > fTemp)
			fTemp = (_float)(MAXFRAME / 2.f * m_dTemp * m_dTemp * m_dTemp * m_dTemp * m_dTemp);

		m_dTemp -= 1.0;
		fTemp = (_float)(MAXFRAME / 2.f * (m_dTemp * m_dTemp * m_dTemp * m_dTemp * m_dTemp + 2.f));
		m_fTemp = fTemp;
	//}

	return _int();
}

_int CLoading_UI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CLoading_UI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_UI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Loading"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Texture_Mix */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Loading_Mix"), TEXT("Com_Texture_Mix"), (CComponent**)&m_pTextureMixCom)))
		return E_FAIL;

	/* Com_Model_BG */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_BG"), (CComponent**)&m_pModelBGCom)))
		return E_FAIL;

	/* Com_Model_Spinner */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Spinner"), (CComponent**)&m_pModelSpinnerCom)))
		return E_FAIL;

	/* Com_Model_Spiral */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Spiral"), (CComponent**)&m_pModelSpiralCom)))
		return E_FAIL;

	/* Com_Model_Mix */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Mix"), (CComponent**)&m_pModelMixCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_UI::SetUp_ConstantTable()
{
	// BG + Spiral
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = g_iWinCX;
	WorldMatrix._22 = g_iWinCY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 0.5f;

	_float4x4	ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	
	m_pTextureCom->SetUp_OnShader("g_NoiseTexture", m_pModelBGCom, 0);
	m_pTextureCom->SetUp_OnShader("g_SpiralTexture", m_pModelSpiralCom, 0);
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom, 3);
	m_pTextureCom->SetUp_OnShader("g_MixTexture", m_pModelSpiralCom, 3);
	 
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fX * TEXTSCALE * cosf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._12 = m_fX * TEXTSCALE * sinf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._21 = m_fX * TEXTSCALE * -sinf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._22 = m_fX * TEXTSCALE * cosf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 0.5f;


	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSpiralCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSpiralCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSpiralCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_NoiseTexture", m_pModelBGCom, 2);
	m_pTextureCom->SetUp_OnShader("g_SpiralTexture", m_pModelSpiralCom, 2);
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom, 4);
	m_pTextureCom->SetUp_OnShader("g_MixTexture", m_pModelSpiralCom, 4);

	m_pModelBGCom->Render(9);
	m_pModelSpiralCom->Render(9);

	// FIFO
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fX * m_fTemp * cosf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._12 = m_fX * m_fTemp * sinf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._21 = m_fX * m_fTemp * -sinf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._22 = m_fX * m_fTemp * cosf(XMConvertToRadians(-(_float)m_dTimeAcc));
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelMixCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelMixCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelMixCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureMixCom->SetUp_OnShader("g_DiffuseTexture", m_pModelMixCom, 0);
	m_pTextureMixCom->SetUp_OnShader("g_MixTexture", m_pModelMixCom, 1);

	if (1.f <= m_fTimeAcc)
		m_fTimeAcc = 1.f;

	m_pModelMixCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	// Spinner
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fSpinnerSize * cosf(XMConvertToRadians((_float)m_dTimeAcc));
	WorldMatrix._12 = m_fSpinnerSize * sinf(XMConvertToRadians((_float)m_dTimeAcc));
	WorldMatrix._21 = m_fSpinnerSize * -sinf(XMConvertToRadians((_float)m_dTimeAcc));
	WorldMatrix._22 = m_fSpinnerSize * cosf(XMConvertToRadians((_float)m_dTimeAcc));
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSpinnerSize / 2.f) - (g_iWinCX * 0.5f) + 1120;
	WorldMatrix._42 = -(m_fSpinnerSize / 2.f) + (g_iWinCY * 0.5f) - 560;
	WorldMatrix._43 = 0.3f;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSpinnerCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSpinnerCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSpinnerCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSpinnerCom, 5);

	m_pModelSpinnerCom->Render(8);

	if (m_IsLoad)
	{
		m_pModelMixCom->Render(10);
	}

	return S_OK;
}

CLoading_UI * CLoading_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLoading_UI*	pInstance = new CLoading_UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CLoading_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoading_UI::Clone(void * pArg, _uint iCurrentScene)
{
	CLoading_UI*	pInstance = new CLoading_UI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CLoading_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoading_UI::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureMixCom);

	Safe_Release(m_pModelBGCom);
	Safe_Release(m_pModelSpinnerCom);
	Safe_Release(m_pModelMixCom);
	Safe_Release(m_pModelSpiralCom);
}
