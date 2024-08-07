#include "stdafx.h"
#include "..\public\CookingUI.h"
#include "GameInstance.h"

CCookingUI::CCookingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCookingUI::CCookingUI(const CCookingUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CCookingUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookingUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	ORTHTEXUTREDESC tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(ORTHTEXUTREDESC));

	memcpy(&tTextureDesc, pArg, sizeof(ORTHTEXUTREDESC));

	if (FAILED(SetUp_Component(tTextureDesc.iSceneType, tTextureDesc.szTag)))
		return E_FAIL;

	m_fSizeX = (_float)tTextureDesc.iSize.x;
	m_fSizeY = (_float)tTextureDesc.iSize.y;

	m_fX = (_float)tTextureDesc.iPos.x;
	m_fY = (_float)tTextureDesc.iPos.y;
	m_iCurrentImgIndex = tTextureDesc.iImgNum;
	m_iRenderPass = tTextureDesc.iPassNum;
	m_fViewZ = tTextureDesc.iSortZ;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	m_isRender = tTextureDesc.bRenderInit;
	return S_OK;
}

_int CCookingUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	return _int();
}

_int CCookingUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CCookingUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookingUI::SetUp_Component(_uint iSceneNum, const _tchar* TexComTag)
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(iSceneNum, TexComTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookingUI::SetUp_ConstantTable()
{
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

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, m_iCurrentImgIndex);

	m_pVIBufferCom->Render(m_iRenderPass);
	
	return S_OK;
}

CCookingUI * CCookingUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCookingUI*	pInstance = new CCookingUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCookingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCookingUI::Clone(void * pArg, _uint iCurrentScene)
{
	CCookingUI*	pInstance = new CCookingUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CCookingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCookingUI::Free()
{
	__super::Free();
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
