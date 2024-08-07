#include "stdafx.h"
#include "..\public\Scene_Stage01.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "LoadingThreadUI.h"
#include "Scene_Loading.h"

CScene_Stage01::CScene_Stage01(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Stage01::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	
	pGameInstance->Lights_Clear();

	if (FAILED(Ready_Light_Desc()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(SetUp_Renderer()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(true);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);

	Ready_CameraOption();
	Ready_ShaderOption();

	Ready_Static_Object();
	Ready_Animation_Object();
	Rendy_LightPoint();

	//if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage1_Trigger.dat"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage1_TriggerTest.dat"))))
		return E_FAIL;

	// UI
	if (FAILED(Ready_Layer_UI(TEXT("Layer_Skill_UI"))))
		return E_FAIL;


	/* For.Plane */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_Plane"), SCENE_STAGE01, TEXT("Layer_Plane"))))
		return E_FAIL;

	///* For.ParkourTrigger */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_ParkourTrigger"))))
	//	return E_FAIL;
	

	//m_pCharacterController->Set_Position(_float3(47.8f, 1.f, 20.6f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(-151.f, 5.f, 107.f)), 1.f));
	//m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);

	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_CurlingIndex(0);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Delete_CurlingIndex(-1);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->SetUp_PlayerPosition(SCENE_STAGE01);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpCreate(SCENE_STAGE01);

	m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);
	m_pNextLoader->Set_NextThreadingSceneID(SCENE_STAGE01);

	/* For.Layer_ThreadLoading */
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Thread"), SCENE_STAGE01, TEXT("Layer_ThreadLoading"), (CGameObject**)&m_pLoadingUI, nullptr)))
		return E_FAIL;
	
	if (FAILED(Ready_EnvironmentEffect()))
		return E_FAIL;

	if (FAILED(Ready_QuestNpc()))
	{
		return E_FAIL;
	}

	UISIZEDESC desc;
	for (_uint i = 0; i < 5; ++i)
	{
		desc.fX = 10.f;
		desc.fY = 324.f;
		desc.fSizeX = 500.f;
		desc.fSizeY = 126.f;
		desc.iPassIndx = i;
		if (i == 4)
		{
			desc.fY = 262.f;
			desc.fSizeY = 250.f;
		}
		/* For.Prototype_GameObject_Skill_Info */
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_Info"), SCENE_STAGE01, TEXT("Layer_Skill_Info"), &desc)))
			return E_FAIL;
	}

	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//SOUND
	if (FAILED(pGameInstance->PlayBGM(TEXT("Stage1BackGroundSound.wav"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->BGMVolumeDown(0.7f);

	RELEASE_INSTANCE(CGameInstance);

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}

_int CScene_Stage01::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (nullptr != m_pNextLoader)
	{
		if (true == m_pNextLoader->Get_Finished())
		{
			m_fLoadTimeAcc += TimeDelta;
			
			if (m_fLoadTimeAcc < 1.0)
				return 0;
			
			m_fLoadTimeAcc = 0.0;
	
			if (m_pLoadingUI)
			{
				((CLoadingThreadUI*)m_pLoadingUI)->Set_IsFinished(true);
				m_pLoadingUI = nullptr;
			}

			Safe_Release(m_pNextLoader);
		}
	}
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(VK_BACK))
	{
		pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE02));
	}
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CScene_Stage01::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));
	
	return S_OK;
}

