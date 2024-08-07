#include "stdafx.h"
#include "..\public\Background.h"
#include "GameInstance.h"

CBackground::CBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBackground::CBackground(const CBackground & rhs)
	:CGameObject(rhs)
{
}

HRESULT CBackground::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBackground::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_fLogoSizeX = 940;
	m_fLogoSizeY = 240;

	m_fTextSizeX = 420;
	m_fTextSizeY = 60;

	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));

	return S_OK;
}

_int CBackground::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_fTimeAcc -= TimeDelta;

	if (0.f >= m_fTimeAcc)
		m_fTimeAcc = 1.f;

	return _int();
}

_int CBackground::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	return _int();
}

HRESULT CBackground::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Texture_Title */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Title"), TEXT("Com_Texture_Title"), (CComponent**)&m_pTextureTitleCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Model_Text */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Text"), (CComponent**)&m_pModelTextCom)))
		return E_FAIL;

	/* Com_Model_Title */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Title"), (CComponent**)&m_pModelTitleCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom || nullptr ==  m_pModelTitleCom)
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = g_iWinCX;
	WorldMatrix._22 = g_iWinCY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 0.5;


	_float4x4	ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelCom, 0);

	m_pModelCom->Render(0);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fLogoSizeX;
	WorldMatrix._22 = m_fLogoSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fLogoSizeX * 0.5f) - (g_iWinCX * 0.5f) + 160;
	WorldMatrix._42 = -(m_fLogoSizeY * 0.5f) + (g_iWinCY * 0.5f) - 130;
	WorldMatrix._43 = 0.5;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelTitleCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelTitleCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelTitleCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureTitleCom->SetUp_OnShader("g_DiffuseTexture", m_pModelTitleCom, 0);

	m_pModelTitleCom->Render(8);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fTextSizeX;
	WorldMatrix._22 = m_fTextSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f) - 40;
	WorldMatrix._43 = 0.45;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelTextCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelTextCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelTextCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pModelTextCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelTextCom, 2);

	m_pModelTextCom->Render(7);

	return S_OK;
}

CBackground * CBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBackground*	pInstance = new CBackground(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBackground");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBackground::Clone(void * pArg, _uint iCurrentScene)
{
	CBackground*	pInstance = new CBackground(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBackground");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackground::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureTitleCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelTextCom);
	Safe_Release(m_pModelTitleCom);
}
