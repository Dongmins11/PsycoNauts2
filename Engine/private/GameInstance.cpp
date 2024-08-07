#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pScene_Manager(CScene_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pSound_Manager(CSoundManager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pPhysXScene_Manager(CPhysXScene_Manager::Get_Instance())
	, m_pData_Manager(CData_Manager::Get_Instance())
	, m_pEffect_Manager(CEffect_Manager::Get_Instance())
{
	Safe_AddRef(m_pData_Manager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pScene_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pPhysXScene_Manager);
	Safe_AddRef(m_pEffect_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumScenes, INPUTDESC InputDesc, CGraphic_Device::GRAPHICDESC GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pInput_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDevice, ppDeviceContext)))
		return E_FAIL;
	
	if (FAILED(m_pPicking->NativeConstruct(*ppDevice, *ppDeviceContext, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_Input_Device(InputDesc.hInst, InputDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pPhysXScene_Manager->Initialize_PhysX()))
		return E_FAIL;

	m_pComponent_Manager->Reserve_Manager(iNumScenes);

	m_pObject_Manager->Reserve_Manager(iNumScenes);

	m_pFrustum->Intialize_Frustum();

	m_pTarget_Manager->NativeConstruct(*ppDevice, *ppDeviceContext);

	m_pLight_Manager->NativeConstruct(*ppDevice, *ppDeviceContext);

	m_pPhysXScene_Manager->Create_Scene();

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	return S_OK;
}

_int CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pInput_Device || 
		nullptr == m_pScene_Manager)
		return -1;

	m_pInput_Device->SetUp_InputDeviceState();

	m_pPicking->Transform_ToWorldSpace();

	m_pFrustum->Update();

	m_pObject_Manager->Tick(TimeDelta);

	m_pPipeLine->Tick();
	
	m_pPhysXScene_Manager->Simulate(TimeDelta);

	m_pObject_Manager->LateTick(TimeDelta);

	m_pScene_Manager->Tick(TimeDelta);

	if (!m_isReady)
	{
		m_pPhysXScene_Manager->Set_IsReadyPhysX(true);
		m_isReady = true;
	}

	return _int();
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	return m_pScene_Manager->Render();
}

HRESULT CGameInstance::Clear_SceneResource(_uint iSceneIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pComponent_Manager->Clear_Component(iSceneIndex);

	m_pObject_Manager->Clear_Object(iSceneIndex);	

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);	
}

_double CGameInstance::Compute_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

_bool CGameInstance::Get_Stiffness()
{
	if (nullptr == m_pTimer_Manager)
		return false;

	return m_pTimer_Manager->Get_Stiffness();
}

void CGameInstance::Set_Stiffness(_bool isStiffness)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Set_Stiffness(isStiffness);
}

_byte CGameInstance::Get_DIKeyState(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(eKeyID);
}

_long CGameInstance::Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMoveState(eMouseMoveState);
}

_byte CGameInstance::Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseButtonState(eMouseButtonState);
}

_bool CGameInstance::Key_Pressing(_uint _key)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Pressing(_key);
}

_bool CGameInstance::Key_Down(_uint _key)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Down(_key);
}

_bool CGameInstance::Key_Up(_uint _key)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Key_Up(_key);
}

HRESULT CGameInstance::Add_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(iSceneIndex, pPrototypeTag, pPrototype);
}

_bool CGameInstance::Find_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return false;

	return m_pObject_Manager->Is_Prototype_Existed(iSceneIndex,pPrototypeTag);
}

