#include "stdafx.h"
#include "..\public\ImGUIManager.h"
#include "../../ImGUI/imgui.h"
#include "../../ImGUI/imgui_impl_dx11.h"
#include "../../ImGUI/imgui_impl_win32.h"
#include "GameInstance.h"
//#include "Scene_Logo.h"
#include "Scene_Loading.h"
#include "Target_Manager.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "Light_Manager.h"
#include "Light.h"
#include "DemoRect.h"
#include "OfficeWalls.h"
#include "Static_Mesh.h"
#include "FallObject.h"
#include "VentDoor.h"
CImGUIManager* CImGUIManager::m_pInstance = nullptr;
typedef void(*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback  GImGuiDemoMarkerCallback;
extern void*                    GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback         GImGuiDemoMarkerCallback = NULL;
void*                           GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

CImGUIManager::CImGUIManager()
{
}

CImGUIManager::~CImGUIManager()
{
	if (LayerNameVector.size() > 0)
	{
		for (auto& pLayerTag : LayerNameVector)
		{
			Safe_Delete(pLayerTag);
		}
		LayerNameVector.clear();
	}

	if (m_ObjectVector.size() > 0)
	{
		for (auto& pObject : m_ObjectVector)
		{
			Safe_Delete(pObject);
		}
		m_ObjectVector.clear();
	}
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	ImGui::DestroyContext();
}

void CImGUIManager::Initialize_ImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("../../ImGUI/malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());
	ImGui::StyleColorsDark();

	/*For.Bloom*/

	m_BloomWeight[0] = 0.4f;
	m_BloomWeight[1] = 0.1f;
	m_BloomWeight[2] = 1.f;
	m_BloomWeight[3] = 1.f;

	/*For.BloomEffect*/

	m_BloomEffectWeight[0] = 1.f;
	m_BloomEffectWeight[1] = 0.245f;
	m_BloomEffectWeight[2] = 0.5f;
	m_BloomEffectWeight[3] = 0.625f;

	/*For.DOF*/
	m_DOFWeight[0] = 0.1f;
	m_DOFWeight[1] = 0.1f;
	m_DOFWeight[2] = 15.f;
	m_DOFWeight[3] = 20.f;

	/*For.Camera - Dependent*/

	m_vCamera_Dependent_Value = _float4(0.8f, 0.1f, 4.f, 3.f);

	m_fCamera_Dependent_IntervalZ = 2.f;
	m_fFovy_Angle = 70.f;


	m_vScale[0] = _float3(9.945f, 8.63f, 1.f);
	m_vPos[0] = _float3(-116.09f, 3.002f, 107.241f);

	m_vScale[1] = _float3(1.f, 1.f, 1.f);
	m_vPos[1] = _float3(-116.09f, 0.132f, 103.391f);
	m_fRad[0] = -2.715f;

	m_vScale[2] = _float3(1.f, 1.f, 1.f);
	m_vPos[2] = _float3(-116.09f, 0.132f, 110.931f);
	m_fRad[1] = -0.63f;

	m_fSourSpeed[0] = 0.2f;
	m_fDestSpeed[0] = 1.7f;

	m_fSourSpeed[1] = 0.2f;
	m_fDestSpeed[1] = 1.5f;

	m_fMaxTime[0] = 2.3f;
	m_fMaxTime[1] = 2.f;

	m_vRimRatio = _float2(1.f, 1.f);

	/*For.Fog*/
	m_fFog_Values[0] = 40.f;
	m_fFog_Values[1] = 0.03f;
	m_fFog_Values[2] = 0.001f;
}

void CImGUIManager::SettingDevice(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}
void CImGUIManager::SettingRenderer(CRenderer * pRenderer)
{
	m_pRenderer = pRenderer;

	Safe_AddRef(m_pRenderer);
}
#ifdef _DEBUG
void CImGUIManager::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	Render_DebugButtons();
	ImGui::End();

	// 카메라 & Scene
	if (m_isOpenDebugs[0])
	{
		ImGui::Begin("Default_Camera");
		Render_Frame();
		Render_SceneMove();
		Render_CameraDebug();
		ImGui::End();
	}

	// Animation
	if (m_isOpenDebugs[1])
	{
		ImGui::Begin("Player_Animation");
		Render_Animation();
		ImGui::End();
	}

	// Player State
	if (m_isOpenDebugs[2])
	{
		ImGui::Begin("Player");
		Render_PlayerState();
		ImGui::End();
	}

	// Layer & Object
	if (m_isOpenDebugs[3])
	{
		ImGui::Begin("Hierarchy");
		Render_Layer_Hierarchy();
		ImGui::End();
	}

	// Shader
	if (m_isOpenDebugs[4])
	{
		ImGui::Begin("Shader");
		Render_Shader_Default();
		ImGui::End();
	}

	// Action
	if (m_isOpenDebugs[5])
	{
		ImGui::Begin("Action");
		Render_Action_Default();
		ImGui::End();
	}
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGUIManager::Render_DebugButtons()
{
	for (_uint i = 0; i < m_iSizeOfDebugs; ++i)
	{
		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button(m_isDebugButtonText[i]))
		{
			m_isOpenDebugs[i] = !m_isOpenDebugs[i];
		}
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}
}

void CImGUIManager::Render_Frame()
{

	ImGui::Text(u8"현재 프레임: %d", m_iFrames);
	ImGui::Checkbox(u8"랜더타겟 버퍼 그리기", &isRenderTargetRender);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_TargeBufferIsRender(isRenderTargetRender);
	RELEASE_INSTANCE(CGameInstance);

	ImGui::Separator();
}

void CImGUIManager::Render_SceneMove()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	ImGui::BulletText(u8"현재 씬 : "); ImGui::SameLine();
	const char* _szBuff;
	_szBuff = ToString((SCENE)pGameInstance->Get_CurrentSceneIndex());
	ImGui::Text(_szBuff);

	ImGui::Text(u8"이동할 씬 선택");

	_bool Move_to_Scene00 = ImGui::Button("LOGO"); ImGui::SameLine();
	_bool Move_to_Scene01 = ImGui::Button("DEMO");

	SCENE eSceneNum = SCENE_END;
	if (Move_to_Scene00)
	{
		eSceneNum = SCENE_LOGO;
	}
	if (Move_to_Scene01)
	{
		eSceneNum = SCENE_DEMO;
	}

	if (SCENE_END != eSceneNum)
	{
		pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, eSceneNum));
	}

	RELEASE_INSTANCE(CGameInstance);
	ImGui::Separator();

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ImVec4 vFontColorX = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4 vFontColorY = ImVec4(1.f, 1.f, 1.f, 1.f);

	if (pt.x > 0 && pt.x <= g_iWinCX)
		vFontColorX = ImVec4(1.f, 1.f, 1.f, 1.f);
	else
		vFontColorX = ImVec4(1.f, 0.f, 0.f, 1.f);

	if (pt.y > 0 && pt.y <= g_iWinCY)
		vFontColorY = ImVec4(1.f, 1.f, 1.f, 1.f);
	else
		vFontColorY = ImVec4(1.f, 0.f, 0.f, 1.f);

	ImGui::BulletText(u8"현재 마우스 좌표");
	ImGui::TextColored(vFontColorX, u8"X: %d", pt.x);  ImGui::SameLine();
	ImGui::TextColored(vFontColorY, u8"Y: %d", pt.y);
}

