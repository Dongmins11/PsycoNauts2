#include "stdafx.h"
#include "..\public\Scene_Demo.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "Scene_Loading.h"
#include "OfficeWalls.h"
#include "LoadingThreadUI.h"
#include "FadeInOut.h"


CScene_Demo::CScene_Demo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Demo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;
	if (FAILED(SetUp_Renderer()))
		return E_FAIL;
	if (FAILED(Ready_Light_Desc()))
		return E_FAIL;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(true);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_SkyBox"), 0)->Set_IsRender(true);
	/*static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpCreate(SCENE_DEMO);*/

	Ready_CameraOption();
	Ready_ShaderOption();


	Ready_Static_Object();
	Rendy_Animation_Object();
	Rendy_LightPoint();

	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_CurlingIndex(0);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Delete_CurlingIndex(-1);

	if (FAILED(Ready_Layer_EnviromentTrigger(TEXT("../../Data/Stage0_Trigger.dat"))))
		return E_FAIL;


	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//SOUND
	if (FAILED(pGameInstance->PlayBGM(TEXT("StageDemoBackGroundSound.ogg"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->BGMVolumeDown(0.8f);
	// UI
	//if (FAILED(Ready_Layer_UI(TEXT("Layer_Skill_UI"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_DemoRect(TEXT("Layer_DemoRect"))))
		return E_FAIL;

	/* For.Plane */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_Plane"), SCENE_DEMO, TEXT("Layer_Plane"))))
		return E_FAIL;

	/* For.Wall */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_Wall"), SCENE_DEMO, TEXT("Layer_Wall"))))
		return E_FAIL;

	/* For.Loboto */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Loboto"), SCENE_DEMO, TEXT("Layer_Loboto"))))
		return E_FAIL;

	/* For.Layer_Player_CutScene */
	_uint Arg = SCENE_DEMO;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Player_CutScene"), SCENE_DEMO, TEXT("Layer_Player_CutScene"), &Arg)))
		return E_FAIL;
	
	/* For.Layer_NPC_ScriptUI */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_ScriptUI"), SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"))))
		return E_FAIL;

	if (FAILED(Ready_EnvironmentEffect()))
		return E_FAIL;

	/* For.Layer_TutoMessageUI */
	UISIZEDESC desc;
	for (_uint i = 0; i < 8; ++i)
	{
		desc.fX = 10.f;
		desc.fY = 324.f;
		desc.fSizeX = 500.f;
		desc.fSizeY = 126.f;
		desc.iPassIndx = i;
		if (i == 4 || i == 6 || i == 7)
		{
			desc.fY = 262.f;
			desc.fSizeY = 250.f;
		}
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_TutoMessageUI"), SCENE_DEMO, TEXT("Layer_TutoMessageUI"),&desc)))
			return E_FAIL;
	}

	for (_uint i = 0; i < 4; ++i)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_MoveNpc"), SCENE_DEMO, TEXT("Layer_MoveNpc"))))
			return E_FAIL;
	}

	/* For.Layer_Trampoline */
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Trampoline"), SCENE_DEMO, TEXT("Layer_Trampoline"))))
	//	return E_FAIL;

	m_pNextLoader = CLoader::Create(m_pDevice, m_pDeviceContext, SCENE_THREADING);
	m_pNextLoader->Set_NextThreadingSceneID(SCENE_DEMO);

	/* For.Layer_ThreadLoading */
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Thread"), SCENE_DEMO, TEXT("Layer_ThreadLoading"), (CGameObject**)&m_pLoadingUI, nullptr)))
		return E_FAIL;
	
	//dynamic_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_OUT);
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->SetUp_PlayerPosition(SCENE_DEMO);

	RELEASE_INSTANCE(CGameInstance);



//	if (FAILED(Ready_Demo_Data()))
//		return E_FAIL;
	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));


	return S_OK;
}

_int CScene_Demo::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	//m_fFadeTime += TimeDelta;

	//if (1.5f < m_fFadeTime && m_isOns == true)
	//	m_isFadeIn = true;

	//if (m_isFadeIn && m_isOns)
	//{
	//	m_isOns = false;
	//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//	dynamic_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);
	//	RELEASE_INSTANCE(CGameInstance);
	//}

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


			//OutputDebugString(TEXT("Stage01로딩 끝\n"));
			Safe_Release(m_pNextLoader);
		}
	}
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(VK_BACK))
	{
		pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE01));
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CScene_Demo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

