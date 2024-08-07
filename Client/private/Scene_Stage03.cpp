#include "stdafx.h"
#include "..\public\Scene_Stage03.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "FruitSpawner.h"
#include "Fruit.h"
#include "CookingUI.h"
#include "LoadingThreadUI.h"
#include "Scene_Loading.h"
#include "Judge.h"
#include "NonBT_Ford.h"
#include "NonBT_Hollis.h"
#include "NonBT_Otto.h"
#include "Censor.h"

CScene_Stage03::CScene_Stage03(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Stage03::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Lights_Clear();

	if (FAILED(Ready_Light_Desc()))
		return E_FAIL;
	//Rendy_LightPoint();

	if (FAILED(SetUp_Renderer()))
		return E_FAIL;
	
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(true);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	pPlayer->Set_IsRender(true);

	Ready_CameraOption();
	Ready_ShaderOption();
	
	Ready_Static_Object();
	Ready_Animation_Object();
	Ready_Cloud_Create();

	/* For.PlatingPlate0 */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate0"), SCENE_STAGE03, TEXT("Layer_Plating_Plate"))))
		return E_FAIL;

	/* For.PlatingPlate1 */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate1"), SCENE_STAGE03, TEXT("Layer_Plating_Plate"))))
		return E_FAIL;
	/* For.PlatingPlate2 */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate2"), SCENE_STAGE03, TEXT("Layer_Plating_Plate"))))
		return E_FAIL;

	Setting_FruitPlatingTriggers(pPlayer->Get_Stage03PhaseNum());
	Ready_Layer_Cooking_UI();


	if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage3_TriggerSoungjun.dat"))))
		return E_FAIL;

	// UI
	if (FAILED(Ready_Layer_UI(TEXT("Layer_Skill_UI"))))
		return E_FAIL;


	/* For.Plane */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plane"), SCENE_STAGE03, TEXT("Layer_Plane"))))
		return E_FAIL;

	///* For.ParkourTrigger */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_ParkourTrigger"))))
	//	return E_FAIL;


	//m_pCharacterController->Set_Position(_float3(47.8f, 1.f, 20.6f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(-151.f, 5.f, 107.f)), 1.f));
	//m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);

	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_CurlingIndex(0);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Delete_CurlingIndex(-1);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->SetUp_PlayerPosition(SCENE_STAGE03);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpCreate(SCENE_STAGE03);

	if (pPlayer->Get_Stage03PhaseNum() == 0)
	{
		m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);
		m_pNextLoader->Set_NextThreadingSceneID(SCENE_STAGE03);

		/* For.Layer_ThreadLoading */
		if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Thread"), SCENE_STAGE03, TEXT("Layer_ThreadLoading"), (CGameObject**)&m_pLoadingUI, nullptr)))
			return E_FAIL;
	}
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = -0.979f;
	WorldMatrix._13 = 0.206f;
	WorldMatrix._31 = -0.206f;
	WorldMatrix._33 = -0.979f;
	WorldMatrix._41 = 241.f;
	WorldMatrix._42 = 2.5f;
	WorldMatrix._43 = 210.f;


	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_NonBTOtto"), SCENE_STAGE03, TEXT("Layer_Otto"), &WorldMatrix)))
		return E_FAIL;

	WorldMatrix._11 = -0.999f;
	WorldMatrix._13 = 0.036f;
	WorldMatrix._31 = -0.036f;
	WorldMatrix._33 = -0.999f;
	WorldMatrix._41 = 237.f;
	WorldMatrix._42 = 2.5f;
	WorldMatrix._43 = 210.f;
	WorldMatrix._44 = 1.f;


	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_NonBTFord"), SCENE_STAGE03, TEXT("Layer_Ford"), &WorldMatrix)))
		return E_FAIL;

	WorldMatrix._11 = -0.995f;
	WorldMatrix._13 = -0.101f;
	WorldMatrix._31 = 0.101f;
	WorldMatrix._33 = -0.995f;
	WorldMatrix._41 = 233.f;
	WorldMatrix._42 = 2.5f;
	WorldMatrix._43 = 209.f;
	WorldMatrix._44 = 1.f;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_NonBTHollis"), SCENE_STAGE03, TEXT("Layer_Hollis"), &WorldMatrix)))
		return E_FAIL;

	//pPlayer->Add_Stage03PhaseNum(); /* 확인용 */

	if (FAILED(Ready_EnvironmentEffect()))
		return E_FAIL;

	pGameInstance->BGMVolumeDown(0.7f);

	RELEASE_INSTANCE(CGameInstance);

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));


	return S_OK;
}

