#include "stdafx.h"
#include "..\public\Tuto_ScriptUI.h"
#include "GameInstance.h"
#include <fstream>

CTuto_ScriptUI::CTuto_ScriptUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTuto_ScriptUI::CTuto_ScriptUI(const CTuto_ScriptUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTuto_ScriptUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTuto_ScriptUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	SetUp_ScriptPresenterArray(TEXT("../../Data/Tutorial/Script_SoundIndex.txt"));
	SetUp_ScriptPositionArray(TEXT("../../Data/Tutorial/Script_Position.txt"));

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	// 버튼처리
	m_isRender = false;
	return S_OK;
}

_int CTuto_ScriptUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	
	if (m_isSettingFin)
	{
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (!m_isLerpScript)
		{
			if (m_lfTimeAcc > (TimeDelta*2) && !pGameInstance->IsChannelPlaying((CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]))
			{
				m_isSettingFin = false;
				m_isRender = false;
			}
		}
		else
		{
			if (m_lfTimeAcc >  (TimeDelta * 2) && !pGameInstance->IsChannelPlaying((CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]) && m_iCurrentPresenter != m_iEndPresenter)
			{
				++m_iCurrentPresenter;
				m_fX = m_fImagePosition[m_iCurrentPresenter].x;
				m_fY = m_fImagePosition[m_iCurrentPresenter].y;
				m_fSizeX = m_fImagePosition[m_iCurrentPresenter].z;
				m_fSizeY = m_fImagePosition[m_iCurrentPresenter].w;

				
				_tchar szSoundTag[12] = TEXT("");
				if (m_iCurrentPresenter != 3 && m_iCurrentPresenter != 4)
					swprintf_s(szSoundTag, 12, TEXT("0%d.ogg"), m_iCurrentPresenter);
				else
					swprintf_s(szSoundTag, 12, TEXT("03,04.ogg"));

				pGameInstance->StopSound((CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]);
				pGameInstance->PlaySoundOnce(szSoundTag, (CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]);
			}
			else if (m_lfTimeAcc >  (TimeDelta * 2) && !pGameInstance->IsChannelPlaying((CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]) && m_iCurrentPresenter == m_iEndPresenter)
			{
				m_isLerpScript = false;
				m_isSettingFin = false;
				m_isRender = false;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
		m_lfTimeAcc += TimeDelta;
	}

	return _int();
}

_int CTuto_ScriptUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CTuto_ScriptUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

void CTuto_ScriptUI::SetUp_ScriptUI(_uint iNumber)
{
	if (!m_isSettingFin)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if((_int)iNumber > m_iCurrentPresenter-1)
			m_iCurrentPresenter = iNumber;
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->StopSound((CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]);

		m_fX = m_fImagePosition[m_iCurrentPresenter].x;
		m_fY = m_fImagePosition[m_iCurrentPresenter].y;
		m_fSizeX = m_fImagePosition[m_iCurrentPresenter].z;
		m_fSizeY = m_fImagePosition[m_iCurrentPresenter].w;

		_tchar szSoundTag[12] = TEXT("");
		if(m_iCurrentPresenter != 3 && m_iCurrentPresenter !=4)
			swprintf_s(szSoundTag, 12, TEXT("0%d.ogg"), m_iCurrentPresenter);
		else
			swprintf_s(szSoundTag, 12, TEXT("03,04.ogg"));

		pGameInstance->PlaySoundOnce(szSoundTag, (CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]);
		m_isSettingFin = true;
		m_isRender = true;

		m_lfTimeAcc = 0.0;
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CTuto_ScriptUI::SetUp_ScriptUI(_uint iStartNumber, _uint iEndNumber)
{
	if (!m_isSettingFin && !m_isLerpScript)
	{
		if ((_int)iStartNumber > m_iCurrentPresenter - 1)
			m_iCurrentPresenter = iStartNumber;
		else
			return;

		m_fX = m_fImagePosition[m_iCurrentPresenter].x;
		m_fY = m_fImagePosition[m_iCurrentPresenter].y;
		m_fSizeX = m_fImagePosition[m_iCurrentPresenter].z;
		m_fSizeY = m_fImagePosition[m_iCurrentPresenter].w;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_tchar szSoundTag[12] = TEXT("");
		if (m_iCurrentPresenter != 3 && m_iCurrentPresenter != 4)
			swprintf_s(szSoundTag, 12, TEXT("0%d.ogg"), m_iCurrentPresenter);
		else
			swprintf_s(szSoundTag, 12, TEXT("03,04.ogg"));

		m_iEndPresenter = iEndNumber;
		pGameInstance->StopSound((CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]);
		pGameInstance->PlaySoundOnce(szSoundTag, (CHANNELID)m_iScriptPresenterArr[m_iCurrentPresenter]);
		m_isSettingFin = true;
		m_isRender = true;
		m_isLerpScript = true;
		
		m_lfTimeAcc = 0.0;

		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CTuto_ScriptUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_ButtonTexture */
	if (FAILED(__super::SetUp_Component(SCENE_DEMO, TEXT("Prototype_Component_Texture_TutoScript"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Background"), (CComponent**)&m_pBackground_BufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Script"), (CComponent**)&m_pScript_BufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTuto_ScriptUI::SetUp_ConstantTable()
{
	// Background
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	_float fScriptSizeX = 762.f;
	_float fScriptSizeY = 115.f;
	
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
	
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pBackground_BufferCom, 30);
	
	m_pBackground_BufferCom->Render(11);

	// Background
	_float4x4 TextWorldMatrix;
	XMStoreFloat4x4(&TextWorldMatrix, XMMatrixIdentity());

	TextWorldMatrix._11 = fScriptSizeX;
	TextWorldMatrix._22 = fScriptSizeY;
	TextWorldMatrix._33 = 1.f;
	TextWorldMatrix._41 = (m_fX + fScriptSizeX *0.5f) - (g_iWinCX * 0.5f);
	TextWorldMatrix._42 = -(m_fY + fScriptSizeY * 0.5f) + (g_iWinCY * 0.5f);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TextWorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pScript_BufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pScript_BufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pScript_BufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pScript_BufferCom, m_iCurrentPresenter);

	m_pScript_BufferCom->Render(11);

	return S_OK;
}

HRESULT CTuto_ScriptUI::SetUp_ScriptPresenterArray(const _tchar * pFilePath)
{
	wifstream fin;
	//_tchar szBuff[260] = TEXT("");
	fin.open(pFilePath);
	_uint iCount = 0;
	if (!fin.fail())
	{
		while (true)
		{
			_uint iNum = 0;
			//m_modelPrototypeList
			fin >> iNum;
			m_iScriptPresenterArr[iCount] = iNum;
			++iCount;
			/*swprintf_s(szBuff, 260, TEXT("Num: %d\n"), iNum);
			OutputDebugStringW(szBuff);*/
			if (fin.eof())
				break;
		}
		fin.close();
	}
	return S_OK;

}

HRESULT CTuto_ScriptUI::SetUp_ScriptPositionArray(const _tchar * pFilePath)
{
	wifstream fin;
	fin.open(pFilePath);
	_uint iCount = 0;
	if (!fin.fail())
	{
		while (true)
		{
			_uint fX = 0, fY = 0, fSizeX = 0, fSizeY = 0;
			//m_modelPrototypeList
			fin >> fX >> fY >> fSizeX >> fSizeY;
			//m_iScriptPresenterArr[iCount] = iNum;
			m_fImagePosition[iCount] = _float4((_float)fX,(_float)fY,(_float)fSizeX,(_float)fSizeY);
			++iCount;
			if (fin.eof())
				break;
		}
		fin.close();
	}
	return S_OK;
}



CTuto_ScriptUI * CTuto_ScriptUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTuto_ScriptUI*	pInstance = new CTuto_ScriptUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CTuto_ScriptUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTuto_ScriptUI::Clone(void * pArg, _uint iCurrentScene)
{
	CTuto_ScriptUI*	pInstance = new CTuto_ScriptUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CTuto_ScriptUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTuto_ScriptUI::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pScript_BufferCom);
	Safe_Release(m_pBackground_BufferCom);
}

