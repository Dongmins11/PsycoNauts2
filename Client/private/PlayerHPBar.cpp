#include "stdafx.h"
#include "..\public\PlayerHPBar.h"
#include "GameInstance.h"
#include "Player.h"

CPlayerHPBar::CPlayerHPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPlayerHPBar::CPlayerHPBar(const CPlayerHPBar & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayerHPBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHPBar::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	
	m_fSizeX = 70.f;
	m_fSizeY = 70.f;

	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	return S_OK;
}

_int CPlayerHPBar::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (SCENE_STAGE03 == pGameInstance->Get_CurrentSceneIndex())
		m_IsScene3 = true;

	if (SCENE_STAGE05 == pGameInstance->Get_CurrentSceneIndex())
		m_IsScene5 = true;

 	if (m_IsDecrease || m_IsIncrease)
		//m_fTimeBackUpAcc += TimeDelta;
		m_fTimeBackUpAcc += 0.016f;

	if (1.f <= m_fTimeBackUpAcc)
	{
		if (m_IsDecrease)
		{
			m_IsDecrease = false;
			m_fTimeBackUpAcc = 0.f;
		}
		else if (m_IsIncrease)
		{
			m_IsIncrease = false;
			m_IsDefault = true;
			m_fTimeBackUpAcc = 1.f;
		}
	}

	if (m_IsDefault)
		//m_fTimeDefaultAcc += TimeDelta;
		m_fTimeDefaultAcc += 0.016f;

	if (1 <= m_fTimeDefaultAcc)
	{
		m_IsDefault = false;
		m_fTimeBackUpAcc = 0.f;
		m_fTimeDefaultAcc = 0.f;
	}

	if (2 == static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Stage03PhaseNum())
		m_IsPhaseNum3 = true;

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CPlayerHPBar::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	
	return _int();
}