HRESULT CScene_Stage01::Ready_Light_Desc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Set_ShadowLightVector(_float3(0.161f, -0.987f, 0.004f), _float3(49.125f,43.143f,39.744f));

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.iType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.079f, -0.997f, 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage01::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_UI"), SCENE_STAGE01, pLayerTag)))
		return E_FAIL;

	ORTHTEXUTREDESC tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(ORTHTEXUTREDESC));

	tTextureDesc.iSceneType = SCENE_STATIC;
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iSize = _float2(726.f, 180.f);
	tTextureDesc.iPos = _float2(324.f, 496.f);
	tTextureDesc.iPassNum = 0;
	tTextureDesc.iSortZ = 0;

	/* For.Layer_QuestScript : Background */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_QuestScriptUI"), SCENE_STATIC, TEXT("Layer_QuestScript"), &tTextureDesc)))
		return E_FAIL;

	tTextureDesc.iSize = _float2(600.f, 150.f);
	tTextureDesc.iPos = _float2(385.f, 489.f);
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 1;
	/* For.Layer_QuestScript : Script */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_QuestScriptUI"), SCENE_STATIC, TEXT("Layer_QuestScript"), &tTextureDesc)))
		return E_FAIL;

	tTextureDesc.iSceneType = SCENE_STATIC;
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iSize = _float2(227.f, 297.f);
	tTextureDesc.iPos = _float2(307.f, 635.f);
	tTextureDesc.iPassNum = 0;
	tTextureDesc.iSortZ = 2;
	tTextureDesc.iImgNum = 0;
	/* For.Layer_QuestScript : Background */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_QuestUI"), SCENE_STATIC, TEXT("Layer_QuestUI"), &tTextureDesc)))
		return E_FAIL;

	tTextureDesc.iPos = _float2(477.f, 635.f);
	tTextureDesc.iSortZ = 1;
	tTextureDesc.iImgNum = 1;

	/* For.Layer_QuestScript : Background */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_QuestUI"), SCENE_STATIC, TEXT("Layer_QuestUI"), &tTextureDesc)))
		return E_FAIL;

	tTextureDesc.iPos = _float2(652.f, 635.f);
	tTextureDesc.iSortZ = 0;
	tTextureDesc.iImgNum = 2;

	/* For.Layer_QuestScript : Background */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_QuestUI"), SCENE_STATIC, TEXT("Layer_QuestUI"), &tTextureDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage01::Ready_Layer_EnviromentTrigger(const _tchar * pEnviromentTriggerFilePath)
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;

	hFile = CreateFile(pEnviromentTriggerFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	while (true)
	{
		_bool       bCollider = 0;
		_uint       TriggerType = 0;
		_uint       iCnt = 0;
		_float      Range = 0;
		_matrix     WorldMatrix;
		
		ReadFile(hFile, &bCollider, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &TriggerType, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &iCnt, sizeof(_uint), &dwByte, nullptr);
		if (true == bCollider)
		{
			ReadFile(hFile, &Range, sizeof(_float), &dwByte, nullptr);
		}
		ReadFile(hFile, &WorldMatrix, sizeof(_matrix), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		_float3 position;
		XMStoreFloat3(&position, WorldMatrix.r[3]);
		WorldMatrix.r[3] = XMVectorSet(position.x, position.y, position.z, TriggerType); 

		_float4x4 vRight;
		if (true == bCollider)
		{
			XMStoreFloat4x4(&vRight, WorldMatrix);
			vRight._14 = 1;
		}
		else
		{
			XMStoreFloat4x4(&vRight, WorldMatrix);
			vRight._14 = 0;
		}
		vRight._24 = Range;
		vRight._34 = (_float)iCnt;
		WorldMatrix = XMLoadFloat4x4(&vRight);
		Branch(WorldMatrix);
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage01::Branch(_matrix mWorldMatrix)
{
	_float4 indexW;
	XMStoreFloat4(&indexW, mWorldMatrix.r[3]);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/* 0 : 파쿠르 1: LOBOTO */
	switch ((_uint)indexW.w)
	{
		case 0:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_ParkourTrigger"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 1:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_LOBOTOTrigger"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 2:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_CLOSE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 3:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_CURLING"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 4:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_TUTOMSG"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 5:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_STATE0_END"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 6:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_STATE0_NPC"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 7:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_MONSTER_CREATE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 16:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE01, TEXT("Layer_SKILL_INFO"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage01::Ready_Static_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	OBJINFO	ObjectInfo;

	while (true)
	{
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR objectFullName[MAX_PATH] = L"";
		ReadFile(hFile, objectFullName, dwStrByte, &dwByte, nullptr);
		ObjectInfo.StrObjectFullName = objectFullName;

		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR objectLayerName[MAX_PATH] = L"";
		TCHAR objectLayerCatName[MAX_PATH] = L"Layer_";
		ReadFile(hFile, objectLayerName, dwStrByte, &dwByte, nullptr);
		lstrcat(objectLayerCatName, objectLayerName);
		ObjectInfo.StrLayerName = objectLayerCatName;

		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR objectModelName[MAX_PATH] = L"";
		ReadFile(hFile, objectModelName, dwStrByte, &dwByte, nullptr);
		ObjectInfo.StrModelName = objectModelName;

		ReadFile(hFile, &ObjectInfo.iNavigationIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iCurlingIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iDeleteCurlingIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iRenderPass, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iPX_Index, sizeof(_uint), &dwByte, nullptr); /* 스태틱 매쉬 이면서 상호작용 인덱스 */
		ReadFile(hFile, &ObjectInfo.fFrustumRange, sizeof(_uint), &dwByte, nullptr); /* 스태익 매쉬 절두체 컬링 */
		ReadFile(hFile, &ObjectInfo.fCollider_Center, sizeof(_float3), &dwByte, nullptr); /* 센터 */
		ReadFile(hFile, &ObjectInfo.fCollider_Extent, sizeof(_float3), &dwByte, nullptr); /* 반지름 */
		ReadFile(hFile, &ObjectInfo.WorldMatrix, sizeof(_matrix), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		TCHAR* pLayerTemp = new TCHAR[MAX_PATH];
		const _tchar* pLayerTag = nullptr;
		wstring cut1 = ObjectInfo.StrLayerName.substr(0, ObjectInfo.StrLayerName.size() - 1);
		wstring cut2 = ObjectInfo.StrLayerName.substr(cut1.size() - 1, cut1.size());

		if (isdigit(_ttoi(cut2.c_str())))
		{
			lstrcpy(pLayerTemp, cut2.c_str());
			pLayerTag = pGameInstance->Add_LayerTagToList(pLayerTemp);
		}
		else
		{
			lstrcpy(pLayerTemp, cut1.c_str());
			pLayerTag = pGameInstance->Add_LayerTagToList(pLayerTemp);

		}

		if (nullptr == pLayerTag)
		{
			/* 예시 */
			if (FAILED(Static_Object_Change_Tag(ObjectInfo, pLayerTemp)))
			{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE01, pLayerTemp, &ObjectInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
		}
		else
		{
			Safe_Delete(pLayerTemp);
			if (FAILED(Static_Object_Change_Tag(ObjectInfo, pLayerTemp)))
			{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE01, pLayerTag, &ObjectInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
		}
	}


	CloseHandle(hFile);

	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;

}

HRESULT CScene_Stage01::Ready_Animation_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage1_Animation.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	OBJINFO	ObjectInfo;

	while (true)
	{
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR objectFullName[MAX_PATH] = L"";
		ReadFile(hFile, objectFullName, dwStrByte, &dwByte, nullptr);
		ObjectInfo.StrObjectFullName = objectFullName;

		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR objectLayerName[MAX_PATH] = L"";
		TCHAR objectLayerCatName[MAX_PATH] = L"Layer_";
		ReadFile(hFile, objectLayerName, dwStrByte, &dwByte, nullptr);
		lstrcat(objectLayerCatName, objectLayerName);
		ObjectInfo.StrLayerName = objectLayerCatName;

		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR objectModelName[MAX_PATH] = L"";
		ReadFile(hFile, objectModelName, dwStrByte, &dwByte, nullptr);
		ObjectInfo.StrModelName = objectModelName;

		ReadFile(hFile, &ObjectInfo.iNavigationIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iCurlingIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iRenderPass, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.iPX_Index, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &ObjectInfo.fCollider_Center, sizeof(_float3), &dwByte, nullptr); /* 센터 */
		ReadFile(hFile, &ObjectInfo.fCollider_Extent, sizeof(_float3), &dwByte, nullptr); /* 반지름 */
		ReadFile(hFile, &ObjectInfo.WorldMatrix, sizeof(_matrix), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		TCHAR* pLayerTemp = new TCHAR[MAX_PATH];
		const _tchar* pLayerTag = nullptr;
		wstring cut1 = ObjectInfo.StrLayerName.substr(0, ObjectInfo.StrLayerName.size() - 1);
		wstring cut2 = ObjectInfo.StrLayerName.substr(cut1.size() - 1, cut1.size());

		if (isdigit(_ttoi(cut2.c_str())))
		{
			lstrcpy(pLayerTemp, cut2.c_str());
			pLayerTag = pGameInstance->Add_LayerTagToList(pLayerTemp);
		}
		else
		{
			lstrcpy(pLayerTemp, cut1.c_str());
			pLayerTag = pGameInstance->Add_LayerTagToList(pLayerTemp);

		}
		
		if (nullptr == pLayerTag)
		{
			//if (FAILED(Object_Change_Tag(ObjectInfo, pLayerTemp)))
			//{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE01, pLayerTemp, &ObjectInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			//}
		}
		else
		{
			Safe_Delete(pLayerTemp);
			//if (FAILED(Object_Change_Tag(ObjectInfo, pLayerTag)))
			//{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE01, pLayerTag, &ObjectInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			//}
		}
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage01::Rendy_LightPoint()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage1Light.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	LIGHTDESC LightDesc;
	_uint CurIndex;

	while (true)
	{
		ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

		ReadFile(hFile, &LightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &CurIndex, sizeof(_uint), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		LightDesc.iType = tagLightDesc::TYPE_POINT;
		LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

		_uint LightCount = pGameInstance->Get_Light_Size();
		if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
			return E_FAIL;

		pGameInstance->Set_Light_CurlingIndex(LightCount, CurIndex);
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage01::Ready_EnvironmentEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_EnviroParticle"),
		SCENE_DEMO, TEXT("Layer_Env_WhiteSmokeEnv"),
		SCENE_STATIC, TEXT("Effect_Env_WhiteSmokeEnv"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE02, TEXT("Layer_Env_WhiteSmokeEnv"), vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage01::Ready_QuestNpc()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix matWorld = XMMatrixIdentity();
	matWorld.r[0] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	matWorld.r[2] = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	matWorld.r[3] = XMVectorSet(62.173f, 1.677f, 63.011f, 1.f);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_NpcCoach"), SCENE_STAGE01, TEXT("Layer_QuestNpc"), &matWorld)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	matWorld.r[0] = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	matWorld.r[2] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	matWorld.r[3] = XMVectorSet(31.142f, 1.677f, 62.602f, 1.f);
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_NpcSasha"), SCENE_STAGE01, TEXT("Layer_QuestNpc"), &matWorld)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	matWorld.r[0] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	matWorld.r[2] = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	matWorld.r[3] = XMVectorSet(69.512f, 4.129, 41.894f, 1.f);
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_NpcLoboto"), SCENE_STAGE01, TEXT("Layer_QuestNpc"), &matWorld)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage01::Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (LayerType::LAYERTYPE_Not_AFFECTED_RENDER == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_Not_AFFECTED_Render"), SCENE_STAGE01, TEXT("Layer_Not_AFFECTED_Render"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;

		}
		RELEASE_INSTANCE(CGameInstance);
		
		return S_OK;
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_PsyBlExit1"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_PsyBLExit"), SCENE_STAGE01, TEXT("Layer_PsyBlExit1"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_AWaterCooler2"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_AWaterCooler"), SCENE_STAGE01, TEXT("Layer_AWaterCooler"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_FileCabinet2"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE01, TEXT("Prototype_GameObject_Stage1File_Cabinet"), SCENE_STAGE01, TEXT("Layer_Stage1File_Cabinet"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CScene_Stage01::Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag0") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag1") ||
		!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag2") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag3") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag4"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Bag"), SCENE_DEMO, LayerTag, &ObjectInfo)))
		{	
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_ANpc0") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_ANpc1") || 
			 !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_ANpc2") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_ANpc3"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Npc"), SCENE_DEMO, LayerTag, &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	return S_OK;

}

HRESULT CScene_Stage01::SetUp_Renderer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Clone_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer")));
	if (nullptr == m_pRenderer)
	{
		RELEASE_INSTANCE(CGameInstance);
		MSGBOX("Renderer nullptr");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CScene_Stage01::Ready_ShaderOption()
{
	_float BloomWeight[4] = { 0.25f,0.05f,1.f,1.f };
	m_pRenderer->Set_BloomWeight(BloomWeight);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_ShadeAmbient(0.7f);
	RELEASE_INSTANCE(CGameInstance);
}

void CScene_Stage01::Ready_CameraOption()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.7f, -0.305f, 6.f, 3.f), 1.5f);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(70.f);
	RELEASE_INSTANCE(CGameInstance);

}

CScene_Stage01 * CScene_Stage01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CScene_Stage01*	pInstance = new CScene_Stage01(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CScene_Stage01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage01::Free()
{
	__super::Free();
	//Safe_Release(m_pNavigation);
	
	//FreeConsole();
	Safe_Release(m_pRenderer);

}