_int CScene_Stage03::Tick(_double TimeDelta)
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

	if (m_isMoveScene)
	{
		m_lfMoveStageTimeAcc += TimeDelta;
		if (m_lfMoveStageTimeAcc > 1.0)
		{
			pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE(m_iMoveStageNumber)));
			m_isMoveScene = false;
		}
	}
	
	if (!m_IsLoadingMonster)
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

		if (pPlayer->Get_Stage03PhaseNum() == 0)
		{
			m_IsLoadingMonster = true;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			WorldMatrix._11 = -0.806f;
			WorldMatrix._13 = 0.591f;
			WorldMatrix._31 = -0.591f;
			WorldMatrix._33 = -0.806f;
			WorldMatrix._41 = 45.f;
			WorldMatrix._42 = 4.f;
			WorldMatrix._43 = 22.750f;
			WorldMatrix._44 = 1.f;

			CJudge::JUDGEINFO tagJudgeInfo;
			ZeroMemory(&tagJudgeInfo, sizeof(CJudge::JUDGEINFO));

			tagJudgeInfo.eMeshType = CJudge::TYPE::TYPE_DEFAULT;
			tagJudgeInfo.WorldMatrix = WorldMatrix;

			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Judge"), SCENE_STAGE03, TEXT("Layer_Judge"), &tagJudgeInfo)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Far(100.f);
		}
	}
	
	if (!m_IsLoadingMonster2)
	{
		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Judge"));
		if (0 == iLayerSize)
		{
			m_IsLoadingMonster2 = true;
			CCensor::CENSORINFO tagInfo;
			ZeroMemory(&tagInfo, sizeof(CCensor::CENSORINFO));

			tagInfo.iMeshType = 1;
			XMStoreFloat4x4(&tagInfo.WorldMatrix, XMMatrixIdentity());
			_matrix WorldMatrix = XMMatrixIdentity();
			WorldMatrix.r[3] = XMVectorSet(236.f, 3.15f, 177.274f, 1.f);

			WorldMatrix.r[0] *= -1.f;
			WorldMatrix.r[2] *= -1.f;
			WorldMatrix.r[3] += XMVectorSet(0.f, 3.f, 3.f, 0.f);
			XMStoreFloat4x4(&tagInfo.WorldMatrix, WorldMatrix);
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Censor"), SCENE_STAGE03, TEXT("Layer_Censor"), &tagInfo);

			ZeroMemory(&tagInfo, sizeof(CCensor::CENSORINFO));

			tagInfo.iMeshType = 1;
			XMStoreFloat4x4(&tagInfo.WorldMatrix, XMMatrixIdentity());
			WorldMatrix = XMMatrixIdentity();
			WorldMatrix.r[3] = XMVectorSet(236.f, 3.15f, 177.274f, 1.f);
			WorldMatrix.r[3] += XMVectorSet(0.f, 3.f, -3.f, 0.f);
			XMStoreFloat4x4(&tagInfo.WorldMatrix, WorldMatrix);
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Censor"), SCENE_STAGE03, TEXT("Layer_Censor"), &tagInfo);

			ZeroMemory(&tagInfo, sizeof(CCensor::CENSORINFO));

			tagInfo.iMeshType = 1;
			XMStoreFloat4x4(&tagInfo.WorldMatrix, XMMatrixIdentity());
			WorldMatrix = XMMatrixIdentity();
			WorldMatrix.r[3] = XMVectorSet(236.f, 3.15f, 177.274f, 1.f);

			WorldMatrix.r[2] = WorldMatrix.r[0] * -1.f;
			WorldMatrix.r[0] = XMVector3Cross(WorldMatrix.r[1], WorldMatrix.r[2]);
			WorldMatrix.r[3] += XMVectorSet(3.f, 3.f, 0.f, 0.f);
			XMStoreFloat4x4(&tagInfo.WorldMatrix, WorldMatrix);
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Censor"), SCENE_STAGE03, TEXT("Layer_Censor"), &tagInfo);

			ZeroMemory(&tagInfo, sizeof(CCensor::CENSORINFO));

			tagInfo.iMeshType = 1;
			XMStoreFloat4x4(&tagInfo.WorldMatrix, XMMatrixIdentity());
			WorldMatrix = XMMatrixIdentity();
			WorldMatrix.r[3] = XMVectorSet(236.f, 3.15f, 177.274f, 1.f);

			WorldMatrix.r[2] = WorldMatrix.r[0];
			WorldMatrix.r[0] = XMVector3Cross(WorldMatrix.r[1], WorldMatrix.r[2]);
			WorldMatrix.r[3] += XMVectorSet(-3.f, 3.f, 0.f, 0.f);
			XMStoreFloat4x4(&tagInfo.WorldMatrix, WorldMatrix);
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Censor"), SCENE_STAGE03, TEXT("Layer_Censor"), &tagInfo);

		}
	}

	//if (pGameInstance->Key_Down('C'))
	//{
	//	((CNonBT_Otto*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Otto"), 0))->Set_EatMotion();
	//	((CNonBT_Ford*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Ford"), 0))->Set_EatMotion();
	//	((CNonBT_Hollis*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Hollis"), 0))->Set_EatMotion();
	//}

	if(pGameInstance->Key_Down('Z'))
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_JUDGE_DEAD);

	RELEASE_INSTANCE(CGameInstance);
	
	//FruitJumpAnimationCehck(); /* 과일 친구들 손흔들기 */

	return _int();
}