HRESULT CGameInstance::Add_ObjectToLayer(_uint iPrototypeSceneIndex, const _tchar * pPrototypeTag, _uint iSceneIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_ObjectToLayer(iPrototypeSceneIndex, pPrototypeTag, iSceneIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_ObjectToLayer_GetImmediately(_uint iPrototypeSceneIndex, const _tchar * pPrototypeTag, _uint iSceneIndex, const _tchar * pLayerTag, CGameObject ** ppOut, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_ObjectToLayer_GetImmediately(iPrototypeSceneIndex,pPrototypeTag,iSceneIndex,pLayerTag,ppOut,pArg);
}


HRESULT CGameInstance::Clear_Object(_uint iSceneIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clear_Object(iSceneIndex);
}

CComponent * CGameInstance::Get_Components(_uint iSceneIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Components(iSceneIndex, pLayerTag, pComponentTag, iIndex);	
}

_bool CGameInstance::RectPicking(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex, HWND hWnd)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->RectPicking(iSceneIndex, pLayerTag, iIndex, hWnd);
}

_uint CGameInstance::Get_LayerSize(_uint iSceneIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return 0;

	return m_pObject_Manager->Get_LayerSize(iSceneIndex, pLayerTag);
}

HRESULT CGameInstance::Delete_GameObject(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_GameObject(iSceneIndex, pLayerTag, iIndex);
}

CGameObject * CGameInstance::Get_GameObject(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	list<class CGameObject*> pList = m_pObject_Manager->Get_ObjectLayer(iSceneIndex, pLayerTag);

	_uint index = 0;
	for (auto& iter : pList)
	{
		if (index == iIndex)
			return iter;
		else
			++index;
	}

	return nullptr;
}

HRESULT CGameInstance::Add_ObjectToEffect(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iLayerSceneIndex, const _tchar* pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_ObjectToEffect(iPrototypeSceneIndex, pPrototypeTag, iLayerSceneIndex, pLayerTag, iEffectSceneIndex, pEffectTag);
}
HRESULT CGameInstance::Add_ObjectToParticle_GetImmediately(_uint iPrototypeSceneIndex, const _tchar * pPrototypeTag, _uint iLayerSceneIndex, const _tchar * pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag, CGameObject ** ppOut)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Add_ObjectToParticle_GetImmediately(iPrototypeSceneIndex, pPrototypeTag, iLayerSceneIndex, pLayerTag, iEffectSceneIndex, pEffectTag, ppOut);
}
HRESULT CGameInstance::Clear_EffectData(_uint iSceneIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clear_EffectData(iSceneIndex);
}
#ifdef _DEBUG
_uint CGameInstance::Get_LayerMapSize(_uint iSceneIndex)
{
	if (nullptr == m_pObject_Manager)
		return 0;
	return m_pObject_Manager->Get_LayerMapSize(iSceneIndex);
}

const _tchar * CGameInstance::Get_LayerName(_uint iSceneIndex, _uint idx)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return 	m_pObject_Manager->Get_LayerName(iSceneIndex,idx);
}
#endif //_DEBUG

HRESULT CGameInstance::Add_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iSceneIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iSceneIndex, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Clear_Component(_uint iSceneIndex)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Clear_Component(iSceneIndex);
}

HRESULT CGameInstance::Open_Scene(_uint iCurrentSceneIndex, CScene * pNewScene)
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	return m_pScene_Manager->Open_Scene(iCurrentSceneIndex,pNewScene);
}

_uint CGameInstance::Get_CurrentSceneIndex()
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	return m_pScene_Manager->Get_CurrentSceneIndex();
}

const CScene * CGameInstance::Get_CurrentScene() const
{
	if (nullptr == m_pScene_Manager)
		return nullptr;

	return m_pScene_Manager->Get_CurrentScene();
}

HRESULT CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATETYPE eState, _fmatrix StateMatrix)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Set_Transform(eState, StateMatrix);	
}

_matrix CGameInstance::Get_Transform(CPipeLine::TRANSFORMSTATETYPE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform(eState);
}

_vector CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition();
}

LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Lights(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Lights(pDevice, pDeviceContext, LightDesc);
}

void CGameInstance::Set_ShadeAmbient(_float fAmbient)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Set_ShadeAmbient(fAmbient);
}

void CGameInstance::Set_LightDesc(_uint iIndex, LIGHTDESC& tLightDesc)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Set_LightDesc(iIndex, tLightDesc);
}

void CGameInstance::Set_IsAO(_bool bCheck)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Set_IsAO(bCheck);
}

