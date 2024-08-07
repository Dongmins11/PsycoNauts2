#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "Scene_Logo.h"
#include "Scene_Loading.h"
#include "../../ImGUI/imgui_impl_dx11.h"

#include "ImGUIManager.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "SkyBox.h"
#include "Static_Mesh.h"
#include "Bag.h"
#include "Player_LeftHand.h"
#include "Player_RightHand.h"
#include "Skill_PisBlast.h"
#include "Skill_BrainStrom.h"
#include "Loading_UI.h"
#include "LoadingThreadUI.h"

#include "PlayerTrigger_Sphere.h"
#include "PlayerTrigger_Parkour.h"
#include "PlayerTrigger_Body.h"
#include "EnviromentTrigger.h"

#include "HaroldMouse.h"
#include "HealthPack.h"
#include "FadeInOut.h"
#include "QuestManager.h"
#include "Loading_Player.h"
//#include "NavMeshCollider.h"
//#include "BoxCollider.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
	HRESULT		hr = 0;

	if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		return E_FAIL;

	if (FAILED(Initialize_Engine()))
		return E_FAIL;

	//IMGUI Device초기화
	ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
	CImGUIManager::Get_Instance()->SettingDevice(m_pDevice, m_pDeviceContext);

	//if (FAILED(Ready_Gara()))
	//	return E_FAIL;
	srand((_uint)time(NULL));


	if (FAILED(Ready_Component_Prototype_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_GameObject_Prototype_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_GameObject_Layer_ForStatic()))
		return E_FAIL;

	if (FAILED(Open_Scene(SCENE_LOGO)))
		return E_FAIL;

	return S_OK;
}

_int CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Tick_Engine(TimeDelta);

#ifdef _DEBUG
	m_TimeAcc += TimeDelta;
#endif // _DEBUG

	return _int();
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();
	m_pGameInstance->Render_Engine();

#ifdef _DEBUG
	if (GetKeyState('Z') & 0x8000)
	{
		m_bRenderGUI = false;

	}
	if (GetKeyState('E') & 0x8000)
	{
		m_bRenderGUI = true;

	}
	if(true ==m_bRenderGUI)
		CImGUIManager::Get_Instance()->Render();

	++m_iNumRender;

	if (m_TimeAcc > 1.0)
	{
		CImGUIManager::Get_Instance()->Set_Frames(m_iNumRender);
		wsprintf(m_szFPS, TEXT("프레임 : %d"), m_iNumRender);
		m_iNumRender = 0;
		m_TimeAcc = 0.0;
	}

	//if (FAILED(m_pGameInstance->Draw_Font(TEXT("Font_Gulim"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
	//	return E_FAIL;
#endif // _DEBUG
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Initialize_Engine()
{
	CGraphic_Device::GRAPHICDESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;

	INPUTDESC		InputDesc;
	ZeroMemory(&InputDesc, sizeof(INPUTDESC));

	InputDesc.hInst = g_hInst;
	InputDesc.hWnd = g_hWnd;

	if (FAILED(m_pGameInstance->Initialize_Engine(SCENE_END, InputDesc, GraphicDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;


	//CPhysXScene_Manager* pPhysMgr = CPhysXScene_Manager::Get_Instance();
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER,LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER,LayerType::LAYERTYPE_PICK);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_PLANE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_TRIGGER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_ITEM);
	//m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_MONSTER_FRUIT);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_RAIL);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_BOUNCER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_CLIMBINGPLANE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_TREASURE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER, LayerType::LAYERTYPE_CLOUD);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_QUEST);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_STATIC, LayerType::LAYERTYPE_STATIC);
	//m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_STATIC, LayerType::LAYERTYPE_PLAYER_TRIGGER);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PICK, LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PICK, LayerType::LAYERTYPE_PLANE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PICK,LayerType::LAYERTYPE_PICK);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PICK, LayerType::LAYERTYPE_MONSTER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PICK, LayerType::LAYERTYPE_CLIMBINGPLANE);


	//m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PICK, LayerType::LAYERTYPE_PLAYER_TRIGGER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_PLANE);
	//m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_PICK);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_MONSTER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_PLATINGPLATE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_TRIGGER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_FRUIT, LayerType::LAYERTYPE_CLIMBINGPLANE);


	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_PICK);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_TRIGGER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_ITEM);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_MONSTER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_BLADE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_BLENDER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_RAIL);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_BOUNCER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_CLIMBINGPLANE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_GRIDDLE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_TREASURE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_CLOUD);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_ZIP);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_TRIGGER, LayerType::LAYERTYPE_BOSS_DISH);
	

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_SKILL, LayerType::LAYERTYPE_PLANE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_SKILL, LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_SKILL, LayerType::LAYERTYPE_MONSTER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_PLAYER_SKILL, LayerType::LAYERTYPE_TREASURE);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER_ATTACK, LayerType::LAYERTYPE_PLAYER_TRIGGER); 
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_MONSTER, LayerType::LAYERTYPE_STATIC);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_TRIGGER, LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_CAMERA, LayerType::LAYERTYPE_PLANE);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_CAMERA, LayerType::LAYERTYPE_STATIC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_FALL_ITEM_STATE0_END, LayerType::LAYERTYPE_PLANE);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_BRAINSTROM, LayerType::LAYERTYPE_NPC);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_BRAINSTROM, LayerType::LAYERTYPE_MONSTER);


	//m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_TRIGGER, LayerType::LAYERTYPE_ITEM);
	//m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_TRIGGER, LayerType::LAYERTYPE_PLANE);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_KITCHENPANBUTTON, LayerType::LAYERTYPE_PLAYER_TRIGGER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_KITCHENPANBUTTON, LayerType::LAYERTYPE_PLAYER);

	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_2DMAP, LayerType::LAYERTYPE_2D_PLAYER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_2D_PLAYER, LayerType::LAYERTYPE_2D_MONSTER);
	m_pGameInstance->Add_LayerMask(LayerType::LAYERTYPE_2D_PLAYER, LayerType::LAYERTYPE_2D_KEY);

	return S_OK;
}

