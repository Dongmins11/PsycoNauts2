#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Timer_Manager.h"
#include "Scene_Manager.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "PipeLine.h"
#include "Frustum.h"
#include "Target_Manager.h"
#include "SoundManager.h"
#include "Picking.h"
#include "PhysXScene_Manager.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumScenes, INPUTDESC InputDesc, CGraphic_Device::GRAPHICDESC GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext);
	_int Tick_Engine(_double TimeDelta);
	HRESULT Render_Engine();
	HRESULT Clear_SceneResource(_uint iSceneIndex);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_double Compute_TimeDelta(const _tchar* pTimerTag);
	_bool	Get_Stiffness();
	void	Set_Stiffness(_bool isStiffness);

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte eKeyID);
	_long Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState);
	_byte Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState);

	// 키 프레싱, 다운, 업함수
	_bool Key_Pressing(_uint _key);
	_bool Key_Down(_uint _key);
	_bool Key_Up(_uint _key);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(_uint iSceneIndex, const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	_bool Find_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag);
	HRESULT Add_ObjectToLayer(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iSceneIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	HRESULT Clear_Object(_uint iSceneIndex);
	CComponent*	Get_Components(_uint iSceneIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	
	// Object를 Layer에 클론해서 넣어줌과 동시에 매개변수에 전달(오브젝트를 반환)해주는 함수
	HRESULT Add_ObjectToLayer_GetImmediately(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iSceneIndex, const _tchar* pLayerTag, CGameObject** ppOut, void* pArg);
	// 렉트 피킹함수 ( GameObject의 Rect변수에 값을 설정해줘야함 )
	_bool RectPicking(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex, HWND hWnd);
	// LayerSize를 구해주는 함수
	_uint Get_LayerSize(_uint iSceneIndex, const _tchar * pLayerTag);
	// 특정 Scene에 있는 특정 Layer의 특정 index에 위치하는 오브젝트를 지워주는 함수 
	HRESULT Delete_GameObject(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex);
	// 오브젝트를 반환하는 함수
	class CGameObject*	Get_GameObject(_uint iSceneIndex, const _tchar* pLayerTag, _uint iIndex);
	
	// Effect 관련	
	HRESULT Add_ObjectToEffect(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iLayerSceneIndex, const _tchar* pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag);
	HRESULT Add_ObjectToParticle_GetImmediately(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iLayerSceneIndex, const _tchar* pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag, CGameObject** ppOut);
	HRESULT Clear_EffectData(_uint iSceneIndex);

#ifdef _DEBUG
	_uint Get_LayerMapSize(_uint iSceneIndex);
	const _tchar* Get_LayerName(_uint iSceneIndex, _uint idx);
#endif // _DEBUG

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iSceneIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iSceneIndex, const _tchar* pPrototypeTag, void* pArg=nullptr);
	HRESULT Clear_Component(_uint iSceneIndex);

public: /* For.Scene_Manager */
	HRESULT Open_Scene(_uint iCurrentSceneIndex, class CScene* pNewScene);
	_uint	Get_CurrentSceneIndex();
	const CScene* Get_CurrentScene() const;
public: /* For.PipeLine */
	HRESULT Set_Transform(CPipeLine::TRANSFORMSTATETYPE eState, _fmatrix StateMatrix);
	_matrix Get_Transform(CPipeLine::TRANSFORMSTATETYPE eState);
	_vector Get_CamPosition();

public: /* For.Light_Manager */
	LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT	   Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	void	   Set_ShadeAmbient(_float fAmbient);
	void	   Set_LightDesc(_uint iIndex, LIGHTDESC& tLightDesc);
	void	   Set_IsAO(_bool bCheck);
	void	   Light_Curling(_uint iCurlingIndex); /* 매니져로 들어갈것 */ 
 	void	   Set_Light_CurlingIndex(_uint LayerIndex, _uint iCurlingIndex); /* Light로 들어갈것 */
	_uint	   Get_Light_Size(); /* Light Size */
	void	   Lights_Clear();
	void	   Set_LightRender(_bool bCheck, _uint iIndex);
	void	   Set_ShadowLightVector(_float3 vLook, _float3 vPos);
	void	   Set_ShadowValue(_float fShadowBias, _float fPercentLimitMax);
	void	   Set_LightOffset(_float fLightOffset);
	void	   Set_IsLightCuriling(_bool bCheck);


	_float3 Get_ShadowLightLook();
	_float3 Get_ShadowLightPos();
public: /* For.Frustum */
	_bool isIn_WorldSpace(_fvector vWorldPoint, _float fRange = 0.f);

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Font(const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor);


public:/* For.Target_Manager */
#ifdef _DEBUG
	_bool	Get_TargeBufferIsRender();
	void	Set_TargeBufferIsRender(_bool isRender);
#endif // DEBUG


	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);


