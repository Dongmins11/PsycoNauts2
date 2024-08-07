#include "stdafx.h"
#include "..\public\LoadingThreadUI.h"
#include "GameInstance.h"

CLoadingThreadUI::CLoadingThreadUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CLoadingThreadUI::CLoadingThreadUI(const CLoadingThreadUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CLoadingThreadUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingThreadUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	// 버튼처리
	m_isRender = true;
	return S_OK;
}

_int CLoadingThreadUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!m_isFinished)
	{
		m_lfTimeAcc += TimeDelta;
		if (m_lfTimeAcc > m_lfTimeMax)
		{
			++m_iCurrentIndex;
			++m_iLoadingTextureIndex;
			m_lfTimeAcc = 0.0;
		}
		if (m_iCurrentIndex > (m_fImgSize.x * m_fImgSize.y) - 1)
			m_iCurrentIndex = 0;

		if (m_iLoadingTextureIndex > 4)
			m_iLoadingTextureIndex = 1;
	}
	else
	{
		m_lfDestroyAcc += TimeDelta;
		if (m_lfDestroyAcc > m_lfDestroyMax)
			m_isDestroy = true;
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return _int();
}

_int CLoadingThreadUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CLoadingThreadUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingThreadUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_LoadingThread"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_LoadingTexture"), (CComponent**)&m_pVIBufferLoadingCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingThreadUI::SetUp_ConstantTable()
{
	// Background
	if (!m_isFinished)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		m_fSizeX = 73, m_fSizeY = 73;
		m_fX = 1065;
		m_fY = 55;
		WorldMatrix._11 = m_fSizeX;
		WorldMatrix._22 = m_fSizeY;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

		m_fCurrentIndex.y = m_iCurrentIndex / 7;
		m_fCurrentIndex.x = m_iCurrentIndex % 7;


		_float4x4	ViewMatrix, ProjMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		m_pVIBufferCom->Set_RawValue("g_vRatio", &m_fCurrentIndex, sizeof(_float2));
		m_pVIBufferCom->Set_RawValue("g_vRatioMax", &m_fImgSize, sizeof(_float2));

		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 0);

		m_pVIBufferCom->Render(27);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		m_fSizeX = 250, m_fSizeY = 35;
		m_fX = 1140;
		m_fY = 75;
		WorldMatrix._11 = m_fSizeX;
		WorldMatrix._22 = m_fSizeY;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);


		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferLoadingCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferLoadingCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferLoadingCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferLoadingCom, m_iLoadingTextureIndex);

		m_pVIBufferLoadingCom->Render(11);
	}
	else
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		m_fSizeX = 250, m_fSizeY = 35;
		m_fX = 1030;
		m_fY = 75;
		WorldMatrix._11 = m_fSizeX;
		WorldMatrix._22 = m_fSizeY;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

		_float4x4	ViewMatrix, ProjMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 5);

		m_pVIBufferCom->Render(11);
	}
	
	return S_OK;
}

CLoadingThreadUI * CLoadingThreadUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLoadingThreadUI*	pInstance = new CLoadingThreadUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CLoadingThreadUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoadingThreadUI::Clone(void * pArg, _uint iCurrentScene)
{
	CLoadingThreadUI*	pInstance = new CLoadingThreadUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CLoadingThreadUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadingThreadUI::Free()
{
	__super::Free();
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferLoadingCom);
}
