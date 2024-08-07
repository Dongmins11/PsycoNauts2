#include "stdafx.h"
#include "..\public\Tuto_MessageUI.h"
#include "GameInstance.h"

CTuto_MessageUI::CTuto_MessageUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTuto_MessageUI::CTuto_MessageUI(const CTuto_MessageUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTuto_MessageUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTuto_MessageUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	UISIZEDESC vSizes = *(UISIZEDESC*)pArg;

	m_fX = vSizes.fX;
	m_fY = vSizes.fY;
	m_fSizeX = vSizes.fSizeX;
	m_fSizeY = vSizes.fSizeY;
	m_iRenderPass = vSizes.iPassIndx;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));

	// 버튼처리
	m_isRender = false;
	return S_OK;
}

_int CTuto_MessageUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	
	if (m_isRender)
	{
		m_lfTimeAcc += TimeDelta;
		if (m_lfTimeAcc >= m_lfTimeMax)
		{
			m_isRender = false;
			m_lfTimeAcc = 0.0;
		}
	}
	return _int();
}

_int CTuto_MessageUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CTuto_MessageUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTuto_MessageUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_ButtonTexture */
	if (FAILED(__super::SetUp_Component(SCENE_DEMO, TEXT("Prototype_Component_Texture_Message"), TEXT("Com_ButtonTexture"), (CComponent**)&m_pButtonTextureCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_RectBackground"), (CComponent**)&m_pBackground_BufferCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTuto_MessageUI::SetUp_ConstantTable()
{
	// Background
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

	_float4x4	ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pBackground_BufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pBackground_BufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pBackground_BufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	
	m_pButtonTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pBackground_BufferCom, m_iRenderPass);
	
	m_pBackground_BufferCom->Render(11);

	return S_OK;
}



CTuto_MessageUI * CTuto_MessageUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTuto_MessageUI*	pInstance = new CTuto_MessageUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CTuto_MessageUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTuto_MessageUI::Clone(void * pArg, _uint iCurrentScene)
{
	CTuto_MessageUI*	pInstance = new CTuto_MessageUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CTuto_MessageUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTuto_MessageUI::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pButtonTextureCom);
	Safe_Release(m_pBackground_BufferCom);
}