void CGameInstance::Light_Curling(_uint iCurlingIndex)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Light_Curling(iCurlingIndex);
}

void CGameInstance::Set_Light_CurlingIndex(_uint LayerIndex, _uint iCurlingIndex)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Set_CurIndex(LayerIndex,iCurlingIndex);
}

_uint CGameInstance::Get_Light_Size()
{
	if (nullptr == m_pLight_Manager)
		return 0;

	return m_pLight_Manager->Get_Lights_Size();
}

void CGameInstance::Lights_Clear()
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Lights_Clear();
}

void CGameInstance::Set_LightRender(_bool bCheck, _uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Set_LightRender(bCheck,iIndex);
}

_float3 CGameInstance::Get_ShadowLightLook()
{
	return m_pLight_Manager->Get_ShadowLightLook();
}

_float3 CGameInstance::Get_ShadowLightPos()
{
	return m_pLight_Manager->Get_ShadowLightPos();
}

void CGameInstance::Set_ShadowLightVector(_float3 vLook, _float3 vPos)
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Set_LightVector(vLook, vPos);
}

void CGameInstance::Set_ShadowValue(_float fShadowBias, _float fPercentLimitMax)
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Set_ShadowValue(fShadowBias, fPercentLimitMax);
}

void CGameInstance::Set_LightOffset(_float fLightOffset)
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Set_LightOffset(fLightOffset);
}

void CGameInstance::Set_IsLightCuriling(_bool bCheck)
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Set_IsCuriling(bCheck);
}

_bool CGameInstance::isIn_WorldSpace(_fvector vWorldPoint, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPoint, fRange);
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pDeviceContext, pFontTag, pFontFilePath);	
}

HRESULT CGameInstance::Draw_Font(const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Draw_Font(pFontTag, pText, vPosition, vColor);
}
#ifdef _DEBUG

_bool CGameInstance::Get_TargeBufferIsRender()
{
	if (nullptr == m_pTarget_Manager)
		return false;

	return m_pTarget_Manager->Get_IsRender();
}

void CGameInstance::Set_TargeBufferIsRender(_bool isRender)
{
	if (nullptr == m_pTarget_Manager)
		return;

	m_pTarget_Manager->Set_IsRender(isRender);
}
#endif
ID3D11ShaderResourceView * CGameInstance::Get_SRV(const _tchar * pTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(pTargetTag);
}

HRESULT CGameInstance::PlaySoundOnce(TCHAR * pSoundKey, CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlaySound(pSoundKey, eID);
}

HRESULT CGameInstance::PlayBGM(TCHAR * pSoundKey)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlayBGM(pSoundKey);
}

HRESULT CGameInstance::StopSound(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->StopSound(eID);
}

HRESULT CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->StopAll();
}

HRESULT CGameInstance::BGMVolumeUp(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->BGMVolumeUp(_vol);
}

HRESULT CGameInstance::BGMVolumeDown(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->BGMVolumeDown(_vol);
}

HRESULT CGameInstance::SettingVolume(_float _vol, CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->SettingVolume(_vol, eID);
}

HRESULT CGameInstance::BGMPause(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->BGMPause(eID);
}

_bool CGameInstance::IsChannelPlaying(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->IsChannelPlaying(eID);
}

_bool CGameInstance::Picking_InLocalSpace(CTransform * pTransform, CVIBuffer * pVIBuffer, _float3 * pOut)
{
	if (nullptr == m_pPicking)
		return false;

	return m_pPicking->Picking_InLocalSpace(pTransform, pVIBuffer, pOut);
}

_bool CGameInstance::Picking_InLocalSpace(_fmatrix * _modelWorldMat, CVIBuffer * pVIBuffer, _float3 * pOut)
{
	if (nullptr == m_pPicking)
		return false;

	return m_pPicking->Picking_InLocalSpace(_modelWorldMat, pVIBuffer, pOut);
}

_bool CGameInstance::Picking_InWorldSpace(CTransform * _pTransform, CCollider * pCollider, _float3 * pOut, _float* dist)
{
	if (nullptr == m_pPicking)
		return false;

	return m_pPicking->Picking_InWorldSpace(_pTransform, pCollider, pOut, dist);
}

