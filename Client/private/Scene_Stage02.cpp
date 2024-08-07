#include "stdafx.h"
#include "..\public\Scene_Stage02.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "LoadingThreadUI.h"
#include "Scene_Loading.h"
#include "FadeInOut.h"

CScene_Stage02::CScene_Stage02(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Stage02::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Lights_Clear();

	dynamic_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);

	if (FAILED(Ready_Light_Desc()))
		return E_FAIL;

	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(true);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.7f, -0.305f, 6.f, 3.f), 1.5f);

	Ready_Static_Object();
	//Ready_Animation_Object();
	Rendy_LightPoint();
	if (FAILED(Ready_Shader_Value()))
		return E_FAIL;


	//if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage2_Trigger.dat"))))
	//	return E_FAIL;
	if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage2_Trigger_Test.dat"))))
		return E_FAIL;

	// UI
	if (FAILED(Ready_Layer_UI(TEXT("Layer_Skill_UI"))))
		return E_FAIL;

	/* For.Plane */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Plane"), SCENE_STAGE02, TEXT("Layer_Plane"))))
		return E_FAIL;


	/* For.Layer_Stage02DoorButtons */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Stage02DoorButtons"), SCENE_STAGE02, TEXT("Layer_Stage02DoorButtons"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_HaroldMouse"), SCENE_STAGE02, TEXT("Layer_HaroldMouse"))))
		return E_FAIL;

	/* For.Layer_Player_CutScene */
	_uint Arg = SCENE_STAGE02;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Player_CutScene"), SCENE_STAGE02, TEXT("Layer_Player_CutScene"),&Arg)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Player_2D"), SCENE_STAGE02, TEXT("Layer_Player_2D"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Lock_2D"), SCENE_STAGE02, TEXT("Layer_Lock_2D"))))
		return E_FAIL;
	
	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Monster_2D"), SCENE_STAGE02, TEXT("Layer_Monster_2D"))))
			return E_FAIL;
	}
	
	for (_uint i = 0; i < 3; i++)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Key_2D"), SCENE_STAGE02, TEXT("Layer_Key_2D"))))
			return E_FAIL;
	}

	///* For.ParkourTrigger */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_ParkourTrigger"))))
	//	return E_FAIL;
	
	if (FAILED(Ready_EnvironmentEffect()))
		return E_FAIL;

	//m_pCharacterController->Set_Position(_float3(47.8f, 1.f, 20.6f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(-151.f, 5.f, 107.f)), 1.f));
	//m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);

	UISIZEDESC desc;
	for (_uint i = 0; i < 2; ++i)
	{
		desc.fX = 10.f;
		desc.fY = 324.f;
		desc.fSizeX = 500.f;
		desc.fSizeY = 126.f;
		desc.iPassIndx = i + 5;
		if (i == 0)
		{
			desc.fY = 262.f;
			desc.fSizeY = 250.f;
		}
		/* For.Prototype_GameObject_Skill_Info */
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_Info"), SCENE_STAGE02, TEXT("Layer_Skill_Info"), &desc)))
			return E_FAIL;
	}

	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_CurlingIndex(0);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Delete_CurlingIndex(-1);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->SetUp_PlayerPosition(SCENE_STAGE02);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpCreate(SCENE_STAGE02);


	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//SOUND
	if (FAILED(pGameInstance->PlayBGM(TEXT("Stage2BackGroundSound.wav"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->BGMVolumeDown(0.7f);

	m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);
	m_pNextLoader->Set_NextThreadingSceneID(SCENE_STAGE02);

	/* For.Layer_ThreadLoading */
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Thread"), SCENE_STAGE02, TEXT("Layer_ThreadLoading"), (CGameObject**)&m_pLoadingUI, nullptr)))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);



	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));


	return S_OK;
}

_int CScene_Stage02::Tick(_double TimeDelta)
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
		if (FAILED(pGameInstance->StopAll()))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE03));
	}
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CScene_Stage02::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}

HRESULT CScene_Stage02::Ready_Light_Desc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_ShadowLightVector(_float3(0.f,-1.f,0.081f), _float3(-2.577f,33.35f,35.723f));

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.iType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.079f, -0.997f, 0.0f,0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage02::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_UI"), SCENE_STAGE02, pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage02::Ready_Layer_EnviromentTrigger(const _tchar * pEnviromentTriggerFilePath)
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