//#ifdef _DEBUG
//	if (nullptr != m_pNavigation)
//		m_pNavigation->Render(2);
//#endif _DEBUG

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}

HRESULT CScene_Demo::Ready_Light_Desc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Set_ShadowLightVector(_float3(0.079f, -0.997f, 0.0f), _float3(2.259f, 40.967f, 34.472f));

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.iType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.079f, -0.997f, 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	//LightDesc.iType = tagLightDesc::TYPE_SPOT;
	//LightDesc.vPosition = _float4(9.188f,5.860f,27.176f, 1.f);
	//LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	//LightDesc.fRange = 5.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.fInnerConeAngle = 0.1f;
	//LightDesc.fOuterConeAngle = 0.5f;

	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;

	//LightDesc.iType = tagLightDesc::TYPE_SPOT;
	//LightDesc.vPosition = _float4(9.188f, 5.860f, 40.176f, 1.f);
	//LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	//LightDesc.fRange = 5.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.fInnerConeAngle = 0.1f;
	//LightDesc.fOuterConeAngle = 0.5f;

	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;

	//LightDesc.iType = tagLightDesc::TYPE_SPOT;
	//LightDesc.vPosition = _float4(9.558f, 2.333f, 4.752f, 1.f);
	//LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	//LightDesc.fRange = 5.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.fInnerConeAngle = 0.1f;
	//LightDesc.fOuterConeAngle = 0.5f;

	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;

	//LightDesc.iType = tagLightDesc::TYPE_SPOT;
	//LightDesc.vPosition = _float4(8.671f, 2.048f, 13.81f, 1.f);
	//LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	//LightDesc.fRange = 5.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.fInnerConeAngle = 0.1f;
	//LightDesc.fOuterConeAngle = 0.5f;

	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;

	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(9.698f, 3.178f, 4.452f, 1.f);
	//LightDesc.fRange = 10.4f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(8.671f, 2.888f, 13.81f, 1.f);
	//LightDesc.fRange = 10.23f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(9.188f, 5.860f, 27.176f, 1.f);
	//LightDesc.fRange = 10.0;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	//
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(9.188f, 5.860f, 40.176f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light5 코너1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(9.331f, 6.29f, 53.945f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light6 사무실1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-3.412f, 6.286f, 58.342f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light7 사무실2*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-15.3, 6.288, 58.706, 1.f);
	//LightDesc.fRange = 14.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	//
	///*Light8 선반1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-34.107, 6.564f, 73.476, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light9 선반2*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-42.535f, 6.367f, 73.459f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light10 선반복도1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-60.856f, 6.253f, 72.013f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light11 선반복도2*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-72.643f, 6.088f, 71.218f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light12 선반복도3*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-81.908f, 6.334f, 72.163f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light13 캐비넷1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-82.301f, 6.466f, 86.012f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light14 캐비넷2*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-81.864f, 6.128f, 98.439f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light15 캐비넷3*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-83.465f, 6.493f, 107.759f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light16 렉트1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-99.093f, 1.178f, 107.113f, 1.f);
	//LightDesc.fRange = 4.355f;
	//LightDesc.fPower = 2.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	/////*Light17 렉트2*/
	////LightDesc.iType = tagLightDesc::TYPE_POINT;
	////LightDesc.vPosition = _float4(-103.902f, 6.243f, 107.269f, 1.f);
	////LightDesc.fRange = 10.02f;
	////LightDesc.fPower = 1.f;
	////LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	////LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	////LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	////
	////if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	////	return E_FAIL;
	//
	///*Light18 렉트3*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-108.933f, 1.151f, 107.12f, 1.f);
	//LightDesc.fRange = 4.545f;
	//LightDesc.fPower = 2.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	/////*Light19 렉트4*/
	////LightDesc.iType = tagLightDesc::TYPE_POINT;
	////LightDesc.vPosition = _float4(-117.411f, 6.518f, 107.167f, 1.f);
	////LightDesc.fRange = 10.02f;
	////LightDesc.fPower = 1.f;
	////LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	////LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	////LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	////
	////if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	////	return E_FAIL;
	//
	///*Light20 렉트5*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-120.911f, 1.084f, 107.132f, 1.f);
	//LightDesc.fRange = 4.515f;
	//LightDesc.fPower = 2.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//pGameInstance->Set_LightRender(false, 18);
	//
	/////*Light21 렉트6*/
	////LightDesc.iType = tagLightDesc::TYPE_POINT;
	////LightDesc.vPosition = _float4(-130.151f, 6.650f, 107.259f, 1.f);
	////LightDesc.fRange = 10.02f;
	////LightDesc.fPower = 1.f;
	////LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	////LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	////LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	////
	////if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	////	return E_FAIL;
	//
	///*Light22 렉트7*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-133.963f, 1.202f, 107.148f, 1.f);
	//LightDesc.fRange = 4.47f;
	//LightDesc.fPower = 2.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//pGameInstance->Set_LightRender(false, 19);
	//
	///*Light23 렉트8*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-148.447f, 1.136f, 107.208f, 1.f);
	//LightDesc.fRange = 4.585f;
	//LightDesc.fPower = 2.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//pGameInstance->Set_LightRender(false, 20);
	//
	///*Light24 파쿠르1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-159.195f, 4.658f, 108.121f, 1.f);
	//LightDesc.fRange = 6.105f;
	//LightDesc.fPower = 2.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light25 파쿠르2*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-156.324f, 13.653f, 108.126f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light26 파쿠르3*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-156.858, 16.711f, 116.57f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light27 파쿠르4*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-156.602f, 24.828f, 118.859f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light28 끝1*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-156.103f, 25.527f, 133.282f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	//
	///*Light29 끝2*/
	//LightDesc.iType = tagLightDesc::TYPE_POINT;
	//LightDesc.vPosition = _float4(-155.989f, 24.8f, 143.524f, 1.f);
	//LightDesc.fRange = 10.02f;
	//LightDesc.fPower = 1.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

//HRESULT CScene_Demo::Ready_Layer_Camera(const _tchar * pLayerTag)
//{
//	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
//
//	/* For.Camera_Fly*/
//	CCamera::CAMEREADESC			CameraDesc;
//	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMEREADESC));
//	CameraDesc.vEye = _float3(0.f, 10.f, -8.f);
//	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
//	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
//	CameraDesc.fFovy = XMConvertToRadians(60.0f);
//	CameraDesc.fAspect = g_iWinCX / (_float)g_iWinCY;
//	CameraDesc.fNear = 0.2f;
//	CameraDesc.fFar = 300.f;
//	
//	CameraDesc.TransformDesc.SpeedPerSec = 10.f;
//	CameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
//	
//	
//	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_Camera"), SCENE_DEMO, pLayerTag, &CameraDesc)))
//		return E_FAIL;
//	
//	RELEASE_INSTANCE(CGameInstance);
//
//	return S_OK;
//}
//

HRESULT CScene_Demo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	
	/* For.Terrain */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_Terrain"), SCENE_DEMO, pLayerTag)))
		return E_FAIL;

	/* For.Sky*/
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_Sky"), SCENE_DEMO, pLayerTag)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Demo::Ready_Layer_Monster(const _tchar * pLayerTag)
{

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < 10; ++i)
	{
		/* For.Monster */
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_Monster"), SCENE_DEMO, pLayerTag)))
			return E_FAIL;
	}

	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Demo::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_EffectRect"), SCENE_DEMO, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_EffectPoint"), SCENE_DEMO, pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Demo::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_UI"), SCENE_DEMO, pLayerTag)))
	//	return E_FAIL;
	//
	//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), SCENE_DEMO, pLayerTag)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Demo::Ready_Layer_EnviromentTrigger(const _tchar * pEnviromentTriggerFilePath)
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