_bool CGameInstance::Picking_ShortInLocalSpace(CTransform * pTransform, CVIBuffer * pVIBuffer, _float3 * pOut)
{
	if (nullptr == m_pPicking)
		return false;


	return m_pPicking->Picking_ShortInLocalSpace(pTransform, pVIBuffer, pOut);
}

_bool CGameInstance::Picking_InLocalSpaceWithDist(_fmatrix * _modelWorldMat, CVIBuffer * pVIBuffer, _float3 * pOut, _float * pDist)
{
	if (nullptr == m_pPicking)
		return false;
	    

	return m_pPicking->Picking_InLocalSpaceWithDist(_modelWorldMat, pVIBuffer, pOut, pDist);
}

_vector CGameInstance::Get_RayDir()
{
	if (nullptr == m_pPicking)
		return XMVectorZero();

	return m_pPicking->Get_RayDir();
}

_vector CGameInstance::Get_RayPos()
{
	if (nullptr == m_pPicking)
		return XMVectorZero();

	return m_pPicking->Get_RayPos();
}

HRESULT CGameInstance::Create_Scene()
{
	if (nullptr == m_pPhysXScene_Manager)
		return E_FAIL;

	return m_pPhysXScene_Manager->Create_Scene();
}

void CGameInstance::Simulate(_double TimeDelta)
{
	if (nullptr == m_pPhysXScene_Manager)
		return;

	m_pPhysXScene_Manager->Simulate(TimeDelta);
}

_uint CGameInstance::Add_LayerMask(LayerType eType1, LayerType eType2)
{
	if (nullptr == m_pPhysXScene_Manager)
		return -1;

	return m_pPhysXScene_Manager->Add_LayerMask((_uint)eType1,(_uint)eType2);
}

void CGameInstance::Add_PointsToVector(_float3 * vPoint)
{
	if (nullptr == m_pPhysXScene_Manager)
		return;

	return m_pPhysXScene_Manager->Add_PointsToVector(vPoint);
}

void CGameInstance::Clear_NaviPoints()
{
	if (nullptr == m_pPhysXScene_Manager)
		return;

	return m_pPhysXScene_Manager->Clear_NaviPoints();
}

vector<_float3> CGameInstance::Get_PointVector()
{
	if (nullptr == m_pPhysXScene_Manager)
		return vector<_float3>();

	return m_pPhysXScene_Manager->Get_PointVector();
}

const _bool CGameInstance::Raycast(CPhysXScene_Manager::RAYCASTDESCIN tDescIn, CPhysXScene_Manager::RAYCASTDESCOUT & tDescOut)
{
	if (nullptr == m_pPhysXScene_Manager)
		return false;

	return m_pPhysXScene_Manager->Raycast(tDescIn, tDescOut);
}

const _bool CGameInstance::Overlap(CPhysXScene_Manager::OVERLAPDESC & tDesc)
{
	if (nullptr == m_pPhysXScene_Manager)
		return false;

	return m_pPhysXScene_Manager->Overlap(tDesc);
}

const _tchar* CGameInstance::Add_PrototypeDataToList(const TCHAR * pPrototypeTag)
{
	if (nullptr == m_pData_Manager)
		return nullptr;

	return m_pData_Manager->Add_PrototypeData(pPrototypeTag);
}

HRESULT CGameInstance::Add_PathDataToList(const char * pPath)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;

	return m_pData_Manager->Add_PathData(pPath);
}

const _tchar* CGameInstance::Add_LayerTagToList(const TCHAR * pLayerTag)
{
	if (nullptr == m_pData_Manager)
		return nullptr;

	return m_pData_Manager->Add_LayerTagData(pLayerTag);
}

HRESULT CGameInstance::Create_Prototype_Effect_Data(CEffect_Manager::EFFECT_TYPE eType, _uint iPrototypeEffectSceneIndex, const _tchar * pEffectPath, const _tchar * pEffectTag)
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Create_Prototype_Effect_Data(eType, iPrototypeEffectSceneIndex, pEffectPath, pEffectTag);
}