public: /* For.Sound_Manager */
	HRESULT PlaySoundOnce(TCHAR* pSoundKey, CHANNELID eID);
	HRESULT PlayBGM(TCHAR* pSoundKey);
	HRESULT StopSound(CHANNELID eID);
	HRESULT StopAll();
	HRESULT	BGMVolumeUp(_float _vol);
	HRESULT	BGMVolumeDown(_float _vol);
	HRESULT SettingVolume(_float _vol, CHANNELID eID);
	HRESULT	BGMPause(CHANNELID eID);
	_bool IsChannelPlaying(CHANNELID eID);
public: /* For.Picking */
	_bool Picking_InLocalSpace(class CTransform* pTransform, class CVIBuffer* pVIBuffer, _float3* pOut);
	_bool Picking_InLocalSpace(_fmatrix * _modelWorldMat, CVIBuffer * pVIBuffer, _float3 * pOut);
	_bool Picking_InWorldSpace(CTransform * _pTransform, class CCollider* pCollider, _float3* pOut, _float* dist);
	_bool Picking_ShortInLocalSpace(class CTransform* pTransform, class CVIBuffer* pVIBuffer, _float3* pOut);
	_bool Picking_InLocalSpaceWithDist(_fmatrix * _modelWorldMat, CVIBuffer * pVIBuffer, _float3 * pOut, _float * pDist);
	_vector Get_RayDir();
	_vector Get_RayPos();

public: /* For.PhysXManager */
	HRESULT Create_Scene();
	void	Simulate(_double TimeDelta);
	_uint	Add_LayerMask(LayerType eType1,LayerType eType2);
	void	Add_PointsToVector(_float3* vPoint);
	void	Clear_NaviPoints();
	vector<_float3> Get_PointVector();

	const _bool Raycast(CPhysXScene_Manager::RAYCASTDESCIN tDescIn, CPhysXScene_Manager::RAYCASTDESCOUT& tDescOut);
	const _bool Overlap(CPhysXScene_Manager::OVERLAPDESC& tDesc);
	/* For.DataManager */
public:
	HRESULT Add_PathDataToList(const char* pPath);
	const _tchar* Add_PrototypeDataToList(const TCHAR* pPrototypeTag);
	const _tchar* Add_LayerTagToList(const TCHAR* pLayerTag);

public: /* For Effect_Manager */
	/* 이펙트 프로토 타입 생성 */
	HRESULT Create_Prototype_Effect_Data(CEffect_Manager::EFFECT_TYPE eType, _uint iPrototypeEffectSceneIndex, const _tchar * pEffectPath, const _tchar * pEffectTag);
	HRESULT	Create_Prototype_Effect_Particle_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iPrototypeEffectSceneIndex, const _tchar* pEffectPath, const _tchar* pEffectTag);
	/* 이펙트 트랜스폼으로 상속 */
	HRESULT Effect_Call_Inheritance(_uint iLayerSceneIndex, _bool isOnce, _bool isNonBillboard, class CTransform* pTransform, const _tchar * pLayerTag);
	HRESULT Effect_Call_WorldPos(_uint iLayerSceneIndex, const _tchar* pLayerTag, _vector vPos, _bool isWorldPosEnable = false);
	HRESULT Effect_ParentBoneMatrix(_uint iLayerSceneIndex, const _tchar* pLayerTag, class CTransform* pTransform, class CModel* pModel, const char* pBoneName);
	/* 전체적인 이펙트 속도 조절 */
	void Setting_Effect_Speed(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _double EffectSpeed = 1.0);
	/* 전체적인 이펙트 UV 조절 */
	void Setting_Effect_UVSprite(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _double SprtieTime = 0.1);
	/* 전체적인 이펙트 로컬 Pos 조정*/
	void Setting_Effect_MovePos(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _vector vMovePos);
	void Setting_Effect_TransformPos(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _uint iType, _float fScale = 1.f);
	/* 전체적인 이펙트 스케일 조정 */
	void Setting_Effect_Scale(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _vector vScale);
	/* 전체적인 Rotation 여부 */
	void Setting_Effect_NonRotation(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _bool isNonRotation);

	void Setting_Effect_LocalBillboard(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _bool isLocalBillboard);

	_bool Get_Ball_Confirm(_uint iIndex);
	void Set_Ball_Confirm(_uint iIndex, _bool isConfirm);

private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;	
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CScene_Manager*					m_pScene_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CPipeLine*						m_pPipeLine = nullptr;
	CLight_Manager*					m_pLight_Manager = nullptr;
	CFrustum*						m_pFrustum = nullptr;
	CFont_Manager*					m_pFont_Manager = nullptr;
	CTarget_Manager*				m_pTarget_Manager = nullptr;
	CSoundManager*					m_pSound_Manager = nullptr;
	CPicking*						m_pPicking = nullptr;
	CPhysXScene_Manager*			m_pPhysXScene_Manager = nullptr;
	CData_Manager*					m_pData_Manager = nullptr;
	CEffect_Manager*				m_pEffect_Manager = nullptr;

private:
	_bool						m_isReady = false;
public:
	static void Release_Engine();
	virtual void Free() override;
};

END