#include "stdafx.h"
#include "..\public\TimerUI.h"
#include "GameInstance.h"
#include "Scene.h"
CTimerUI::CTimerUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTimerUI::CTimerUI(const CTimerUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTimerUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTimerUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_fSizeX = 40.f;
	m_fSizeY = 71.f;
	m_fX = 510.f;
	m_fY = 182.f;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_lfTimeAcc = *(_double*)pArg;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	if (m_lfTimeAcc <= 300.0)
		m_isRender = false;
	return S_OK;
}

_int CTimerUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	
	return _int();
}

_int CTimerUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (m_lfTimeAcc > 0.0)
	{
		m_lfTimeAcc -= TimeDelta;
		if (m_lfTimeAcc <= 0.0)
			m_lfTimeAcc = 0.0;

		Compute_TimerNumbers();
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CTimerUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

void CTimerUI::Compute_TimerNumbers()
{
	_int Min = (_int)m_lfTimeAcc / 60;
	Compute_DisitNumbers(Min, &m_iTimeNumbers[0], &m_iTimeNumbers[1]);

	_int Sec = (_int)m_lfTimeAcc - (Min*60);
	Compute_DisitNumbers(Sec, &m_iTimeNumbers[2], &m_iTimeNumbers[3]);

	_float Data = m_lfTimeAcc - (Min*60) - Sec;
	_int Rest = _int(Data * 100.f);
	Compute_DisitNumbers(Rest, &m_iTimeNumbers[4], &m_iTimeNumbers[5]);

}

void CTimerUI::Compute_DisitNumbers(_int Number, _uint * Tenth, _uint * Ones)
{
	if (Number < 10)
	{
		*Tenth = 0;
		*Ones = Number;
	}
	else
	{
		*Tenth = Number/10;
		*Ones = Number%10;
	}
}



HRESULT CTimerUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STAGE03, TEXT("Prototype_Component_Texture_Stage03Timer"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	for (_uint i = 0; i < 8; ++i)
	{
		/* Com_VIBuffer */
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), m_szVIBufferTags[i], (CComponent**)&m_pVIBufferCom[i])))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CTimerUI::SetUp_ConstantTable()
{
	_float4x4 WorldMatrix;
	


	_float4x4	ViewMatrix, ProjMatrix;
	for (_uint i = 0; i < 6; ++i)
	{

		m_fAddX = (40.f*i);
		if (i != 0 && i % 2 == 0)
			m_fAddX += 10 * (i / 2);
		else if (i / 2 > 0)
			m_fAddX += 10 * _uint(i / 2);

		m_fX = 510.f + m_fAddX;

		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = m_fSizeX;
		WorldMatrix._22 = m_fSizeY;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pVIBufferCom[i]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferCom[i]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pVIBufferCom[i]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom[i],m_iTimeNumbers[i]);

		m_pVIBufferCom[i]->Render(11);
	}

	m_fX = 574.f;
	m_fY = 176.f;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pVIBufferCom[6]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom[6]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom[6]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom[6], 10);

	m_pVIBufferCom[6]->Render(11);

	m_fX = 665.f;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pVIBufferCom[7]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom[7]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom[7]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom[7], 10);

	m_pVIBufferCom[7]->Render(11);

	return S_OK;
}

CTimerUI * CTimerUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTimerUI*	pInstance = new CTimerUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CTimerUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTimerUI::Clone(void * pArg, _uint iCurrentScene)
{
	CTimerUI*	pInstance = new CTimerUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CTimerUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimerUI::Free()
{
	__super::Free();
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	for(_uint i=0;i<8;++i)
		Safe_Release(m_pVIBufferCom[i]);
}
