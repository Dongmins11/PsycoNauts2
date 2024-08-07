#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"		

BEGIN(Engine)
class CRenderer;
END
class CImGUIManager final
{
public:
	CImGUIManager();
	~CImGUIManager();

public:
	void Initialize_ImGUI();
	void SettingDevice(ID3D11Device*	pDevice, ID3D11DeviceContext*	pDeviceContext);
	void SettingRenderer(CRenderer* pRenderer);
	void ReleaseRenderer() 
	{ 
		if (nullptr != m_pRenderer)
		{
			Safe_Release(m_pRenderer);
			m_pRenderer = nullptr;
		}
	}
#ifdef _DEBUG
	void Render();
#endif // _DEBUG

	void Set_Frames(_uint iFrame) { m_iFrames = iFrame; }
public:
	static CImGUIManager* Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CImGUIManager;
		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;					
			m_pInstance = nullptr;					
		}											
		
	}


private:
	// For.DebugButtons
	void Render_DebugButtons();

	// Camera & Debug
	void Render_Frame();
	void Render_SceneMove();
	void Render_CameraDebug();

	// Animation
	void Render_Animation();

	// Player
	void Render_PlayerState();

	// Hierarchy
	void Render_Layer_Hierarchy();
	void Render_SelectScene();
	void Render_ListOfLayerName();
	void Render_ListOfLayerObjects();

	void Destroy_CloneObject(const _tchar* pLayerTag);
	void Open_ObjectData(const _tchar* pLayerTag);
	// Shader
	void Render_Shader_Default();
	// Action
	void Render_Action_Default();

private:
	void UpdateColor(_float4 vColor);
	void UpdateColor(_float* pColor);
