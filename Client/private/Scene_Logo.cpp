#include "stdafx.h"
#include "..\public\Scene_Logo.h"

#include "GameInstance.h"
#include "Scene_Loading.h"
#include "Camera_Fly.h"
#include "Navigation.h"

CScene_Logo::CScene_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(false);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(false);
	RELEASE_INSTANCE(CGameInstance);


	/*if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->PlayBGM(TEXT("Logo.wav"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->BGMVolumeDown(0.8f);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

_int CScene_Logo::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
		
		if (FAILED(pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_DEMO))))
			return E_FAIL;
		//if (FAILED(pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE03))))
		//	return E_FAIL;
		//if (FAILED(pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE05))))
			//return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
	}

	return _int();
}

HRESULT CScene_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	///* For.Camera_Fly*/
	//CCamera::CAMEREADESC			CameraDesc;
	//ZeroMemory(&CameraDesc, sizeof(CCamera::CAMEREADESC));
	//CameraDesc.vEye = _float3(0.f, 10.f, -8.f);
	//CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	//CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	//CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	//CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	//
	//
	//if (FAILED(pGameInstance->Add_ObjectToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Fly"), LEVEL_LOGO, pLayerTag, &CameraDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	
	 /* For.BackGround */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_LOGO, TEXT("Prototype_GameObject_Background"), SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CScene_Logo * CScene_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CScene_Logo*	pInstance = new CScene_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CScene_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	__super::Free();


}