HRESULT CMainApp::Ready_Component_Prototype_ForStatic()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;


	/* for.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	CImGUIManager::Get_Instance()->SettingRenderer(m_pRenderer);

	/* for.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* for.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	///* for.Prototype_Component_VIBuffer_Rect */
	//if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext,2.f,2.f))))
	//	return E_FAIL;

	/* for.Prototype_Component_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Default"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Test/Default%d.png"), 1))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_HDR */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_HDR"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/TestHDR/TestHDR.dds"), 1))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Raz */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Raz"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Raz_RenderTexture/TEX_CV_Harold%d.dds"), 1))))
		return E_FAIL;

	///*For.Navigation*/
	//if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../../Data/NaviData.dat")))))
	//	return E_FAIL;

	// 형석 : UI Texture Prototype

	/* for.Prototype_Component_Texture_Key */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Key"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Key/Keys%d.dds"), 7))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Loading*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Loading"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading%d.dds"), 6))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_LoadingThread*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_LoadingThread"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingThread%d.dds"),6))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Loading_Mix*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Loading_Mix"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading_Mix%d.png"), 2))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo%d.dds"), 3))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Title"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Title.png"), 1))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Fade */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Fade"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/FadeInOut/FadeInOut%d.dds"), 2))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Dissolve */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Dissolve/Dissolve%d.dds"), 23))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_SkyBox"), CModel::Create(m_pDevice, m_pDeviceContext, "../bin/Resources/Meshes/SkyBox/SkyBox.dat", "../bin/Resources/Meshes/SkyBox/", XMMatrixIdentity() *XMMatrixScaling(0.01f, 0.01f, 0.01f)))))
		return E_FAIL;

	/*For.CharacterController*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_ChracterController"), CCharacterController::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_Navigation"), CNavMeshCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_PxBox */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), CBoxCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_PxSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), CSphereCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	///* For.Prototype_Component_Collider_PxMeshCollider*/
	//if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxMeshCollider"), CMeshCollider::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	/* for.Prototype_Component_EffectRect*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_EffectRect"), CVIBuffer_EffectRect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* for.Prototype_Component_Trail*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBuffer_Trail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
	/* for.Prototype_Component_EffectOption*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), CEffect_Option::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
	/* For.DiffuseTexture */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Effect/Texture/Diffuse/Diffuse%d.dds"), 6))))
		return E_FAIL;

	/* For.AlphaTexture */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Effect/Texture/Alpha/Alpha%d.dds"), 74))))
		return E_FAIL;

	/* For.AlphaTexture */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_LightTexture"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/LightTexture/LightTexture%d.dds"), 3))))
		return E_FAIL;

	/* For.AlphaTexture */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Cloud"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Cloud/Cloud%d.dds"), 5))))
		return E_FAIL;

	/*For.Model_Player*/
	_matrix PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Player/Player0.dat", "../Bin/Resources/Meshes/Player/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_Boss"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Player/Player1.dat", "../Bin/Resources/Meshes/Player/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_CutScene"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Player/Player2.dat", "../Bin/Resources/Meshes/Player/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_CutScene_Two"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Player/Player3.dat", "../Bin/Resources/Meshes/Player/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_LoadingPlayer"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Player/Player4.dat", "../Bin/Resources/Meshes/Player/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_LeftHand"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/RazLeftHand/RazLeftHand0.dat", "../Bin/Resources/Meshes/Stage0/Anim/RazLeftHand/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_RightHand"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/RazRightHand/RazRightHand0.dat", "../Bin/Resources/Meshes/Stage0/Anim/RazRightHand/", PivotMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Ground"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/NonAnim/Rug/Rug0.dat", "../Bin/Resources/Meshes/Stage0/NonAnim/Rug/", PivotMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_HaroldMouse"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/HaroldMouse/HaroldMouse0.dat", "../Bin/Resources/Meshes/Stage0/Anim/HaroldMouse/", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Effect_PisBlast"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/NonAnim/PisBlast/PisBlast0.dat", "../Bin/Resources/Meshes/Stage0/NonAnim/PisBlast/", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Loboto"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/Loboto/Loboto0.dat", "../Bin/Resources/Meshes/Stage0/Anim/Loboto/", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_HealthPack"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage1/Anim/HealthPack/HealthPack0.dat", "../Bin/Resources/Meshes/Stage1/Anim/HealthPack/", PivotMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_GameObject_Prototype_ForStatic()
{
	/*For.SkyBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_SkyBox"), CSkyBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Player"), CLoading_Player::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LeftHand */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_LeftHand"), CPlayer_LeftHand::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RightHand */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_RightHand"), CPlayer_RightHand::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_PisBlast */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_PisBlast"), CSkill_PisBlast::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_BrainStrom */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_BrainStrom"), CSkill_BrainStrom::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_HaroldMouse */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_HaroldMouse"), CHaroldMouse::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_HealthPack */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_HealthPack"), CHealthPack::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/*For.CameraFly*/
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Camera"), CCamera_Fly::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StaticMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_StaticMesh"), CStatic_Mesh::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Trigger"), CPlayerTrigger_Sphere::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Player_ParkourTrigger"), CPlayerTrigger_Parkour::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Player_BodyTrigger"), CPlayerTrigger_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), CEnviromentTrigger::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Bog       => 툴에서 만들어 진거 확인  */
	//if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Bag"), CBag::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Loading_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_UI"), CLoading_UI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Loading_Thread */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Loading_Thread"), CLoadingThreadUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fade */
	if (FAILED(m_pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Fade"), CFadeInOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_GameObject_Layer_ForStatic()
{
	/*For.Layer_GameObject_Player*/
	if (FAILED(m_pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Player"), SCENE_STATIC, TEXT("Layer_Player"))))
		return E_FAIL;

	/*For.Layer_GameObject_SkyBox*/
	if (FAILED(m_pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_SkyBox"), SCENE_STATIC, TEXT("Layer_SkyBox"))))
		return E_FAIL;

	/*For.Layer_GameObject_Camera_Fly*/
	CCamera::CAMEREADESC tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(tCameraDesc));

	tCameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	tCameraDesc.vEye = _float3(-0.5f, 0.15f, -7.5f);
	tCameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	tCameraDesc.fAspect = g_iWinCX / (_float)g_iWinCY;
	tCameraDesc.fFovy = XMConvertToRadians(60.f);
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.fFar = 100.f;
	tCameraDesc.TransformDesc.SpeedPerSec = 5.0;
	tCameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(30.f);

	if (FAILED(m_pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Camera"), SCENE_STATIC, TEXT("Layer_Camera"), &tCameraDesc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_HaroldMouse"), SCENE_STATIC, TEXT("Layer_HaroldMouse"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_HealthPack"), SCENE_STATIC, TEXT("Layer_HealthPack"))))
	//	return E_FAIL;

	/*For.Layer_GameObject_Fade*/
	if (FAILED(m_pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_Fade"), SCENE_STATIC, TEXT("Layer_Fade"))))
		return E_FAIL;
	return S_OK;
}
HRESULT CMainApp::Open_Scene(SCENE eScene)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, eScene))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	HANDLE			hFile = 0;

	_ulong	dwByte = 0;
	hFile = CreateFile(TEXT("../../Data/NaviData.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(0.f, 0.f, 5.f);
	vPoints[1] = _float3(5.f, 0.f, 0.f);
	vPoints[2] = _float3(0.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(0.f, 0.f, 5.f);
	vPoints[1] = _float3(5.f, 0.f, 5.f);
	vPoints[2] = _float3(5.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(5.f, 0.f, 5.f);
	vPoints[2] = _float3(0.f, 0.f, 5.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(5.f, 0.f, 5.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(5.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	// MakeSpriteFont "넥슨 풋볼고딕 B 굵게" /FontSize:32 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 kodia.spritefont
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pDeviceContext, TEXT("Font_Gulim"), TEXT("../Bin/Resources/Fonts/kodia.spritefont"))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	CQuestManager::Destroy_Instance();
	Safe_Release(m_pGameInstance);
	CImGUIManager::Get_Instance()->ReleaseRenderer();
	//IMGUI DX11 릴리즈
	ImGui_ImplDX11_Shutdown();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}