HRESULT CPlayerHPBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHPBar::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (m_iCurrentScene)
	{
		case SCENE_DEMO:
			m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_DEMO, TEXT("Layer_PlayerHPBar"));
			break;
		case SCENE_STAGE01:
			m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE01, TEXT("Layer_PlayerHPBar"));
			break;
		case SCENE_STAGE02:
			m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE02, TEXT("Layer_PlayerHPBar"));
			break;
		case SCENE_STAGE03:
			m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_PlayerHPBar"));
			break;
		case SCENE_STAGE04:
			m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE04, TEXT("Layer_PlayerHPBar"));
			break;
		case SCENE_STAGE05:
			m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE05, TEXT("Layer_PlayerHPBar"));
			break;
	}

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_HPBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBufferBG */
	m_szBuffer[0] = new _tchar[MAX_PATH];
	swprintf_s(m_szBuffer[0], MAX_PATH, TEXT("Com_VIBuffer_BG%d"), 0);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), m_szBuffer[0], (CComponent**)&m_pVIBufferBGCom)))
		return E_FAIL;

	/* Com_VIBufferBackUp */
	m_szBuffer[1] = new _tchar[MAX_PATH];
	swprintf_s(m_szBuffer[1], MAX_PATH, TEXT("Com_VIBuffer_Backup%d"), 0);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), m_szBuffer[1], (CComponent**)&m_pVIBufferBackUpCom)))
		return E_FAIL;

	/* Com_VIBufferDefault*/
	m_szBuffer[2] = new _tchar[MAX_PATH];
	swprintf_s(m_szBuffer[2], MAX_PATH, TEXT("Com_VIBuffer_Default%d"), 0);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), m_szBuffer[2], (CComponent**)&m_pVIBufferDefaultCom)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayerHPBar::SetUp_ConstantTable()
{
	_float4x4 WorldMatrix;

	// 검은색 체력 배경
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;

	WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f)  * m_iLayerIndex);
	WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f;
	WorldMatrix._43 = 0.5f;

	if (true == m_IsPhaseNum3 && 2 < m_iLayerIndex && true == m_IsScene3)
	{
		if (3 == m_iLayerIndex)
		{
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f;
		}
		if (4 == m_iLayerIndex)
		{
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 1);
		}
		WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f - ((m_fSizeY - 5.f) * 1);
		WorldMatrix._43 = 0.5f;
	}
	else if (3 < m_iLayerIndex && true == m_IsScene5)
	{
		if (4 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f;
		if (5 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 1);
		if (6 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 2);
		WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f - ((m_fSizeY - 5.f) * 1);
		WorldMatrix._43 = 0.5f;
	}

	_float4x4	ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pVIBufferBGCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferBGCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferBGCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferBGCom, 5);
	if (true == m_IsRenderCheck)
		m_pVIBufferBGCom->Render(11);

	// 체력이 닳는 동안 보일 이미지(노랑, 초록)
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f)  * m_iLayerIndex);
	WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f;
	WorldMatrix._43 = 0.4f;

	if (true == m_IsPhaseNum3 && 2 < m_iLayerIndex && true == m_IsScene3)
	{
		if (3 == m_iLayerIndex)
		{
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f;
		}
		if (4 == m_iLayerIndex)
		{
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 1);
		}
		WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f - ((m_fSizeY - 5.f) * 1);
		WorldMatrix._43 = 0.4f;
	}
	else if (3 < m_iLayerIndex && true == m_IsScene5)
	{
		if (4 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f;
		if (5 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 1);
		if (6 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 2);
		WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f - ((m_fSizeY - 5.f) * 1);
		WorldMatrix._43 = 0.4f;
	}

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pVIBufferBackUpCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferBackUpCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferBackUpCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pVIBufferBackUpCom->Set_RawValue("g_fTimeAcc", &m_fTimeBackUpAcc, sizeof(_float));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferBackUpCom, m_iHpBackUpIndex);
	if (true == m_IsRenderCheck)
	{
		if (m_IsDecrease)
			m_pVIBufferBackUpCom->Render(25);

		if (m_IsIncrease || m_IsDefault)
			m_pVIBufferBackUpCom->Render(31);
	}

	// 핑크색 체력 이미지
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f)  * m_iLayerIndex);
	WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f;
	WorldMatrix._43 = 0.4f;

	if (true == m_IsPhaseNum3 && 2 < m_iLayerIndex && true == m_IsScene3)
	{
		if (3 == m_iLayerIndex)
		{
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f;
		}
		if (4 == m_iLayerIndex)
		{
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 1);
		}
		WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f - ((m_fSizeY - 5.f) * 1);
		WorldMatrix._43 = 0.4f;
	}
	else if (3 < m_iLayerIndex && true == m_IsScene5)
	{
		if (4 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f;
		if (5 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 1);
		if (6 == m_iLayerIndex)
			WorldMatrix._41 = (m_fSizeX / 2.f) - (g_iWinCX * 0.5f) + 18.f + ((m_fSizeX + 3.f) * 2);
		WorldMatrix._42 = -(m_fSizeY / 2.f) + (g_iWinCY * 0.5f) - 18.f - ((m_fSizeY - 5.f) * 1);
		WorldMatrix._43 = 0.4f;
	}

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pVIBufferDefaultCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferDefaultCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferDefaultCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferDefaultCom, m_iHpDefaultIndex);
	if (true == m_IsRenderCheck)
	{
		if (m_IsDefault)
		{
			m_pVIBufferDefaultCom->Set_RawValue("g_iHpIndex", &m_iHpDefaultIndex, sizeof(_uint));
			m_pVIBufferDefaultCom->Set_RawValue("g_fTimeAcc", &m_fTimeDefaultAcc, sizeof(_float));
			m_pVIBufferDefaultCom->Render(32);
		}
		else
			m_pVIBufferDefaultCom->Render(11);
	}

	return S_OK;
}

CPlayerHPBar * CPlayerHPBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerHPBar* pInstance = new CPlayerHPBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayerHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerHPBar::Clone(void * pArg, _uint iCurrentScene)
{
	CPlayerHPBar* pInstance = new CPlayerHPBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CPlayerHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerHPBar::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferBGCom);
	Safe_Release(m_pVIBufferDefaultCom);
	Safe_Release(m_pVIBufferBackUpCom);

	for (_uint i = 0; i < 3; i++)
		Safe_Delete_Array(m_szBuffer[i]);
}