void CImGUIManager::Render_CameraDebug()
{
	IMGUI_MARKER("Camera");
	if (ImGui::TreeNode("Camera"))
	{
		static int e = 0;
		ImGui::RadioButton(u8"자유시점 카메라", &e, 0); ImGui::SameLine();
		ImGui::RadioButton(u8"플레이어 카메라", &e, 1);

		if (e == 0)
		{
			ImGui::BulletText(u8"현재 카메라 : 자유시점 ");
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CCamera_Fly* pCamera = (CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0);
			pCamera->Set_CameraType(CCamera_Fly::CAMERATYPE_INDEPENDENT);

			_vector vCamPos = pCamera->Get_Camera_TransformState(CTransform::STATE_POSITION);
			_vector vCamLook = pCamera->Get_Camera_TransformState(CTransform::STATE_LOOK);
			_float3 fCamPos = STOREFLOAT3(vCamPos);
			_float3 fCamLook = STOREFLOAT3(vCamLook);
			//ImGui::InputFloat("red", &foo, 0.05f, 0, "%.3f");
			//ImGui::InputFloat3("Camera Look", &fCamLook.x, "%.3f");
			ImGui::Text("Camera Look");
			ImGui::InputFloat("Look X", &fCamLook.x, 0.05f, 0, "%.3f");
			ImGui::InputFloat("Look Y", &fCamLook.y, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Look Z", &fCamLook.z, 0.01f, 1.0f, "%.3f");

			ImGui::Text("Camera Pos");
			ImGui::InputFloat("Pos X", &fCamPos.x, 0.05f, 0, "%.3f");
			ImGui::InputFloat("Pos Y", &fCamPos.y, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Pos Z", &fCamPos.z, 0.01f, 1.0f, "%.3f");
			//ImGui::InputFloat3("Camera Pos",&fCamPos.x);
			pCamera->Set_Camera_TransformState(CTransform::STATE_POSITION, fCamPos);
			pCamera->Set_Camera_TransformState(CTransform::STATE_LOOK, fCamLook);

			// 
			RELEASE_INSTANCE(CGameInstance);

		}
		else
		{
			ImGui::BulletText(u8"현재 카메라 : 플레이어 공전 ");
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CCamera_Fly* pCamera = (CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0);
			pCamera->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);

			_vector vPlayerPos = ((CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0))->Get_Position();
			_vector vLegnth = vPlayerPos - pCamera->Get_Camera_TransformState(CTransform::STATE_POSITION);
			_float fLegnth = GET_VECTORLENGTH(vLegnth);
			ImGui::BulletText(u8"플레이어-카메라 간"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 162.f, 232.f, 1.0f), u8"거리"); ImGui::SameLine();
			ImGui::Text(": %.3f", fLegnth);
			ImGui::Separator();

			ImGui::TextColored(ImVec4(0.0f, 255.f, 0.f, 1.0f), u8"현재 카메라 네비게이션 Index : 추가예정");
			//_vector Compute_Pos_Dependent(_double dTimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval);

			ImGui::DragFloat("Max Dot", &m_vCamera_Dependent_Value.x, 0.005f);
			ImGui::DragFloat("Min Dot", &m_vCamera_Dependent_Value.y, 0.005f);
			ImGui::DragFloat("UpDis", &m_vCamera_Dependent_Value.z, 0.005f);
			ImGui::DragFloat("DownDis", &m_vCamera_Dependent_Value.w, 0.005f);
			ImGui::DragFloat("IntervalZ", &m_fCamera_Dependent_IntervalZ, 0.005f);
			ImGui::DragFloat("Fovy", &m_fFovy_Angle, 0.005f);

			pCamera->Set_DependentType_Value(m_vCamera_Dependent_Value, m_fCamera_Dependent_IntervalZ);
			pCamera->Set_Fovy(m_fFovy_Angle);
			RELEASE_INSTANCE(CGameInstance);
			//Update_Dependent(TimeDelta, 0.95f, 0.4f, 5.f, 1.f, _float4(0.f, 5.f, 1.f, 1.f));

		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	else
	{
		ImGui::Separator();
	}
}

void CImGUIManager::Render_Animation()
{
	ImGui::TextColored(ImVec4(255.f, 127.f, 39.f, 1.0f), u8"플레이어 애니메이션 정보");
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CModel* pPlayerModel = (CModel*)(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Model"), 0));
	RELEASE_INSTANCE(CGameInstance);

	ImGui::Columns(3, "PlayerAnimation"); // 4-ways, with border
	ImGui::Separator();
	ImGui::Text("Root Index"); ImGui::NextColumn();
	ImGui::Text("Upper Index"); ImGui::NextColumn();
	ImGui::Text("Bottom Index"); ImGui::NextColumn();
	ImGui::Text("\t\t%d", pPlayerModel->Get_RootAnimIndex()); ImGui::NextColumn();
	ImGui::Text("\t\t%d", pPlayerModel->Get_UpperAnimIndex()); ImGui::NextColumn();
	ImGui::Text("\t\t%d", pPlayerModel->Get_BottomAnimIndex()); ImGui::NextColumn();

	if (pPlayerModel->Get_IsRootAnimFinishied(pPlayerModel->Get_RootAnimIndex()))
	{
		ImGui::Text("Finished");
		ImGui::NextColumn();
	}
	else
		ImGui::NextColumn();
	if (pPlayerModel->Get_IsUpperAnimFinishied(pPlayerModel->Get_UpperAnimIndex()))
	{
		ImGui::Text("Finished");
		ImGui::NextColumn();
	}
	else
		ImGui::NextColumn();
	if (pPlayerModel->Get_IsBottomAnimFinishied(pPlayerModel->Get_BottomAnimIndex()))
	{
		ImGui::Text("Finished");
	}

	ImGui::Columns(1);
	ImGui::Separator();

	_float3 vSpeeds = _float3(pPlayerModel->Get_RootAnimSpeed(pPlayerModel->Get_RootAnimIndex()),
		pPlayerModel->Get_UpperAnimSpeed(pPlayerModel->Get_UpperAnimIndex()),
		pPlayerModel->Get_BottomAnimSpeed(pPlayerModel->Get_BottomAnimIndex()));

	_float3 vBlendSpeeds = _float3(pPlayerModel->Get_RootAnimBlendSpeed(pPlayerModel->Get_RootAnimIndex()),
		pPlayerModel->Get_UpperAnimBlendSpeed(pPlayerModel->Get_UpperAnimIndex()),
		pPlayerModel->Get_BottomAnimBlendSpeed(pPlayerModel->Get_BottomAnimIndex()));
	ImGui::InputFloat("Drag Speed", &m_fAnimScrollSpeed, 0, 0, "%.2f");

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0.f, 128.f / 255.f, 1.f), "****Drag/DoubleClick To Change****");
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), "Root Anim Speed: %.2f", vSpeeds.x);

	ImGui::DragFloat("RootAnim", &vSpeeds.x, 0.05f, 0, 0, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	pPlayerModel->Set_RootAnimSpeed(pPlayerModel->Get_RootAnimIndex(), vSpeeds.x);

	ImGui::TextColored(ImVec4(0.0f, 1.f, 0.f, 1.f), "Upper Anim Speed: %.2f", vSpeeds.y);
	ImGui::DragFloat("UpperAnim", &vSpeeds.y, 0.05f, 0, 0, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	pPlayerModel->Set_UpperAnimSpeed(pPlayerModel->Get_UpperAnimIndex(), vSpeeds.y);

	ImGui::TextColored(ImVec4(0.0f, 0.f, 1.f, 1.f), "Bottom Anim Speed: %.2f", vSpeeds.z);
	ImGui::DragFloat("BottomAnim", &vSpeeds.z, 0.05f, 0, 0, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	pPlayerModel->Set_BottomAnimSpeed(pPlayerModel->Get_BottomAnimIndex(), vSpeeds.z);

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0.f, 128.f / 255.f, 1.f), "****AnimBlendSpeed****");
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), "Root AnimBlendSpeed: %.2f", vBlendSpeeds.x);

	ImGui::DragFloat("RootBlendAnim", &vBlendSpeeds.x, 0.05f, 0, 0, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	pPlayerModel->Set_RootAnimBlendSpeed(pPlayerModel->Get_RootAnimIndex(), vBlendSpeeds.x);

	ImGui::TextColored(ImVec4(0.0f, 1.f, 0.f, 1.f), "Upper AnimBlendSpeed: %.2f", vBlendSpeeds.y);
	ImGui::DragFloat("UpperBlendAnim", &vBlendSpeeds.y, 0.05f, 0, 0, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	pPlayerModel->Set_UpperAnimBlendSpeed(pPlayerModel->Get_UpperAnimIndex(), vBlendSpeeds.y);

	ImGui::TextColored(ImVec4(0.0f, 0.f, 1.f, 1.f), "Bottom AnimBlendSpeed: %.2f", vBlendSpeeds.z);
	ImGui::DragFloat("BottomBlendAnim", &vBlendSpeeds.z, 0.05f, 0, 0, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	pPlayerModel->Set_BottomAnimBlendSpeed(pPlayerModel->Get_BottomAnimIndex(), vBlendSpeeds.z);

	ImGui::Separator();



}

void CImGUIManager::Render_PlayerState()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* vPlayerPos = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	CNavigation* pPlayerNavi = (CNavigation*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation"), 0);
	_float4 vRight = STOREFLOAT4(vPlayerPos->Get_State(CTransform::STATE_RIGHT));
	_float4 vUp = STOREFLOAT4(vPlayerPos->Get_State(CTransform::STATE_UP));
	_float4 vLook = STOREFLOAT4(vPlayerPos->Get_State(CTransform::STATE_LOOK));
	_float4 vPos = STOREFLOAT4(vPlayerPos->Get_State(CTransform::STATE_POSITION));
	_int	iCurrentNaviIndex = 0;
	RELEASE_INSTANCE(CGameInstance);

	IMGUI_MARKER("Player/Matrix");
	ImGui::TextColored(ImVec4(255.0f / 255.f, 209.f / 255.f, 179.f / 255.f, 1.0f), u8"Drag/클릭 : 값변경");
	ImGui::Separator();
	ImGui::TextColored(ImVec4(162.0f / 255.f, 162.f / 255.f, 225.f / 255.f, 1.0f), u8"드래그 스피드");
	ImGui::InputFloat("##Speed", &m_fScrollSpeed);
	ImGui::Separator();
	ImGui::TextColored(ImVec4(255.0f / 255.f, 127.f / 255.f, 39.f / 255.f, 1.0f), u8"플레이어 Matrix");
	ImGui::DragFloat4("Right", &vRight.x, m_fScrollSpeed);
	ImGui::DragFloat4("Up", &vUp.x, m_fScrollSpeed);
	ImGui::DragFloat4("Look", &vLook.x, m_fScrollSpeed);
	ImGui::DragFloat4("Pos", &vPos.x, m_fScrollSpeed);

	vPlayerPos->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&vRight));
	vPlayerPos->Set_State(CTransform::STATE_UP, XMLoadFloat4(&vUp));
	vPlayerPos->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&vLook));
	vPlayerPos->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	ImGui::Separator();

	ImGui::TextColored(ImVec4(255.0f / 255.f, 209.f / 255.f, 179.f / 255.f, 1.0f), u8"플레이어 IsGround");
	ImGui::SameLine();
	if (((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_IsGround())
		ImGui::Text(u8"땅에 충돌중");
	else
		ImGui::Text(u8"땅에 충돌중 아님");

	if (ImGui::TreeNode("Navigation"))
	{
		ImGui::TextColored(ImVec4(255.0f / 255.f, 209.f / 255.f, 179.f / 255.f, 1.0f), u8"현재 플레이어 Navigation: %d", iCurrentNaviIndex = pPlayerNavi->Get_CurrentIndex());
		ImGui::Checkbox(u8"네비게이션 이동", &m_isChangeNavigationData);

		if (m_isChangeNavigationData)
		{
			ImGui::SameLine();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::InputInt("##NaviIndex", &iCurrentNaviIndex);
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), u8"네비게이션 이동 함수 추가예정");
		}
		ImGui::TreePop();
	}

}

