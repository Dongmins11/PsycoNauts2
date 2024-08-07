#include "stdafx.h"
#include "..\public\Scene_Stage05.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "Scene_Loading.h"
#include "FadeInOut.h"

CScene_Stage05::CScene_Stage05(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Stage05::NativeConstruct()
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

	Rendy_LightPoint();


	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(true);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSINTRO);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.8f, 0.1f, 7.4f, 4.5f), 1.5f);

	Ready_Static_Object();
	//Ready_Animation_Object();
	Ready_ShaderOption();

	///* For.PlatingPlate0 */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate0"), SCENE_STAGE03, TEXT("Layer_Plating_Plate"))))
	//	return E_FAIL;
	//
	///* For.PlatingPlate1 */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate1"), SCENE_STAGE03, TEXT("Layer_Plating_Plate"))))
	//	return E_FAIL;
	///* For.PlatingPlate2 */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate2"), SCENE_STAGE03, TEXT("Layer_Plating_Plate"))))
	//	return E_FAIL;
	//
	//Setting_FruitPlatingTriggers(0);
	//Ready_Layer_Cooking_UI();
	//
	//if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage3_Trigger.dat"))))
	//	return E_FAIL;

	// UI
	if (FAILED(Ready_Layer_UI(TEXT("Layer_Skill_UI"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Plane */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE05, TEXT("Prototype_GameObject_Plane"), SCENE_STAGE05, TEXT("Layer_Plane"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	///* For.ParkourTrigger */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_ParkourTrigger"))))
	//	return E_FAIL;


	//m_pCharacterController->Set_Position(_float3(47.8f, 1.f, 20.6f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(-151.f, 5.f, 107.f)), 1.f));
	//m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);

	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_CurlingIndex(0);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Delete_CurlingIndex(-1);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->SetUp_PlayerPosition(SCENE_STAGE05);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpCreate(SCENE_STAGE05);

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = -1.f;
	WorldMatrix._33 = -1.f;
	WorldMatrix._41 = 40.f;
	WorldMatrix._42 = -50.f;
	WorldMatrix._43 = 70.f;
	WorldMatrix._44 = 1.f;

	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_BossGoat"), SCENE::SCENE_STAGE05, TEXT("Layer_FinalBoss"), &WorldMatrix);

	((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);

	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->BGMVolumeDown(0.8f);

	RELEASE_INSTANCE(CGameInstance);

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));


	return S_OK;
}

_int CScene_Stage05::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	return _int();
}

HRESULT CScene_Stage05::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}

void * CScene_Stage05::Get_SceneData()
{
	return &m_iPhaseNum;
}

HRESULT CScene_Stage05::Ready_Light_Desc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

HRESULT CScene_Stage05::Rendy_LightPoint()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage5Light.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		pGameInstance->Set_Light_CurlingIndex(LightCount, CurIndex);
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Stage05::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_UI"), SCENE_STAGE05, pLayerTag)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage05::Ready_Layer_EnviromentTrigger(const _tchar * pEnviromentTriggerFilePath)
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

HRESULT CScene_Stage05::Branch(_matrix mWorldMatrix)
{
	_float4 indexW;
	XMStoreFloat4(&indexW, mWorldMatrix.r[3]);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/* 0 : 파쿠르 1: LOBOTO */
	switch ((_uint)indexW.w)
	{
	case 0:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_ParkourTrigger"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 1:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_LOBOTOTrigger"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 2:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_CLOSE"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 3:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_CURLING"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 4:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_TUTOMSG"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 8:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_BLENDERPATH"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 9:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_CLIMBING"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	case 10:
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE05, TEXT("Layer_CookingTrigger"), &mWorldMatrix)))
			return E_FAIL;
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Stage05::Ready_Static_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage5.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE05, pLayerTemp, &ObjectInfo)))
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE05, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Stage05::Ready_Animation_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage5_Animation.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE05, pLayerTemp, &ObjectInfo)))
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
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_STAGE05, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Stage05::Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_INGHole0"))|| !lstrcmp(ObjectInfo.StrLayerName.c_str(), TEXT("Layer_Compt_INGHole1")))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_INGHole"), SCENE_STAGE05, TEXT("Layer_INGHole"), &ObjectInfo)))
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

HRESULT CScene_Stage05::Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
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

void CScene_Stage05::Ready_ShaderOption()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
	if (nullptr == pRenderer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	_float ArrWeight[4];

	ArrWeight[0] = 0.4f;
	ArrWeight[1] = 0.1f;
	ArrWeight[2] = 3.f;
	ArrWeight[3] = 0.5f;
	pRenderer->Set_BloomWeight(ArrWeight);
	pRenderer->Set_BlurOption(1);
	pRenderer->Set_ToneOption(2);
	pGameInstance->Set_ShadeAmbient(0.4f);
	pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_SHADOW, false);

	RELEASE_INSTANCE(CGameInstance);
}

CScene_Stage05 * CScene_Stage05::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Stage05*	pInstance = new CScene_Stage05(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CScene_Stage05");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage05::Free()
{
	__super::Free();
	Safe_Release(m_pNextLoader);
}