HRESULT CScene_Demo::Ready_Layer_DemoRect(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Rect */
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_DemoRect"), SCENE_DEMO, pLayerTag)))
		return E_FAIL;
	
	/* For.OfficeWall_Left */
	COfficeWalls::WALLTYPE eWallType = COfficeWalls::WALLTYPE_LEFT;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_OfficeWalls"), SCENE_DEMO, TEXT("Layer_OfficeWalls"),&eWallType)))
		return E_FAIL;

	/* For.OfficeWall_Right */

	eWallType = COfficeWalls::WALLTYPE_RIGHT;
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_OfficeWalls"), SCENE_DEMO, TEXT("Layer_OfficeWalls"), &eWallType)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Demo::Branch(_matrix mWorldMatrix)
{
	_float4 indexW;
	XMStoreFloat4(&indexW, mWorldMatrix.r[3]);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/* 0 : 파쿠르 1: LOBOTO */
	switch ((_uint)indexW.w)
	{
		case 0:
		{

			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_ParkourTrigger"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 1:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_LOBOTOTrigger"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 2:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_CLOSE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 3:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_CURLING"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 4:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_TUTOMSG"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 5:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_STATE0_END"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 6:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_STATE0_NPC"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
		case 7:
		{
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_DEMO, TEXT("Layer_MONSTER_CREATE"), &mWorldMatrix)))
				return E_FAIL;
			break;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Demo::Ready_Static_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage0.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_DEMO, pLayerTemp, &ObjectInfo)))
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
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_DEMO, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Demo::Rendy_Animation_Object()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage0_Animation.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
			if (FAILED(Object_Change_Tag(ObjectInfo, pLayerTemp)))
			{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_DEMO, pLayerTemp, &ObjectInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
		}
		else
		{
			Safe_Delete(pLayerTemp);
			if (FAILED(Object_Change_Tag(ObjectInfo, pLayerTag)))
			{
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, ObjectInfo.StrObjectFullName.c_str(), SCENE_DEMO, pLayerTag, &ObjectInfo)))
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