void CImGUIManager::Render_Layer_Hierarchy()
{
	Render_SelectScene();
	if (ImGui::TreeNode("Layer_List"))
	{
		IMGUI_MARKER("Hierarchy/Layer_List");
		//ImGui::BeginListBox("ListBox");
		if (LayerNameVector.size() > 0)
		{
			m_isLayerSelectChange = ImGui::ListBox("##Layer_listbox", &m_iLayerSelectIndex, &LayerNameVector.front(), (_int)LayerNameVector.size(), 5);
			Render_ListOfLayerObjects();
		}
		else
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), u8"현재씬에 생성된 Layer 없음");


		//ImGui::ListBox("##Layer_List", &m_iLayerSelectIndex, items, IM_ARRAYSIZE(items), 4);
		ImGui::TreePop();
	}
}

void CImGUIManager::Render_SelectScene()
{
	_bool isClick = false;
	_bool isClick2 = false;
	IMGUI_MARKER("Hierarchy/Buttons (Colored)");
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
	if (isClick = ImGui::Button(u8"STATIC SCENE"))
	{
		m_isLayerSceneIndexStatic = true;

	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SameLine();

	ImGui::PushID(1);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
	if (isClick2 = ImGui::Button(u8"현재 SCENE"))
	{
		m_isLayerSceneIndexStatic = false;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::Separator();
	if (m_isLayerSceneIndexStatic)
	{
		m_eLayerSceneIndex = SCENE_STATIC;
	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_eLayerSceneIndex = (SCENE)pGameInstance->Get_CurrentSceneIndex();
		RELEASE_INSTANCE(CGameInstance);
	}
	const char* _szBuff;
	_szBuff = ToString(m_eLayerSceneIndex);
	ImGui::BulletText(u8"List Scene index : "); ImGui::SameLine();
	ImGui::Text(_szBuff);

	if (isClick || isClick2)
	{
		m_iLayerSelectIndex = 0;
		m_iObjectListSelectIndex = 0;
		Render_ListOfLayerName();
	}

	ImGui::Separator();
}

void CImGUIManager::Render_ListOfLayerName()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_iMapSize = pGameInstance->Get_LayerMapSize(m_eLayerSceneIndex);
	if (LayerNameVector.size() > 0)
	{
		for (auto& pLayerTag : LayerNameVector)
		{
			Safe_Delete(pLayerTag);
		}
		LayerNameVector.clear();
	}
	if (m_iMapSize <= 0)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	for (_uint i = 0; i < m_iMapSize; ++i)
	{
		const _tchar* strName = pGameInstance->Get_LayerName(m_eLayerSceneIndex, i);
		int strSize = WideCharToMultiByte(CP_ACP, 0, strName, -1, NULL, 0, NULL, NULL);
		char* szName = new char[strSize];
		WideCharToMultiByte(CP_ACP, 0, strName, -1, szName, strSize, NULL, NULL);
		LayerNameVector.push_back(szName);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImGUIManager::Render_ListOfLayerObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	const _tchar* pLayerName = pGameInstance->Get_LayerName(m_eLayerSceneIndex, m_iLayerSelectIndex);
	_uint iSize = pGameInstance->Get_LayerSize(m_eLayerSceneIndex, pLayerName);

	// 값이 변했을 때만 레이어 변환출력
	if (m_isLayerSelectChange || m_isObjectDestroy)
	{
		if (m_ObjectVector.size() > 0)
		{
			for (auto& pObject : m_ObjectVector)
			{
				Safe_Delete(pObject);
			}
			m_ObjectVector.clear();
		}

		m_isObjectDestroy = false;
		if (iSize <= 0)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (_uint i = 0; i < iSize; ++i)
		{
			CGameObject* pObject = pGameInstance->Get_GameObject(m_eLayerSceneIndex, pLayerName, i);
			const _tchar* strName = pObject->Get_LayerName();
			int strSize = WideCharToMultiByte(CP_ACP, 0, strName, -1, NULL, 0, NULL, NULL);
			char* szName = new char[strSize];
			WideCharToMultiByte(CP_ACP, 0, strName, -1, szName, strSize, NULL, NULL);
			m_ObjectVector.push_back(szName);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	ImGui::Text(u8"클릭한 레이어");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), LayerNameVector[m_iLayerSelectIndex]);
	if (ImGui::TreeNode("Layer_ObjectList"))
	{
		ImGui::Text(u8"오브젝트 리스트 사이즈 : %d", iSize);
		if (m_ObjectVector.size() > 0)
		{
			m_isObjectListSelectChange = ImGui::ListBox("##Object_listbox", &m_iObjectListSelectIndex, &m_ObjectVector.front(), (_int)m_ObjectVector.size(), 5);
		}
		else
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), u8"레이어에 오브젝트가 없음");


		if (m_ObjectVector.size() > m_iObjectListSelectIndex)
		{
			Destroy_CloneObject(pLayerName);
		}
		ImGui::TreePop();

	}
}

