#include "stdafx.h"
#include "..\public\QuestScriptUI.h"
#include "GameInstance.h"

CQuestScriptUI::CQuestScriptUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CQuestScriptUI::CQuestScriptUI(const CQuestScriptUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CQuestScriptUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestScriptUI::NativeConstruct(void * pArg, _uint iCurrentScene)
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
	//m_iCurrentImgIndex = tTextureDesc.iImgNum;
	m_iRenderPass = tTextureDesc.iPassNum;
	m_fViewZ = tTextureDesc.iSortZ;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	m_isRender = false;
	//m_iRenderPass = 1;
	return S_OK;
}

_int CQuestScriptUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	return _int();
}

_int CQuestScriptUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CQuestScriptUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestScriptUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Quest_Script"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CQuestScriptUI::SetUp_ConstantTable()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	
	_float4x4	ViewMatrix, ProjMatrix;

	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, m_iRenderPass);

	m_pVIBufferCom->Render(11);

	return S_OK;
}

CQuestScriptUI * CQuestScriptUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CQuestScriptUI*	pInstance = new CQuestScriptUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CQuestScriptUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CQuestScriptUI::Clone(void * pArg, _uint iCurrentScene)
{
	CQuestScriptUI*	pInstance = new CQuestScriptUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CQuestScriptUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuestScriptUI::Free()
{
	__super::Free();
	
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