HRESULT CScene_Stage03::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}

HRESULT CScene_Stage03::Ready_Light_Desc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_ShadowLightVector(_float3(0.022f, -0.994f, 0.104f), _float3(43.664f, 24.355f, 19.136f));

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

HRESULT CScene_Stage03::Rendy_LightPoint()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage3Light.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

HRESULT CScene_Stage03::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_UI"), SCENE_STAGE03, pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage03::Ready_Layer_EnviromentTrigger(const _tchar * pEnviromentTriggerFilePath)
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

HRESULT CScene_Stage03::Ready_Layer_Cooking_UI()
{
	switch (m_iPhaseNum)
	{
	case 0:
	{
		Ready_Phase0_UI();
	}
	break;
	case 1:
	{
		Ready_Phase1_UI();
	}
	break;
	case 2:
	{
		Ready_Phase2_UI();
	}
	break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CScene_Stage03::Branch(_matrix mWorldMatrix)
{
	_float4 indexW;
	XMStoreFloat4(&indexW, mWorldMatrix.r[3]);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/* 0 : 파쿠르 1: LOBOTO */
	switch ((_uint)indexW.w)
	{
	case 0:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_ParkourTrigger"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 1:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_LOBOTOTrigger"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 2:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_CLOSE"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 3:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_CURLING"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 4:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_TUTOMSG"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 8:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_BLENDERPATH"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 9:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_CLIMBING"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 10:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_CookingTrigger"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 11:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_CLIMBINGEND"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 12:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_SLIDE"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 13:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_DEAD"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage03::Ready_Static_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE03, pLayerTemp, &ObjectInfo)))
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE03, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Stage03::Ready_Animation_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	//hFile = CreateFile(TEXT("../../Data/Stage3_Animation_DongMin.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//hFile = CreateFile(TEXT("../../Data/Stage3_Animation_HS.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	hFile = CreateFile(TEXT("../../Data/Stage3_Animation.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE03, pLayerTemp, &ObjectInfo)))
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
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE03, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Stage03::Ready_EnvironmentEffect()
{
	if (FAILED(Create_FieldSmoke(XMVectorSet(255.f, 3.f, 200.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(254.f, 2.f, 203.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(256.f, 2.5f, 201.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(252.f, 2.3f, 205.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(250.f, 2.5f, 204.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(248.f, 2.5f, 205.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(243.f, 0.8f, 207.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(241.f, 1.f, 208.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(241.f, 0.2f, 211.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(239.f, 0.4f, 213.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(237.f, 1.2f, 214.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(232.f, 1.7f, 212.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(230.f, 1.1f, 207.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(228.f, 0.f, 207.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(229.f, 1.2f, 208.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(230.f, 0.5f, 212.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(233.f, 1.7f, 211.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(220.f, -0.1f, 203.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(218.f, -0.3f, 204.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(216.f, -0.2f, 203.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(217.f, -0.8f, 202.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(217.f, -0.6f, 200.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(216.f, -0.1f, 197.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(215.f, -0.3f, 200.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(213.f, -0.6f, 201.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(212.f, -0.5f, 196.f, 0.f))))
		return E_FAIL;

	if (FAILED(Create_FieldSmoke(XMVectorSet(237.f, 0.8f, 209.f, 0.f))))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Water"), SCENE_STAGE03, TEXT("Layer_Water"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_WaterWave"), SCENE_STAGE03, TEXT("Layer_WaterWave"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage03::Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Stands1")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE03, LayerTag, &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		_uint Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_FruitSpawner"));
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_FruitSpawner"), SCENE_STAGE03, TEXT("Layer_FruitSpawner"), &ObjectInfo.WorldMatrix)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		Fruit_ObjectIndex(Count);
	}
	else if (LayerType::LAYERTYPE_BLADE == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BLADE"), SCENE_STAGE03, LayerTag, &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(),TEXT("Layer_Compt_Boiler8")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_HippoLegs"), SCENE_STAGE03, TEXT("Layer_HippoLegs"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_BlenderPath0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BlenderPath"), SCENE_STAGE03, TEXT("Layer_BlenderPath"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Blender2")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Blender2"), SCENE_STAGE03, TEXT("Layer_Blender2"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if ((!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_RailSlide0"))) || (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_RailSlide1")))
		|| (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_RailSlide2"))))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_RailSlide"), SCENE_STAGE03, LayerTag, &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if ((!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_PigChopping0"))) || (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_PigChopping1"))))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PigChopping"), SCENE_STAGE03, TEXT("Layer_Choppig"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Blender6")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Blender"), SCENE_STAGE03, TEXT("Layer_Blender6"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Griddle0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Griddle"), SCENE_STAGE03, TEXT("Layer_Griddle"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_StudioCam0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_StudioCam"), SCENE_STAGE03, TEXT("Layer_StudioCam"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Vent0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_Vent"), SCENE_STAGE03, TEXT("Layer_Compt_Vent"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Treasure0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Treasure"), SCENE_STAGE03, TEXT("Layer_Treasure"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (LayerType::LAYERTYPE_KITCHENPAN == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_KitchenPan"), SCENE_STAGE03, TEXT("Layer_KitchenPan"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (LayerType::LAYERTYPE_KITCHENPANBUTTON == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_KitchenPanButton"), SCENE_STAGE03, TEXT("Layer_KitchenPanButton"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_Boiler4")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_GasBurner"), SCENE_STAGE03, TEXT("Layer_GasBurner"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_StaticBell0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Static_Bell"), SCENE_STAGE03, TEXT("Layer_Static_Bell"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_LightBar0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_LightBar"), SCENE_STAGE03, TEXT("Layer_Compt_LightBar"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_SS_Cuff0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_SS_Cuff"), SCENE_STAGE03, TEXT("Layer_Compt_SS_Cuff"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_HostZone0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_HostZone"), SCENE_STAGE03, TEXT("Layer_Compt_HostZone"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_StationRamp0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_StationRamp0"), SCENE_STAGE03, TEXT("Layer_Compt_StationRamp0"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_StationRamp1")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_StationRamp1"), SCENE_STAGE03, TEXT("Layer_Compt_StationRamp1"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_GiantCloche0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_GiantCloche"), SCENE_STAGE03, TEXT("Layer_GiantCloche"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_BossDish0")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Stage3BossDish"), SCENE_STAGE03, TEXT("Layer_Stage3BossDish"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else if (LayerType::LAYERTYPE_FALL_CABINET == (LayerType)ObjectInfo.iPX_Index)/* 임시 방편*/
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BossWall"), SCENE_STAGE03, TEXT("Layer_BossWall"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage03::Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
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

HRESULT CScene_Stage03::Ready_Cloud_Create()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE03, TEXT("Layer_Cloud"), &XMVectorSet(222.42f, 16.87f, 146.23f, 1.f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE03, TEXT("Layer_Cloud"), &XMVectorSet(239.22f, 14.07f, 151.57f, 1.f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE03, TEXT("Layer_Cloud"), &XMVectorSet(262.61f, 14.94f, 156.46f, 1.f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE03, TEXT("Layer_Cloud"), &XMVectorSet(283.22f, 20.60f, 173.82f, 1.f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE03, TEXT("Layer_Cloud"), &XMVectorSet(262.67f, 17.48f, 180.95f, 1.f))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CScene_Stage03::Ready_Phase0_UI()
{
	ORTHTEXUTREDESC tTextureDesc;
	// Background_Phase0
	tTextureDesc.iSceneType = (_uint)SCENE_STAGE03;
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Background");
	tTextureDesc.iPos = { 484.f,0.f };
	tTextureDesc.iSize = { 312.f,183.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_Background"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	////////////////////////////
	
	// Layer_CookingUI_OnPlate : EGG
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 486.f,0.f };
	tTextureDesc.iSize = { 93.f,184.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 1;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Layer_CookingUI_OnPlate : ONION
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 644.f,0.f };
	tTextureDesc.iSize = { 93.f,186.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 2;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	////////////////////////////////////////////////////////

	// Egg_Image
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 490.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_EGG;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 2;


	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 649.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_ONION;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_double TimeAcc = 300.f;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_TimerUI"), SCENE_STAGE03, TEXT("Layer_TimerUI"), &TimeAcc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	// Test_ProgressBar : 0번 Egg:Boil
	tTextureDesc.iPos = { 595.f,130.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{ 
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar2 : 1번 Onion:Cut
	tTextureDesc.iPos = { 747.f,130.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	RELEASE_INSTANCE(CGameInstance);

}

void CScene_Stage03::Ready_Phase1_UI()
{
	ORTHTEXUTREDESC tTextureDesc;
	// Background_Phase0
	tTextureDesc.iSceneType = (_uint)SCENE_STAGE03;
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Background");
	tTextureDesc.iPos = { 460.f,0.f };
	tTextureDesc.iSize = { 364.f,183.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_Background"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	////////////////////////////

	// Layer_CookingUI_OnPlate : EGG
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 457.f,0.f };
	tTextureDesc.iSize = { 75.f,184.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 1;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Layer_CookingUI_OnPlate : Bread
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 584.f,0.f };
	tTextureDesc.iSize = { 71.f,183.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Layer_CookingUI_OnPlate : StrawBerry
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 704.f,0.f };
	tTextureDesc.iSize = { 71.f,185.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 2;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	////////////////////////////////////////////////////////

	// Egg_Image
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 452.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_EGG;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 2;


	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 585.f,62.f };
	tTextureDesc.iSize = { 72.f,72.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_BREAD;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 698.f,56.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_STRAWBERRY;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_double TimeAcc = 600.f;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_TimerUI"), SCENE_STAGE03, TEXT("Layer_TimerUI"), &TimeAcc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar : 0번 Egg:Boil
	tTextureDesc.iPos = { 543.f,130.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar2 : 1번 Bread:Cut
	tTextureDesc.iPos = { 680.f,44.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 2;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar : 2번 StrawBerry:Blend
	tTextureDesc.iPos = { 783.f,130.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar : 3번 Bread:Bake
	tTextureDesc.iPos = { 680.f,156.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 2;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	RELEASE_INSTANCE(CGameInstance);

}

void CScene_Stage03::Ready_Phase2_UI()
{
	ORTHTEXUTREDESC tTextureDesc;
	// Background_Phase0
	tTextureDesc.iSceneType = (_uint)SCENE_STAGE03;
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Background");
	tTextureDesc.iPos = { 276.f,0.f };
	tTextureDesc.iSize = { 732.f,183.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = 2;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_Background"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	////////////////////////////

	// Layer_CookingUI_OnPlate : EGG
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 276.f,0.f };
	tTextureDesc.iSize = { 112.f,183.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 1;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Layer_CookingUI_OnPlate : Pig
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 463.f,0.f };
	tTextureDesc.iSize = { 109.f,184.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	// Layer_CookingUI_OnPlate : Bee
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 644.f,0.f };
	tTextureDesc.iSize = { 109.f,184.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Layer_CookingUI_OnPlate : WaterMelon
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_OnPlate");
	tTextureDesc.iPos = { 827,1.f };
	tTextureDesc.iSize = { 106.f,183.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 2;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	////////////////////////////////////////////////////////

	// Egg_Image
	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 288.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_EGG;
	tTextureDesc.iPassNum = 11;
	tTextureDesc.iSortZ = 2;


	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 476.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_PORK;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 658.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_BEEHIVE;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	tTextureDesc.szTag = TEXT("Prototype_Component_Texture_CookingUI_Fruit");
	tTextureDesc.iPos = { 839.f,50.f };
	tTextureDesc.iSize = { 84.f,84.f };
	tTextureDesc.bRenderInit = true;
	tTextureDesc.iImgNum = (_uint)FruitType::FRUIT_WATERMELON;
	tTextureDesc.iPassNum = 11;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_double TimeAcc = 900.f;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_TimerUI"), SCENE_STAGE03, TEXT("Layer_TimerUI"), &TimeAcc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	// Test_ProgressBar : 0번 Egg:Bake
	tTextureDesc.iPos = { 405.f,130.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar2 : 1번 Pork:Cut
	tTextureDesc.iPos = { 611.f,45.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 2;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar : 2번 Bee:Boil
	tTextureDesc.iPos = { 774.f,130.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 1;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// Test_ProgressBar : 3번 WaterMelon:Cut
	tTextureDesc.iPos = { 977.f,45.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 1;
	tTextureDesc.iPassNum = 2;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	// Test_ProgressBar : 4번 Pork:Bake
	tTextureDesc.iPos = { 611.f,154.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 2;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	// Test_ProgressBar : 5번 WaterMelon:Blend
	tTextureDesc.iPos = { 977.f,154.f };
	tTextureDesc.iSize = { 33.f,33.f };
	tTextureDesc.bRenderInit = false;
	tTextureDesc.iImgNum = 0;
	tTextureDesc.iPassNum = 2;
	tTextureDesc.iSortZ = 2;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), &tTextureDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	RELEASE_INSTANCE(CGameInstance);
}


HRESULT CScene_Stage03::SetUp_Renderer()
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

void CScene_Stage03::Ready_ShaderOption()
{
	_float fWeight[4] = { 0.4f,0.1f,10.f,0.f };

	m_pRenderer->Set_BloomWeight(fWeight);
	//m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_SHADOW, false);
	m_pRenderer->Set_ToneOption(1);
	m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_FOG, true);
	m_pRenderer->Set_BlurOption(1);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_ShadeAmbient(0.3f);
	pGameInstance->Set_IsLightCuriling(false);
	RELEASE_INSTANCE(CGameInstance);

}

void CScene_Stage03::Ready_CameraOption()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.7f, -0.7f, 5.84f, 2.045f), 1.74f);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(90.f);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Far(200.f);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CScene_Stage03::Create_FieldSmoke(_vector vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_Environment"),
		SCENE_STAGE03, TEXT("Layer_FieldSmoke"),
		SCENE_STATIC, TEXT("Effect_FieldSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE03, TEXT("Layer_FieldSmoke"), vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//pGameInstance->Setting_Effect_Speed(SCENE_STAGE03, TEXT("Layer_FieldSmoke"), TEXT("Com_Transform"), 3.0);
	//pGameInstance->Setting_Effect_UVSprite(SCENE_STAGE03, TEXT("Layer_FieldSmoke"), TEXT("Com_Transform"), 0.2);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void* CScene_Stage03::Get_SceneData()
{
	return &m_iPhaseNum;
}

void CScene_Stage03::Setting_StageUpdate(_uint iSceneNum)
{
	m_iMoveStageNumber = iSceneNum;
	m_isMoveScene = true;
}

void CScene_Stage03::FruitJumpAnimationCehck()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint SpwnerSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_FruitSpawner"));

	for (_uint i = 0; i < SpwnerSize; ++i)
	{
		CFruit* pFruit = nullptr;
		pFruit = static_cast<CFruitSpawner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), i))->Get_FruitObject();
		if (pFruit)
			pFruit->Get_Camera_Distance();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CScene_Stage03::Setting_FruitPlatingTriggers(_uint iNum)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < 3; ++i)
	{
		if (i == iNum)
			pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), i)->Set_IsRender(true);
		else
			pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), i)->Set_IsRender(false);

	}

	_uint TriggerCntArr[4] = { 0,2,5,9 };
	_uint iCnt = 0;
	for (_uint i = 0; i < TriggerCntArr[3]; ++i)
	{
		if (i >= TriggerCntArr[iNum] && i < TriggerCntArr[iNum + 1])
		{
			pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_PlateTrigger"), i)->Set_IsRender(true);	
			pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_PlateTrigger"), i)->Set_RenderPass(iCnt);
			++iCnt;
		}
		else
			pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_PlateTrigger"), i)->Set_IsRender(false);

	}
	m_iPhaseNum = iNum;


	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CScene_Stage03::Fruit_ObjectIndex(_uint LayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FruitType eType = FruitType::FRUIT_END;
	switch (LayerIndex)
	{
	case 0:
	case 2:
	case 5:
		eType = FruitType::FRUIT_ONION;
		//eType = FruitType::FRUIT_EGG;
		break;
	case 1:
	case 12:
		///eType = FruitType::FRUIT_EGG;
		eType = FruitType::FRUIT_WATERMELON;
		break;
	case 3:
	case 7:
	case 8:
	case 11:
		///eType = FruitType::FRUIT_EGG;
		eType = FruitType::FRUIT_STRAWBERRY;
		break;
	case 4:
		eType = FruitType::FRUIT_BREAD;
		break;
	case 6:
	case 10:
	case 14:
	case 15:
		eType = FruitType::FRUIT_EGG;
		break;
	case 9:
		eType = FruitType::FRUIT_PORK;
		//eType = FruitType::FRUIT_EGG;
		break;
	case 13:
		eType = FruitType::FRUIT_BEEHIVE;
		break;
	}
	if (eType != FruitType::FRUIT_END)
	{
		static_cast<CFruitSpawner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), LayerIndex))->Set_FruitIndex(eType);
		static_cast<CFruitSpawner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), LayerIndex))->Create_SpwnerIndex(LayerIndex);
		static_cast<CFruitSpawner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), LayerIndex))->Create_Fruit(eType);

	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CScene_Stage03 * CScene_Stage03::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Stage03*	pInstance = new CScene_Stage03(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created Scene_Stage3");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage03::Free()
{
	__super::Free();
	//Safe_Release(m_pNextLoader);
	Safe_Release(m_pRenderer);
}