HRESULT CScene_Stage02::Branch(_matrix mWorldMatrix)
{
	_float4 indexW;
	XMStoreFloat4(&indexW, mWorldMatrix.r[3]);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/* 0 : 파쿠르 1: LOBOTO */
	switch ((_uint)indexW.w)
	{
		case 0:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_ParkourTrigger"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 1:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_LOBOTOTrigger"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 2:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_CLOSE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 3:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_CURLING"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 4:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_TUTOMSG"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 5:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_STATE0_END"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 6:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_STATE0_NPC"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 7:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_MONSTER_CREATE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 14:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_MOUSECUTSCENE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 15:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_2DMAP"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 16:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE02, TEXT("Layer_SKILL_INFO"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage02::Ready_Static_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage2_Test.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//hFile = CreateFile(TEXT("../../Data/Stage2.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
		// 오브젝트의 사이즈가 10을 넘어가지않을때
		wstring cut1 = ObjectInfo.StrLayerName.substr(0, ObjectInfo.StrLayerName.size() - 1);
		// cut2가 숫자일경우 넘어감
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE02, pLayerTemp, &ObjectInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
		}
		else
		{
			Safe_Delete(pLayerTemp);
			if (FAILED(Static_Object_Change_Tag(ObjectInfo, pLayerTag)))
			{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE02, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Stage02::Ready_Animation_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage2_Animation.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE02, pLayerTemp, &ObjectInfo)))
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE02, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Stage02::Rendy_LightPoint()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage2Light.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

HRESULT CScene_Stage02::Ready_Shader_Value()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
	if (nullptr == pRenderer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	_float ArrWeight[4];

	ArrWeight[0] = 0.4f;
	ArrWeight[1] = 0.1f;
	ArrWeight[2] = 1.f;
	ArrWeight[3] = 1.f;
	pRenderer->Set_BloomWeight(ArrWeight);
	pGameInstance->Set_ShadeAmbient(0.4f);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage02::Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_PyroPainting1") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_PyroPainting7"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_PyroPainting"), SCENE_STAGE02, TEXT("Layer_PyroPainting1"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;

		}
		RELEASE_INSTANCE(CGameInstance);
		
		return S_OK;
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Door2"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Door2"), SCENE_STAGE02, TEXT("Layer_VentDoor"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	/*else if (LayerType::LAYERTYPE_ANIMATED_SHELV == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_AnimatedShelv"), SCENE_DEMO, TEXT("Layer_AnimatedShelv"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}*/
	else if (LayerType::LAYERTYPE_2DMAP == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Plane_2D"), SCENE_STAGE02, TEXT("Layer_Plane_2D"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Lock0") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Chain0"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE02, TEXT("Prototype_GameObject_Lock_Object"), SCENE_STAGE02, TEXT("Layer_Lock_Object"), &ObjectInfo)))
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
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage02::Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag)
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

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;

}

HRESULT CScene_Stage02::Ready_EnvironmentEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_EnviroParticle"),
		SCENE_STAGE02, TEXT("Layer_Env_VentSmoke1"),
		SCENE_STATIC, TEXT("Effect_Env_VentSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	
	_vector vPos = XMVectorSet(-2.f, 4.f, 64.f, 1.f);
	if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE02, TEXT("Layer_Env_VentSmoke1"), vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_EnviroParticle"),
		SCENE_STAGE02, TEXT("Layer_Env_VentSmoke2"),
		SCENE_STATIC, TEXT("Effect_Env_VentSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	vPos = XMVectorSet(-12.f, 4.f, 58.f, 1.f);
	if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE02, TEXT("Layer_Env_VentSmoke2"), vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
		SCENE_STAGE02, TEXT("Layer_Repeat_VentPowerSmokeZ"),
		SCENE_STATIC, TEXT("Effect_Repeat_VentPowerSmokeZ"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	vPos = XMVectorSet(6.f, 3.5f, 72.5f, 1.f);
	if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE02, TEXT("Layer_Repeat_VentPowerSmokeZ"), vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_EnviroParticle"),
		SCENE_DEMO, TEXT("Layer_Env_WhiteSmokeEnv"),
		SCENE_STATIC, TEXT("Effect_Env_WhiteSmokeEnv"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE02, TEXT("Layer_Env_WhiteSmokeEnv"), vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CScene_Stage02 * CScene_Stage02::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CScene_Stage02*	pInstance = new CScene_Stage02(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CScene_Stage02");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage02::Free()
{
	__super::Free();
	//Safe_Release(m_pNavigation);
	
	//FreeConsole();
}