HRESULT CScene_Demo::Rendy_LightPoint()
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;

	hFile = CreateFile(TEXT("../../Data/Stage0Light.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

		pGameInstance->Set_Light_CurlingIndex(LightCount,CurIndex);
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CScene_Demo::Ready_EnvironmentEffect()
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

HRESULT CScene_Demo::Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar * LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (LayerType::LAYERTYPE_FALL_CABINET == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_MoveCabinet"), SCENE_DEMO, TEXT("Layer_MoveCabinet"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		
		return S_OK;
	}
	else if (LayerType::LAYERTYPE_ANIMATED_SHELV == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_AnimatedShelv"), SCENE_DEMO, TEXT("Layer_AnimatedShelv"), &ObjectInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	else if (LayerType::LAYERTYPE_FALL_ITEM_STATE0_END == (LayerType)ObjectInfo.iPX_Index)
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_FallObject"), SCENE_DEMO, TEXT("Layer_FallObject"), &ObjectInfo)))
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
}

HRESULT CScene_Demo::Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag0") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag1") ||
		!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag2") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag3") || !lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_Bag4"))
	{
		//if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Bag"), SCENE_DEMO, LayerTag, &ObjectInfo)))
		//{	
		//	RELEASE_INSTANCE(CGameInstance);
		//	return E_FAIL;
		//}
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
	else if (!lstrcmp(ObjectInfo.StrLayerName.c_str(), L"Layer_ZiantLoboto0"))
	{
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_DEMO, TEXT("Prototype_GameObject_ZiantLoboto0"), SCENE_DEMO, LayerTag, &ObjectInfo)))
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

HRESULT CScene_Demo::SetUp_Renderer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pRenderer =dynamic_cast<CRenderer*>(pGameInstance->Clone_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer")));
	if (nullptr == m_pRenderer)
	{
		RELEASE_INSTANCE(CGameInstance);
		MSGBOX("Renderer nullptr");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CScene_Demo::Ready_ShaderOption()
{
	m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_SSAO, false);
	_float BloomWeight[4] = { 0.2f,0.05f,1.f,1.f };
	m_pRenderer->Set_BloomWeight(BloomWeight);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_ShadeAmbient(0.2f);
	RELEASE_INSTANCE(CGameInstance);
}

void CScene_Demo::Ready_CameraOption()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Fly* pCam = ((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0));
	if (nullptr == pCam)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pCam->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
	pCam->Set_DependentType_Value(_float4(0.7f, 0.1f, 4.7f, 4.f), 1.6f);
	pCam->Set_Fovy(60.f);
	RELEASE_INSTANCE(CGameInstance);
}

CScene_Demo * CScene_Demo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CScene_Demo*	pInstance = new CScene_Demo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CScene_Demo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Demo::Free()
{
	__super::Free();
	//Safe_Release(m_pNavigation);
	
	//FreeConsole();

	Safe_Release(m_pRenderer);

}