private:
	static CImGUIManager* m_pInstance;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CRenderer*				m_pRenderer = nullptr;

	_uint m_iSizeOfDebugs = 6;
	_bool m_isOpenDebugs[6] = { false };
	const char* m_isDebugButtonText[6] = { "Camera&Scene", "PlayerAnimation", "PlayerSetting", "Object Hierarchy", "Shader" , "Action" };

	_uint m_iFrames = 0;
	_float4 vPlayerRight = _float4(1.f, 0.f, 0.f, 0.f),vPlayerUp = _float4(0.f, 1.f, 0.f, 0.f), vPlayerLook = _float4(0.f, 0.f, 1.f, 0.f), vPlayerPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float m_fScrollSpeed = 0.05f;
	_bool	m_isChangeNavigationData = false;

	// For.PlayerAnimation
	_float m_fAnimScrollSpeed = 0.05f;



	// For.Hierarchy
	// Static에 있는 레이어인지, 현재 씬에있는 레이어인지 판단하기 위한 변수
	_bool	m_isLayerSceneIndexStatic = true;
	// 레이어 리스트의 클릭 인덱스를 판단하기 위한 변수
	_int	m_iLayerSelectIndex = 0;
	// 클릭한 씬이 어디인지 판단하기 위한 변수
	SCENE	m_eLayerSceneIndex = SCENE_END;
	// 씬의 레이어를 불러오기 위한 레이어 총 갯수
	_uint	m_iMapSize = 0;
	// 레이어를 위한 벡터
	vector<const char*> LayerNameVector;

	// 오브젝트vector 초기화를 위한 변수
	_bool	m_isLayerSelectChange = false;
	// 오브젝트 리스트를 위한 벡터
	vector<const char*> m_ObjectVector;
	// 오브젝트 리스트의 클릭 인덱스를 판단하기 위한 변수
	_int	m_iObjectListSelectIndex = 0;
	_bool	m_isObjectListSelectChange = false;

	_bool	m_isObjectDestroy = false;

	/*For. Camera - Dependent*/
	_float4 m_vCamera_Dependent_Value;
	_float m_fCamera_Dependent_IntervalZ = 2.f;
	_float m_fFovy_Angle = 70.f;
	_float m_fCameraBias = 1.f;
	_float m_fCameraBias_Down = 1.f;
	// For.Shader
	_bool	m_isRenderLimRight = false;
	_bool	m_IsRenderHDR = true;
	_bool	m_IsRenderBloom = true;
	_bool	m_IsRenderBloomEffect = true;
	_bool	m_IsRenderDOF = false;
	_bool	m_IsRenderEmissive = true;
	_bool	m_IsRenderShadow = true;
	_bool	m_IsRenderMotionBlur = false;
	_bool	m_IsRenderRadial = false;
	_bool	m_bAO = true;

	_int	m_iToneOption = 0;
	_int	m_iBlurOption = 0;
	_float	m_BloomWeight[4];
	_float	m_BloomEffectWeight[4];
	_float	m_DOFWeight[4];
	_float  m_fShadeAmbient = 0.2f;
	_float  m_fSSAORadious = 1.f;
	_float  m_fBias = 3.f;
	_float2	m_vCameraFixAdjustPos = _float2(200.f, -100.f);
	_float2	m_vCameraMouseMoveSpeed = _float2(0.16f, 0.16f);
	// RimLight Color
	_float m_RimLightColorArr[4] = { 1.f,1.f,1.f,1.f };
	_float2	m_vRimRatio;
	_float m_fRimWidth = 0.f;
	/*For.RectMove*/
	_float3 m_vScale[3];
	_float3 m_vPos[3];
	_float	m_fRad[2];

	_float m_fSourSpeed[2];
	_float m_fDestSpeed[2];
	_float m_fMaxTime[2];

	/*For.Shadow*/
	_float	m_fShadowBias = 0.175f;
	_float	m_fPercentLimitMax = 0.175f;

	/*For Dissolve*/
	_bool	m_bDissolve = false;
	_float	m_fDissolveSpeed = 1.0f;
	_float	m_fDissolveStart = 0.1f;
	_float	m_fDissolveScaleSpeed = 1.f;
	_uint	m_iDissolveTextureIndex = 0;

	/*For. Rail*/
	_float					m_fNonFixJumpPower = 1.5f;
	_float					m_fStartSpeed = 0.025f;
	_float					m_fDefaultSpeed = 0.6f;
	_float					m_fFastSpeed = 1.3f;
	_float					m_fSlowSpeed = 0.3f;
	_float					m_fRailCollisionCheckTerm = 2.0f;
	_float					m_fJumpCorrection = 1.7f;
	_float					m_fStartJumpAccMax = 0.1f;
	_float					m_fRayTrans = 0.08f;

	_float					m_fJumpPower = 17.5f;
	_float					m_fFallSpeed = 3.f;
	_float					m_fLookSpeed = 20.f;
	_float					m_fMoveSpeed = 1.f;
	_float					m_fAdjustSpeed = 20.0f;
	_uint					m_iLut_Count = 0;
	_bool					m_bLut = true;

	/*For.OffsetRGB*/
	_float3					m_vOffsetRGB_Wall = _float3(0.f, 0.f, 0.f);
	_float3					m_vOffsetRGB_Floor = _float3(0.f, 0.f, 0.f);
	_float3					m_vOffsetRGB_Player = _float3(0.f, 0.f, 0.f);
	_float					m_fLightOffset = 1.f;

	/*For. MSAA*/
	_float					m_fMsaaOffset = 0.125f;
	_uint					m_iMassSampler = 0;

	/*For. Fog*/
	_bool					m_bFog_Height = true;
	_float4					m_vFog_Color = _float4(0.f, 0.f, 0.f, 1.f);
	_float					m_fFog_Values[3];

	/*For. Radial*/
	_int					m_iRadialCnt = 14;
	_float					m_fRadialBlurPower = 0.03f;
	_float2					m_vRadialDir = _float2(0.5f, 0.5f);

	/*For. SSAA*/
	_bool					m_bIsSSAA = true;

	/*For. Height*/
	_float					m_fReflectHeight = 0.f;
	_float					m_fReflectHeight2 = 0.f;
#ifdef _DEBUG
	_bool isRenderTargetRender = true;
#endif // _DEBUG

};