HRESULT CGameInstance::Create_Prototype_Effect_Particle_Data(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iPrototypeEffectSceneIndex, const _tchar * pEffectPath, const _tchar * pEffectTag)
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Create_Prototype_Effect_Particle_Data(pDevice, pDeviceContext, iPrototypeEffectSceneIndex, pEffectPath, pEffectTag);
}

HRESULT CGameInstance::Effect_Call_Inheritance(_uint iLayerSceneIndex, _bool isOnce, _bool isNonBillboard, class CTransform* pTransform, const _tchar * pLayerTag)
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Effect_Call_Inheritance(iLayerSceneIndex, isOnce, isNonBillboard, pTransform, pLayerTag);
}

HRESULT CGameInstance::Effect_Call_WorldPos(_uint iLayerSceneIndex, const _tchar * pLayerTag, _vector vPos, _bool isWorldPosEnable)
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Effect_Call_WorldPos(iLayerSceneIndex, pLayerTag, vPos, isWorldPosEnable);
}

HRESULT CGameInstance::Effect_ParentBoneMatrix(_uint iLayerSceneIndex, const _tchar * pLayerTag, CTransform* pTransform, CModel * pModel, const char * pBoneName)
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Effect_ParentBoneMatrix(iLayerSceneIndex, pLayerTag, pTransform, pModel, pBoneName);
}

void CGameInstance::Setting_Effect_Speed(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar* pComTag, _double EffectSpeed)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_Speed(iLayerSceneIndex, pLayerTag, pComTag, EffectSpeed);
}

void CGameInstance::Setting_Effect_UVSprite(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar* pComTag, _double SprtieTime)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_UVSprite(iLayerSceneIndex, pLayerTag, pComTag, SprtieTime);
}

void CGameInstance::Setting_Effect_MovePos(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _vector vMovePos)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_MovePos(iLayerSceneIndex, pLayerTag, pComTag, vMovePos);
}

void CGameInstance::Setting_Effect_TransformPos(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _uint iType, _float fScale)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_TransformPos(iLayerSceneIndex, pLayerTag, pComTag, iType, fScale);
}

void CGameInstance::Setting_Effect_Scale(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _vector vScale)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_Scale(iLayerSceneIndex, pLayerTag, pComTag, vScale);
}

void CGameInstance::Setting_Effect_NonRotation(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _bool isNonRotation)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_NonRotation(iLayerSceneIndex, pLayerTag, pComTag, isNonRotation);
}

void CGameInstance::Setting_Effect_LocalBillboard(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _bool isLocalBillboard)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Setting_Effect_LocalBillboard(iLayerSceneIndex, pLayerTag, pComTag, isLocalBillboard);
}

_bool CGameInstance::Get_Ball_Confirm(_uint iIndex)
{
	if (nullptr == m_pEffect_Manager)
		return true;

	return m_pEffect_Manager->Get_Ball_Confirm(iIndex);
}

void CGameInstance::Set_Ball_Confirm(_uint iIndex, _bool isConfirm)
{
	if (nullptr == m_pEffect_Manager)
		return;

	return m_pEffect_Manager->Set_Ball_Confirm(iIndex, isConfirm);
}

void CGameInstance::Release_Engine()
{
	if (0 != CGameInstance::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CData_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CEffect_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CScene_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CComponent_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CObject_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CPhysXScene_Manager::Get_Instance()->Destroy_Instance())
		return;
	
	if (0 != CTimer_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CPipeLine::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CInput_Device::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CLight_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CFrustum::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CFont_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CTarget_Manager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CPicking::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CSoundManager::Get_Instance()->Destroy_Instance())
		return;

	if (0 != CGraphic_Device::Get_Instance()->Destroy_Instance())
		return;
	
}

void CGameInstance::Free()
{
	Safe_Release(m_pData_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPhysXScene_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pScene_Manager);
	Safe_Release(m_pGraphic_Device);
}

