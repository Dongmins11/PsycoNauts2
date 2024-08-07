#include "stdafx.h"
#include "..\public\Scene_Loading.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Scene_Logo.h"
#include "Scene_Demo.h"
#include "Scene_Stage01.h"
#include "Scene_Stage02.h"
#include "Scene_Stage03.h"
#include "Scene_Stage04.h"
#include "Scene_Stage05.h"
#include "Scene_Ending.h"
#include "Loading_UI.h"

CScene_Loading::CScene_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Loading::NativeConstruct(SCENE eNextScene)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_eNextScene = eNextScene;

	m_pLoader = CLoader::Create(m_pDevice,m_pDeviceContext, eNextScene);
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(Ready_Layer_Loading_UI(TEXT("Layer_Loading_UI"))))
		return E_FAIL;



	//if (FAILED(Ready_Layer_Loading_Player(TEXT("Layer_Loading_Player"))))
		//return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_SkyBox"), 0)->Set_IsRender(true);

	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));
	return S_OK;
}

_int CScene_Loading::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (true == m_pLoader->Get_Finished())
	{
		m_TimeAcc += TimeDelta;

		if (m_TimeAcc < 1.0)
			return 0;

		m_TimeAcc = 0.0;

		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CLoading_UI* pUI = (CLoading_UI*)pGameInstance->Get_GameObject(SCENE_LOADING, TEXT("Layer_Loading_UI"), 0);
		if (nullptr == pUI)
		{
			Safe_Release(pGameInstance);
			return -1;
		}

		pUI->Set_IsLoad(true);

		CScene*		pScene = nullptr;

		switch (m_eNextScene)
		{
		case SCENE_LOGO:
			pScene = CScene_Logo::Create(m_pDevice,m_pDeviceContext);
			break;
		case SCENE_DEMO:
			pScene = CScene_Demo::Create(m_pDevice, m_pDeviceContext);
			break;
		case SCENE_STAGE01:
			pScene = CScene_Stage01::Create(m_pDevice, m_pDeviceContext);
			break;
		case SCENE_STAGE02:
			pScene = CScene_Stage02::Create(m_pDevice, m_pDeviceContext);
			break;
		case SCENE_STAGE03:
			pScene = CScene_Stage03::Create(m_pDevice, m_pDeviceContext);
			break;
		case SCENE_STAGE04:
			pScene = CScene_Stage04::Create(m_pDevice, m_pDeviceContext);
			break;
		case SCENE_STAGE05:
			pScene = CScene_Stage05::Create(m_pDevice, m_pDeviceContext);
			break;
		case SCENE_ENDING:
			pScene = CScene_Ending::Create(m_pDevice, m_pDeviceContext);
			break;
		}

		if (nullptr == pScene)
		{
			Safe_Release(pGameInstance);
			return 0;
		}

		if (FAILED(pGameInstance->Open_Scene(m_eNextScene, pScene)))
		{
			Safe_Release(pGameInstance);
			return 0;
		}

		Safe_Release(pGameInstance);
	}

	return _int();
}

HRESULT CScene_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pLoader->Draw_LoadingText();

	return S_OK;
}

HRESULT CScene_Loading::Ready_Layer_Loading_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_UI"), SCENE_LOADING, pLayerTag);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Loading::Ready_Layer_Loading_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Player"), SCENE_LOADING, pLayerTag);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CScene_Loading * CScene_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SCENE eNextScene)
{
	CScene_Loading*	pInstance = new CScene_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextScene)))
	{
		MSGBOX("Failed to Created CScene_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

