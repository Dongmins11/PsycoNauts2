#include "stdafx.h"
#include "..\public\CookingProgressBarUI.h"
#include "GameInstance.h"

CCookingProgressBarUI::CCookingProgressBarUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCookingProgressBarUI::CCookingProgressBarUI(const CCookingProgressBarUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CCookingProgressBarUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookingProgressBarUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	ORTHTEXUTREDESC tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(ORTHTEXUTREDESC));

	memcpy(&tTextureDesc, pArg, sizeof(ORTHTEXUTREDESC));

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_fSizeX = (_float)tTextureDesc.iSize.x;
	m_fSizeY = (_float)tTextureDesc.iSize.y;

	m_fX = (_float)tTextureDesc.iPos.x;
	m_fY = (_float)tTextureDesc.iPos.y;
	// 만약 time이 없고 바로 끝나는 친구면 1 아니면 0
	m_iCurrentImgIndex = tTextureDesc.iImgNum;
	// passNum이 1이면 기본, 2면 1/2크기
	m_iRenderPass = tTextureDesc.iPassNum;
	m_fViewZ = tTextureDesc.iSortZ;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	m_isRender = tTextureDesc.bRenderInit;

	m_lfTimeMax = 60.0;
	m_isFinished = (_bool)m_iCurrentImgIndex;
	return S_OK;
}

_int CCookingProgressBarUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	return _int();
}

_int CCookingProgressBarUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (!m_isFinished)
	{
		if (m_lfTimeAcc >= m_lfTimeMax)
		{
			m_lfTimeAcc = m_lfTimeMax;
			m_isFinished = true;
		}
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CCookingProgressBarUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookingProgressBarUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STAGE03, TEXT("Prototype_Component_Texture_Cooking_ProgressBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_VIBufferBackground */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBufferBackground"), (CComponent**)&m_pVIBufferBackgroundCom)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CCookingProgressBarUI::SetUp_ConstantTable()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


	if (!m_isFinished)
	{
		WorldMatrix._11 = 41/ m_iRenderPass;
		WorldMatrix._22 = 41/ m_iRenderPass;
		WorldMatrix._33 = 1.0f;
		WorldMatrix._41 = ((m_fX-(4/ m_iRenderPass)) + 41 / m_iRenderPass *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -((m_fY-(4/ m_iRenderPass)) + 41 / m_iRenderPass * 0.5f) + (g_iWinCY * 0.5f);
		WorldMatrix._43 = 1.f;
		_float4x4	ViewMatrix, ProjMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferBackgroundCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferBackgroundCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferBackgroundCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferBackgroundCom, 1);

		//33
		m_pVIBufferBackgroundCom->Render(11);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = m_fSizeX / m_iRenderPass;
		WorldMatrix._22 = m_fSizeY / m_iRenderPass;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = (m_fX + m_fSizeX / m_iRenderPass *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -(m_fY + m_fSizeY / m_iRenderPass * 0.5f) + (g_iWinCY * 0.5f);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
		//g_fRatio
		_float fRatio = (_float)(m_lfTimeAcc / m_lfTimeMax);
		m_pVIBufferCom->Set_RawValue("g_fRatio", &fRatio, sizeof(_float));

		//g_RadiusLoadingTexture
		m_pTextureCom->SetUp_OnShader("g_RadiusLoadingTexture", m_pVIBufferCom, 0);
		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 2);

		//33
		m_pVIBufferCom->Render(33);
	}
	else
	{
		m_fSizeX = 24 / m_iRenderPass;
		m_fSizeY = 24 / m_iRenderPass;

		WorldMatrix._11 = m_fSizeX;
		WorldMatrix._22 = m_fSizeY;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = ((m_fX+(4 / m_iRenderPass)) + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -((m_fY+(5 / m_iRenderPass)) + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

		_float4x4	ViewMatrix, ProjMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 3);
		m_pVIBufferCom->Render(11);
	}
	return S_OK;
}

CCookingProgressBarUI * CCookingProgressBarUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCookingProgressBarUI*	pInstance = new CCookingProgressBarUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCookingProgressBarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCookingProgressBarUI::Clone(void * pArg, _uint iCurrentScene)
{
	CCookingProgressBarUI*	pInstance = new CCookingProgressBarUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CCookingProgressBarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCookingProgressBarUI::Free()
{
	__super::Free();
	
	
	Safe_Release(m_pVIBufferBackgroundCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