void CImGUIManager::Destroy_CloneObject(const _tchar* pLayerTag)
{
	_bool isClicked = ImGui::Button(u8"Delete Object");

	if (isClicked)
	{
		if (IDOK == MessageBox(g_hWnd, L"Really Delete Selected Object?", L"REALLY?", MB_OKCANCEL))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Get_GameObject(m_eLayerSceneIndex, pLayerTag, m_iObjectListSelectIndex)->Set_IsDestroy(true);
			m_isObjectDestroy = true;
			RELEASE_INSTANCE(CGameInstance);

		}
	}
}

void CImGUIManager::Render_Shader_Default()
{

	ImGui::Separator();


	if (ImGui::TreeNode("Shader"))
	{
		IMGUI_MARKER("Shader/Rimlight");

		if (ImGui::TreeNode("Rimlight"))
		{
			ImGui::Checkbox(u8"림라이트 On/Off", &m_isRenderLimRight);

			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT, m_isRenderLimRight);
			if (m_isRenderLimRight)
			{
				_float fPower = m_pRenderer->Get_RimLightPower();
				UpdateColor(m_pRenderer->Get_RimLightColorFloat4());
				ImGui::ColorEdit4("RimLightColor", m_RimLightColorArr);
				UpdateColor(m_RimLightColorArr);

				ImGui::DragFloat("Power", &fPower, 0.005f);
				ImGui::DragFloat("Rim", &m_vRimRatio.x, 0.005f);
				ImGui::DragFloat("Diffuse", &m_vRimRatio.y, 0.005f);
				ImGui::DragFloat("Width", &m_fRimWidth, 0.005f);

				m_pRenderer->Set_RimLightColorPower(fPower);
				m_pRenderer->Set_RimRatio(m_vRimRatio);
				m_pRenderer->Set_RimWidth(m_fRimWidth);
			}
			ImGui::TreePop();

		}

		//IMGUI_MARKER("Shader/HDR");

		//if (ImGui::TreeNode("HDR"))
		//{
		//
		//	ImGui::Checkbox(u8"HDR On/Off", &m_IsRenderHDR);
		//	m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_HDR,m_IsRenderHDR);
		//	ImGui::TreePop();

		//}
		IMGUI_MARKER("Shader/Bloom");

		if (ImGui::TreeNode("Bloom"))
		{

			ImGui::Checkbox(u8"Bloom On/Off", &m_IsRenderBloom);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_BLOOM, m_IsRenderBloom);

			IMGUI_MARKER("Shader/Bloom/DragFloat");


			ImGui::DragFloat("Diffuse", &m_BloomWeight[0], 0.005f);
			ImGui::DragFloat("BloomColor", &m_BloomWeight[1], 0.005f);
			ImGui::DragFloat("BlurColor_2", &m_BloomWeight[2], 0.005f);
			ImGui::DragFloat("BlurColor_4", &m_BloomWeight[3], 0.005f);

			m_pRenderer->Set_BloomWeight(m_BloomWeight);


			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/EffectBloom");

		if (ImGui::TreeNode("EffectBloom"))
		{

			ImGui::Checkbox(u8"EffectBloom On/Off", &m_IsRenderBloomEffect);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_BLOOMEFFECT, m_IsRenderBloomEffect);

			IMGUI_MARKER("Shader/EffectBloom/DragFloat");


			ImGui::DragFloat("Diffuse", &m_BloomEffectWeight[0], 0.005f);
			ImGui::DragFloat("BloomColor", &m_BloomEffectWeight[1], 0.005f);
			ImGui::DragFloat("BlurColor_2", &m_BloomEffectWeight[2], 0.005f);
			ImGui::DragFloat("BlurColor_4", &m_BloomEffectWeight[3], 0.005f);

			m_pRenderer->Set_BloomEffectWeight(m_BloomEffectWeight);


			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/DOF");

		if (ImGui::TreeNode("DOF"))
		{

			ImGui::Checkbox(u8"DOF On/Off", &m_IsRenderDOF);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_DOP, m_IsRenderDOF);

			IMGUI_MARKER("Shader/DOF/DragFloat");


			ImGui::DragFloat("NearStart", &m_DOFWeight[0], 0.005f);
			ImGui::DragFloat("NearEnd", &m_DOFWeight[1], 0.005f);
			ImGui::DragFloat("FarStart", &m_DOFWeight[2], 0.005f);
			ImGui::DragFloat("FarEnd", &m_DOFWeight[3], 0.005f);

			m_pRenderer->Set_DOFWeight(m_DOFWeight);

			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Emissive");

		if (ImGui::TreeNode("Emissive"))
		{

			ImGui::Checkbox(u8"Emissive On/Off", &m_IsRenderEmissive);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_EMISSIVE, m_IsRenderEmissive);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Shadow");

		if (ImGui::TreeNode("Shadow"))
		{

			ImGui::Checkbox(u8"Shadow On/Off", &m_IsRenderShadow);
			ImGui::DragFloat("Bias", &m_fShadowBias, 0.005f);
			ImGui::DragFloat("PercentLimitMax", &m_fPercentLimitMax, 0.005f);
			_float fShadowBias = m_fShadowBias * 0.0001f;

			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_SHADOW, m_IsRenderShadow);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Set_ShadowValue(fShadowBias, m_fPercentLimitMax);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/MotionBlur");

		if (ImGui::TreeNode("MotionBlur"))
		{

			ImGui::Checkbox(u8"MotionBlur On/Off", &m_IsRenderMotionBlur);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR, m_IsRenderMotionBlur);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Tone");

		if (ImGui::TreeNode("Tone"))
		{
			ImGui::RadioButton("ACES NEW", &m_iToneOption, 0); ImGui::SameLine();
			ImGui::RadioButton("HABLE NEW", &m_iToneOption, 1); ImGui::SameLine();
			ImGui::RadioButton("ACES OLD", &m_iToneOption, 2);
			ImGui::RadioButton("HABLE OLD", &m_iToneOption, 3); ImGui::SameLine();
			ImGui::RadioButton("REIN", &m_iToneOption, 4);

			m_pRenderer->Set_ToneOption((_uint)m_iToneOption);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Blur");

		if (ImGui::TreeNode("Blur"))
		{
			ImGui::RadioButton(u8"가우시안", &m_iBlurOption, 0); ImGui::SameLine();
			ImGui::RadioButton("Normal", &m_iBlurOption, 1); ImGui::SameLine();
			ImGui::RadioButton("Test", &m_iBlurOption, 2);

			m_pRenderer->Set_BlurOption((_uint)m_iBlurOption);
			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/Shade");

		if (ImGui::TreeNode("Shade"))
		{
			ImGui::DragFloat("Ambient", &m_fShadeAmbient, 0.005f);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Set_ShadeAmbient(m_fShadeAmbient);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/LightOffset");

		if (ImGui::TreeNode("LightOffset"))
		{
			ImGui::DragFloat("Offset", &m_fLightOffset, 0.005f);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Set_LightOffset(m_fLightOffset);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Light");

		if (ImGui::TreeNode("Light"))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			IMGUI_MARKER("Shader/Light/Direction");
			if (ImGui::TreeNode("Direction"))
			{
				LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
				ImGui::DragFloat("Dir: X", &pLightDesc->vPosition.x, 0.005f);
				ImGui::DragFloat("Dir: Y", &pLightDesc->vPosition.y, 0.005f);
				ImGui::DragFloat("Dir: Z", &pLightDesc->vPosition.z, 0.005f);

				//LightDesc.iType = tagLightDesc::TYPE_POINT;
				//LightDesc.vPosition = _float4(8.671f, 2.048f, 13.81f, 1.f);
				//LightDesc.fRange = 4.f;
				//LightDesc.vDiffuse = _float4(1.f, 1.0, 1.f, 1.f);
				//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
				//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

				ImGui::TreePop();
			}

			if (SCENE_DEMO == pGameInstance->Get_CurrentSceneIndex())

			{
				IMGUI_MARKER("Shader/Light/Point1");
				if (ImGui::TreeNode("Point1"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(1);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point2");
				if (ImGui::TreeNode("Point2"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(2);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point3");
				if (ImGui::TreeNode("Point3"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(3);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point4");
				if (ImGui::TreeNode("Point4"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(4);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point5");
				if (ImGui::TreeNode("Point5"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(5);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point6");
				if (ImGui::TreeNode("Point6"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(6);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point7");
				if (ImGui::TreeNode("Point7"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(7);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point8");
				if (ImGui::TreeNode("Point8"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(8);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point9");
				if (ImGui::TreeNode("Point9"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(9);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point10");
				if (ImGui::TreeNode("Point10"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(10);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point11");
				if (ImGui::TreeNode("Point11"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(11);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point12");
				if (ImGui::TreeNode("Point12"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(12);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point13");
				if (ImGui::TreeNode("Point13"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(13);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point14");
				if (ImGui::TreeNode("Point14"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(14);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point15");
				if (ImGui::TreeNode("Point15"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(15);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point16");
				if (ImGui::TreeNode("Point16"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(16);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point17");
				if (ImGui::TreeNode("Point17"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(17);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point18");
				if (ImGui::TreeNode("Point18"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(18);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point19");
				if (ImGui::TreeNode("Point19"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(19);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point20");
				if (ImGui::TreeNode("Point20"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(20);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point21");
				if (ImGui::TreeNode("Point21"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(21);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point22");
				if (ImGui::TreeNode("Point22"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(22);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point23");
				if (ImGui::TreeNode("Point23"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(23);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point24");
				if (ImGui::TreeNode("Point24"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(24);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point25");
				if (ImGui::TreeNode("Point25"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(25);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point26");
				if (ImGui::TreeNode("Point26"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(26);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point27");
				if (ImGui::TreeNode("Point27"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(27);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point28");
				if (ImGui::TreeNode("Point28"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(28);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point29");
				if (ImGui::TreeNode("Point29"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(29);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point30");
				if (ImGui::TreeNode("Point30"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(30);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point31");
				if (ImGui::TreeNode("Point31"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(31);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point32");
				if (ImGui::TreeNode("Point32"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(32);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point33");
				if (ImGui::TreeNode("Point33"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(33);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point34");
				if (ImGui::TreeNode("Point34"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(34);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point35");
				if (ImGui::TreeNode("Point35"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(35);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point36");
				if (ImGui::TreeNode("Point36"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(36);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point37");
				if (ImGui::TreeNode("Point37"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(37);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point38");
				if (ImGui::TreeNode("Point38"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(38);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point39");
				if (ImGui::TreeNode("Point39"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(39);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point40");
				if (ImGui::TreeNode("Point40"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(40);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point41");
				if (ImGui::TreeNode("Point41"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(41);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point42");
				if (ImGui::TreeNode("Point42"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(42);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point43");
				if (ImGui::TreeNode("Point43"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(43);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point44");
				if (ImGui::TreeNode("Point44"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(44);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point45");
				if (ImGui::TreeNode("Point45"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(45);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point46");
				if (ImGui::TreeNode("Point46"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(46);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

			}
			else if (SCENE_STAGE01 == pGameInstance->Get_CurrentSceneIndex())
			{
				IMGUI_MARKER("Shader/Light/Point1");
				if (ImGui::TreeNode("Point1"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(1);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point2");
				if (ImGui::TreeNode("Point2"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(2);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point3");
				if (ImGui::TreeNode("Point3"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(3);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point4");
				if (ImGui::TreeNode("Point4"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(4);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point5");
				if (ImGui::TreeNode("Point5"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(5);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point6");
				if (ImGui::TreeNode("Point6"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(6);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point7");
				if (ImGui::TreeNode("Point7"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(7);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point8");
				if (ImGui::TreeNode("Point8"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(8);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point9");
				if (ImGui::TreeNode("Point9"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(9);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);
					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point10");
				if (ImGui::TreeNode("Point10"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(10);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point11");
				if (ImGui::TreeNode("Point11"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(11);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point12");
				if (ImGui::TreeNode("Point12"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(12);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point13");
				if (ImGui::TreeNode("Point13"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(13);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point14");
				if (ImGui::TreeNode("Point14"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(14);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point15");
				if (ImGui::TreeNode("Point15"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(15);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point16");
				if (ImGui::TreeNode("Point16"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(16);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
			}
			else if (SCENE_STAGE02 == pGameInstance->Get_CurrentSceneIndex())
			{
				IMGUI_MARKER("Shader/Light/Point1");
				if (ImGui::TreeNode("Point1"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(1);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point2");
				if (ImGui::TreeNode("Point2"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(2);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point3");
				if (ImGui::TreeNode("Point3"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(3);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point4");
				if (ImGui::TreeNode("Point4"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(4);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point5");
				if (ImGui::TreeNode("Point5"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(5);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
			}
			else if (SCENE_STAGE03 == pGameInstance->Get_CurrentSceneIndex())

			{
				IMGUI_MARKER("Shader/Light/Point1");
				if (ImGui::TreeNode("Point1"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(1);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);


					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point2");
				if (ImGui::TreeNode("Point2"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(2);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point3");
				if (ImGui::TreeNode("Point3"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(3);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point4");
				if (ImGui::TreeNode("Point4"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(4);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point5");
				if (ImGui::TreeNode("Point5"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(5);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point6");
				if (ImGui::TreeNode("Point6"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(6);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point7");
				if (ImGui::TreeNode("Point7"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(7);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point8");
				if (ImGui::TreeNode("Point8"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(8);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point9");
				if (ImGui::TreeNode("Point9"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(9);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point10");
				if (ImGui::TreeNode("Point10"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(10);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point11");
				if (ImGui::TreeNode("Point11"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(11);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point12");
				if (ImGui::TreeNode("Point12"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(12);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point13");
				if (ImGui::TreeNode("Point13"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(13);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point14");
				if (ImGui::TreeNode("Point14"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(14);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point15");
				if (ImGui::TreeNode("Point15"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(15);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point16");
				if (ImGui::TreeNode("Point16"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(16);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point17");
				if (ImGui::TreeNode("Point17"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(17);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
				IMGUI_MARKER("Shader/Light/Point18");
				if (ImGui::TreeNode("Point18"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(18);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point19");
				if (ImGui::TreeNode("Point19"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(19);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point20");
				if (ImGui::TreeNode("Point20"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(20);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point21");
				if (ImGui::TreeNode("Point21"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(21);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point22");
				if (ImGui::TreeNode("Point22"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(22);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point23");
				if (ImGui::TreeNode("Point23"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(23);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point24");
				if (ImGui::TreeNode("Point24"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(24);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
			}
			else if (SCENE_STAGE04 == pGameInstance->Get_CurrentSceneIndex())
			{
				IMGUI_MARKER("Shader/Light/Point1");
				if (ImGui::TreeNode("Point1"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(1);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point2");
				if (ImGui::TreeNode("Point2"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(2);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point3");
				if (ImGui::TreeNode("Point3"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(3);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point4");
				if (ImGui::TreeNode("Point4"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(4);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point5");
				if (ImGui::TreeNode("Point5"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(5);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
			}
			else if (SCENE_STAGE05 == pGameInstance->Get_CurrentSceneIndex())
			{
				IMGUI_MARKER("Shader/Light/Point1");
				if (ImGui::TreeNode("Point1"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(1);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point2");
				if (ImGui::TreeNode("Point2"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(2);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point3");
				if (ImGui::TreeNode("Point3"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(3);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point4");
				if (ImGui::TreeNode("Point4"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(4);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point5");
				if (ImGui::TreeNode("Point5"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(5);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}

				IMGUI_MARKER("Shader/Light/Point6");
				if (ImGui::TreeNode("Point6"))
				{
					LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(6);
					ImGui::DragFloat("Pos: X", &pLightDesc->vPosition.x, 0.005f);
					ImGui::DragFloat("Pos: Y", &pLightDesc->vPosition.y, 0.005f);
					ImGui::DragFloat("Pos: Z", &pLightDesc->vPosition.z, 0.005f);
					ImGui::DragFloat("Range", &pLightDesc->fRange, 0.005f);
					ImGui::DragFloat("Power", &pLightDesc->fPower, 0.005f);
					ImGui::DragFloat("Color : x", &pLightDesc->vDiffuse.x, 0.005f);
					ImGui::DragFloat("Color : y", &pLightDesc->vDiffuse.y, 0.005f);
					ImGui::DragFloat("Color : z", &pLightDesc->vDiffuse.z, 0.005f);

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
			RELEASE_INSTANCE(CGameInstance);

		}

		IMGUI_MARKER("Shader/AO");

		if (ImGui::TreeNode("AO"))
		{
			ImGui::Checkbox(u8"AO On/Off", &m_bAO);
			ImGui::DragFloat("Range", &m_fSSAORadious, 0.005f);
			ImGui::DragFloat("Bias", &m_fBias, 0.005f);

			_float fTempRad = m_fSSAORadious*0.001f;
			_float fTempBias = m_fBias*0.0001f;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_SSAO, m_bAO);
			pGameInstance->Set_IsAO(m_bAO);
			RELEASE_INSTANCE(CGameInstance);
			m_pRenderer->Set_SSAOValue(fTempRad, fTempBias);
			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/Reflect");

		if (ImGui::TreeNode("Reflect"))
		{
			ImGui::DragFloat("Reflect", &m_fReflectHeight, 0.005f);


			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			_uint iSize = pGameInstance->Get_LayerSize(SCENE_STAGE04, TEXT("Layer_Compt_Floor"));
			CStatic_Mesh* pStaticMesh = dynamic_cast<CStatic_Mesh*>(pGameInstance->Get_GameObject(SCENE_STAGE04, TEXT("Layer_Compt_Floor"), 0));
			if (nullptr != pStaticMesh)
			{
				pStaticMesh->Set_Reflect_Height(m_fReflectHeight);
			}
			RELEASE_INSTANCE(CGameInstance);

			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/LUT");

		if (ImGui::TreeNode("LUT"))
		{
			ImGui::Checkbox(u8"LUT On/Off", &m_bLut);
			// Arrow buttons with Repeater
			IMGUI_MARKER("Shader/LUT/Buttons (Repeating)");

			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { m_iLut_Count--; }
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { m_iLut_Count++; }
			if (59 <= m_iLut_Count)
				m_iLut_Count = 0;

			ImGui::PopButtonRepeat();
			ImGui::SameLine();
			ImGui::Text("%d", m_iLut_Count);

			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_LUT, m_bLut);
			m_pRenderer->Set_LUT_Index(m_iLut_Count);




			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/Fog");

		if (ImGui::TreeNode("Fog"))
		{
			//_float4 vFogcolor = _float4(0.3019f, 0.4f, 0.5019f, 1.f);
			//_float fFogstart = 40.f;
			//_float fFogdensity = 0.03f;
			//_float fFogfalloff = 0.001f;
			ImGui::DragFloat("Fogstart", &m_fFog_Values[0], 0.005f);
			ImGui::DragFloat("Fogdensity", &m_fFog_Values[1], 0.005f);
			ImGui::DragFloat("Fogfalloff", &m_fFog_Values[2], 0.005f);

			ImGui::DragFloat("Fogcolor_x", &m_vFog_Color.x, 0.005f);
			ImGui::DragFloat("Fogcolor_y", &m_vFog_Color.y, 0.005f);
			ImGui::DragFloat("Fogcolor_z", &m_vFog_Color.z, 0.005f);

			ImGui::Checkbox(u8"Height", &m_bFog_Height);

			m_pRenderer->Set_Fog(m_fFog_Values, m_vFog_Color, m_bFog_Height);
			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/MSAA");

		if (ImGui::TreeNode("MSAA"))
		{
			ImGui::DragFloat("Offset", &m_fMsaaOffset, 0.005f);
			// Arrow buttons with Repeater
			IMGUI_MARKER("Shader/Dissolve/Buttons (Repeating)");
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { m_iMassSampler--; }
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { m_iMassSampler++; }
			if (3 == m_iMassSampler || -1 == m_iMassSampler)
				m_iDissolveTextureIndex = 0;
			ImGui::PopButtonRepeat();
			ImGui::SameLine();
			ImGui::Text("%d", m_iMassSampler);

			m_pRenderer->Set_MSAA(m_fMsaaOffset, m_iMassSampler);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/SSAA");

		if (ImGui::TreeNode("SSAA"))
		{
			ImGui::Checkbox(u8"SSAA On/Off", &m_bIsSSAA);


			m_pRenderer->Set_IsSSAA(m_bIsSSAA);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Radial");

		if (ImGui::TreeNode("Radial"))
		{
			ImGui::Checkbox(u8"Radial On/Off", &m_IsRenderRadial);

			IMGUI_MARKER("Shader/Radial/Buttons (Repeating)");
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { m_iRadialCnt--; }
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { m_iRadialCnt++; }
			if (15 == m_iRadialCnt || -1 == m_iRadialCnt)
				m_iRadialCnt = 0;
			ImGui::PopButtonRepeat();
			ImGui::SameLine();
			ImGui::Text("%d", m_iRadialCnt);
			ImGui::DragFloat("power", &m_fRadialBlurPower, 0.005f);
			ImGui::DragFloat("dir_x", &m_vRadialDir.x, 0.005f);
			ImGui::DragFloat("dir_y", &m_vRadialDir.y, 0.005f);

			m_pRenderer->Set_Radial(m_iRadialCnt, m_fRadialBlurPower, m_vRadialDir);
			m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RADIAL, m_IsRenderRadial);
			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/RectMove");

		if (ImGui::TreeNode("RectMove"))
		{

			ImGui::DragFloat("x", &m_vScale[0].x, 0.005f);
			ImGui::DragFloat("y", &m_vScale[0].y, 0.005f);
			ImGui::DragFloat("z", &m_vScale[0].z, 0.005f);

			ImGui::DragFloat("PosX", &m_vPos[0].x, 0.005f);
			ImGui::DragFloat("PosY", &m_vPos[0].y, 0.005f);
			ImGui::DragFloat("PosZ", &m_vPos[0].z, 0.005f);

			ImGui::DragFloat("PosX_Left", &m_vPos[1].x, 0.005f);
			ImGui::DragFloat("PosY_Left", &m_vPos[1].y, 0.005f);
			ImGui::DragFloat("PosZ_Left", &m_vPos[1].z, 0.005f);
			ImGui::DragFloat("Rad_Left", &m_fRad[0], 0.005f);

			ImGui::DragFloat("PosX_Right", &m_vPos[2].x, 0.005f);
			ImGui::DragFloat("PosY_Right", &m_vPos[2].y, 0.005f);
			ImGui::DragFloat("PosZ_Right", &m_vPos[2].z, 0.005f);
			ImGui::DragFloat("Rad_Right", &m_fRad[1], 0.005f);

			ImGui::DragFloat("SourSpeed0", &m_fSourSpeed[0], 0.005f);
			ImGui::DragFloat("DestSpeed0", &m_fDestSpeed[0], 0.005f);
			ImGui::DragFloat("MaxTime0", &m_fMaxTime[0], 0.005f);

			ImGui::DragFloat("SourSpeed1", &m_fSourSpeed[1], 0.005f);
			ImGui::DragFloat("DestSpeed1", &m_fDestSpeed[1], 0.005f);
			ImGui::DragFloat("MaxTime1", &m_fMaxTime[1], 0.005f);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CDemoRect* pDemoRect = dynamic_cast<CDemoRect*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DemoRect"), 0));
			if (nullptr == pDemoRect)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}
			pDemoRect->Set_Scaling(m_vScale[0]);
			pDemoRect->Set_Pos(m_vPos[0]);
			pDemoRect->Set_AllSpeed(m_fSourSpeed, m_fDestSpeed);
			pDemoRect->Set_MaxTime(m_fMaxTime);

			COfficeWalls* pOfficeWall_Left = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 0));
			if (nullptr == pOfficeWall_Left)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}
			pOfficeWall_Left->Set_Scaling(m_vScale[1]);
			pOfficeWall_Left->Set_Pos(m_vPos[1]);
			pOfficeWall_Left->Set_Rad(m_fRad[0]);
			pOfficeWall_Left->Set_AllSpeed(m_fSourSpeed, m_fDestSpeed);
			pOfficeWall_Left->Set_MaxTime(m_fMaxTime);

			COfficeWalls* pOfficeWall_Right = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 1));
			if (nullptr == pOfficeWall_Right)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}
			pOfficeWall_Right->Set_Scaling(m_vScale[2]);
			pOfficeWall_Right->Set_Pos(m_vPos[2]);
			pOfficeWall_Right->Set_Rad(m_fRad[1]);
			pOfficeWall_Right->Set_AllSpeed(m_fSourSpeed, m_fDestSpeed);
			pOfficeWall_Right->Set_MaxTime(m_fMaxTime);


			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();
		}
		IMGUI_MARKER("Shader/CameraBias");

		if (ImGui::TreeNode("CameraBias"))
		{
			ImGui::DragFloat("CameraBias", &m_fCameraBias, 0.005f);
			ImGui::DragFloat("CameraBias_Down", &m_fCameraBias_Down, 0.005f);
			ImGui::DragFloat("Adjust_x", &m_vCameraFixAdjustPos.x, 1.f);
			ImGui::DragFloat("Adjust_y", &m_vCameraFixAdjustPos.y, 1.f);
			ImGui::DragFloat("Move_x", &m_vCameraMouseMoveSpeed.x, 0.005f);
			ImGui::DragFloat("Move_y", &m_vCameraMouseMoveSpeed.y, 0.005f);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Bias(m_fCameraBias, m_fCameraBias_Down);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_FixAdjustPos(m_vCameraFixAdjustPos);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_MouseMoveSpeed(m_vCameraMouseMoveSpeed);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}


		IMGUI_MARKER("Shader/OffsetRGB_Player");

		if (ImGui::TreeNode("OffsetRGB_Player"))
		{
			ImGui::DragFloat("OffsetRGB_Player_x", &m_vOffsetRGB_Player.x, 0.005f);
			ImGui::DragFloat("OffsetRGB_Player_y", &m_vOffsetRGB_Player.y, 0.005f);
			ImGui::DragFloat("OffsetRGB_Player_z", &m_vOffsetRGB_Player.z, 0.005f);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_OffsetRGB(m_vOffsetRGB_Player);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/OffsetRGB");

		if (ImGui::TreeNode("OffsetRGB"))
		{
			ImGui::DragFloat("OffsetRGB_Wall_x", &m_vOffsetRGB_Wall.x, 0.005f);
			ImGui::DragFloat("OffsetRGB_Wall_y", &m_vOffsetRGB_Wall.y, 0.005f);
			ImGui::DragFloat("OffsetRGB_Wall_z", &m_vOffsetRGB_Wall.z, 0.005f);
			ImGui::DragFloat("OffsetRGB_Floor_x", &m_vOffsetRGB_Floor.x, 0.005f);
			ImGui::DragFloat("OffsetRGB_Floor_y", &m_vOffsetRGB_Floor.y, 0.005f);
			ImGui::DragFloat("OffsetRGB_Floor_z", &m_vOffsetRGB_Floor.z, 0.005f);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			//if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeFloor0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeFloor1"))
			//	|| !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall1")) ||
			//	!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall3")) ||
			//	!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall4")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall5")) ||
			//	!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall6")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall7")) ||
			//	!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall8")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall9")) ||
			//	!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall10")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall11")) ||
			//	!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall12")))
			//{

			//}
			_uint iSize = pGameInstance->Get_LayerSize(SCENE_DEMO, TEXT("Layer_OfficeWall"));
			if (0 >= iSize)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;

			}
			for (_uint i = 0; i < 30; ++i)
			{
				((CStatic_Mesh*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWall"), i))->Set_OffsetRGB(m_vOffsetRGB_Wall, m_vOffsetRGB_Floor);
			}

			iSize = pGameInstance->Get_LayerSize(SCENE_DEMO, TEXT("Layer_FloorOffice"));
			if (0 >= iSize)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}
			for (_uint i = 0; i < iSize; ++i)
			{
				((CStatic_Mesh*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_FloorOffice"), i))->Set_OffsetRGB(m_vOffsetRGB_Wall, m_vOffsetRGB_Floor);
			}

			iSize = pGameInstance->Get_LayerSize(SCENE_DEMO, TEXT("Layer_OfficeFloor"));
			if (0 >= iSize)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}
			for (_uint i = 0; i < iSize; ++i)
			{
				((CStatic_Mesh*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeFloor"), i))->Set_OffsetRGB(m_vOffsetRGB_Wall, m_vOffsetRGB_Floor);
			}


			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}

		IMGUI_MARKER("Shader/Dissolve");
		if (ImGui::TreeNode("Dissolve"))
		{

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			ImGui::Checkbox("Start", &m_bDissolve);
			ImGui::DragFloat("Speed", &m_fDissolveSpeed, 0.005f);
			ImGui::DragFloat("DissolveStart", &m_fDissolveStart, 0.005f);
			ImGui::DragFloat("ScaleSpeed", &m_fDissolveScaleSpeed, 0.005f);

			// Arrow buttons with Repeater
			IMGUI_MARKER("Shader/Dissolve/Buttons (Repeating)");
			if (20 == m_iDissolveTextureIndex)
				m_iDissolveTextureIndex = 0;
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { m_iDissolveTextureIndex--; }
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { m_iDissolveTextureIndex++; }
			ImGui::PopButtonRepeat();
			ImGui::SameLine();
			ImGui::Text("%d", m_iDissolveTextureIndex);

			CVentDoor* pVentDoor = dynamic_cast<CVentDoor*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_VentDoor"), 0));
			if (nullptr == pVentDoor)
			{
				RELEASE_INSTANCE(CGameInstance);
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}
			pVentDoor->Set_Dissolve(m_bDissolve, (_double)m_fDissolveSpeed, m_iDissolveTextureIndex, m_fDissolveStart, m_fDissolveScaleSpeed);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}
		IMGUI_MARKER("Shader/Rail");

		if (ImGui::TreeNode("Rail"))
		{
			//m_dStartSpeed = dStartSpeed;
			//m_dDefaultSpeed = dDefaultSpeed;
			//m_dFastSpeed = dFastSpeed;
			//m_dSlowSpeed = dSlowSpeed;
			//m_dNonFixJumpPower = dNonFixJumpPower;
			//m_dRailCollisionCheckTerm = dRailCollisionCheckTerm;
			//m_dJumpCorrection = dJumpCorrection;
			//m_dStartJumpAccMax = dStartJumpAccMax;
			//m_dRayTrans = dRayTrans;
			//m_fJumpPower = dJumpPower;
			//m_lfFallSpeed = dJumpFallSpeed;
			//m_dLerpLook_Speed = dLookSpeed;
			//m_dLerpSpeed_Speed = dMoveSpeed;
			//m_dLerpAdjust_Speed = dAdjustSpeed;

			ImGui::DragFloat("StartSpeed", &m_fStartSpeed, 0.005f);
			ImGui::DragFloat("DefaultSpeed", &m_fDefaultSpeed, 0.005f);
			ImGui::DragFloat("FastSpeed", &m_fFastSpeed, 0.005f);
			ImGui::DragFloat("SlowSpeed", &m_fSlowSpeed, 0.005f);
			ImGui::DragFloat("NonFixJumpPower", &m_fNonFixJumpPower, 0.005f);
			ImGui::DragFloat("RailCollisionCheckTerm", &m_fRailCollisionCheckTerm, 0.005f);
			ImGui::DragFloat("JumpCorrection", &m_fJumpCorrection, 0.005f);
			ImGui::DragFloat("StartJumpAccMax", &m_fStartJumpAccMax, 0.005f);
			ImGui::DragFloat("RayTrans", &m_fRayTrans, 0.005f);
			ImGui::DragFloat("JumpPower", &m_fJumpPower, 0.005f);
			ImGui::DragFloat("FallSpeed", &m_fFallSpeed, 0.005f);
			ImGui::DragFloat("LookSpeed", &m_fLookSpeed, 0.005f);
			ImGui::DragFloat("MoveSpeed", &m_fMoveSpeed, 0.005f);
			ImGui::DragFloat("AdjustSpeed", &m_fAdjustSpeed, 0.005f);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_RailValues((_float)m_fStartSpeed, (_float)m_fDefaultSpeed,
				(_float)m_fFastSpeed, (_float)m_fSlowSpeed, (_float)m_fNonFixJumpPower, (_float)m_fRailCollisionCheckTerm, (_float)m_fJumpCorrection,
				(_float)m_fStartJumpAccMax, (_float)m_fRayTrans, (_float)m_fJumpPower,
				(_float)m_fFallSpeed, (_float)m_fLookSpeed, (_float)m_fMoveSpeed, (_float)m_fAdjustSpeed);
			RELEASE_INSTANCE(CGameInstance);
			ImGui::TreePop();

		}
		ImGui::TreePop();

	}

}

void CImGUIManager::Render_Action_Default()
{
	ImGui::Separator();
	IMGUI_MARKER("Action");
	if (ImGui::TreeNode("Action"))
	{
		ImGui::Checkbox(u8"림라이트 On/Off", &m_isRenderLimRight);

		m_pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT, m_isRenderLimRight);
		if (m_isRenderLimRight)
		{
			_float fPower = m_pRenderer->Get_RimLightPower();
			UpdateColor(m_pRenderer->Get_RimLightColorFloat4());
			ImGui::ColorEdit4("RimLightColor", m_RimLightColorArr);
			UpdateColor(m_RimLightColorArr);

			ImGui::DragFloat("Power", &fPower, 0.005f);
			ImGui::DragFloat("Rim", &m_vRimRatio.x, 0.005f);
			ImGui::DragFloat("Diffuse", &m_vRimRatio.y, 0.005f);
			ImGui::DragFloat("Width", &m_fRimWidth, 0.005f);

			m_pRenderer->Set_RimLightColorPower(fPower);
			m_pRenderer->Set_RimRatio(m_vRimRatio);
			m_pRenderer->Set_RimWidth(m_fRimWidth);
		}
		ImGui::TreePop();
	}
}

void CImGUIManager::UpdateColor(_float4 vColor)
{
	m_RimLightColorArr[0] = vColor.x;
	m_RimLightColorArr[1] = vColor.y;
	m_RimLightColorArr[2] = vColor.z;
	m_RimLightColorArr[3] = vColor.w;
}

void CImGUIManager::UpdateColor(_float * pColor)
{
	m_pRenderer->Set_RimLightColor((_float4)pColor);
}
#endif // _DEBUG