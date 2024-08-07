#pragma once

#include "Base.h"
#include "PhysXCollider.h"
BEGIN(Engine)
class CCollisionObject;
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent*	Get_Components(const _tchar* pComponentTag);
	
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollisionEnter(CCollisionObject& collision) {};
	virtual void OnCollisionStay(CCollisionObject& collision) {};
	virtual void OnCollisionExit(CCollisionObject& collision) {};

	virtual void OnTriggerEnter(CCollisionObject& collision) {};
	virtual void OnTriggerStay(CCollisionObject& collision) {};
	virtual void OnTriggerExit(CCollisionObject& collision) {};

	virtual void OnControllerCollisionHit(CCollisionObject& collision) {};

	_bool	RectPicking(HWND hWnd);
	virtual void Setting_Throwing(_fvector vDir) {};
	virtual void Add_Force(_fvector vDir,PxForceMode::Enum eEnum) {};
	virtual void SettingActorActive() {};
public:
	_float Get_ViewZ() { return m_fViewZ;}
	_uint	Get_CurrentScene() const { return m_iCurrentScene; }
	_bool Get_IsDestroy() const { return m_isDestroy; }
	_bool Get_IsInteraction() const { return m_isInteraction; }
	_bool Get_IsRender() const { return m_isRender; }
	_uint Get_RenderPass() const { return m_iRenderPass; }
	const _tchar* Get_ObjectFullName() { return m_StrObjectFullName.c_str(); }
	const _tchar*  Get_LayerName() { return m_StrLayerName.c_str(); }
	const _tchar*  Get_ModelName() { return m_StrModelName.c_str(); }
	const _bool Get_IsPicking() const { return m_isPicking; }
	const _bool Get_IsThrowing() const { return m_isThrowing; }
	const _bool Get_IsPulling() const { return m_isPulling; }
	const _float Get_ObjectRange() const { return m_fRangeForFrustum; }
	const LayerType Get_ObjectLayerType() const { return m_eLayerType; }
	const _bool	Get_IsActorActive() const { return m_isActorActive; }
	const _float4 Get_ColliderDir() const { return m_vColliderDir; }


	void Set_RectForPicking(RECT rc){ m_tRect = rc;	}
	void Set_CurrentScene(_uint idx) { m_iCurrentScene = idx; }
	void Set_IsDestroy(_bool isDestroy) { m_isDestroy = isDestroy; }
	void Set_IsInteraction(_bool isInteraction) { m_isInteraction = isInteraction; }
	void Set_IsRender(_bool isRender) { m_isRender = isRender; }
	void Set_RenderPass(_uint iPass) { m_iRenderPass = iPass; }
	void Set_RenderPassBackUp() { m_iRenderPass = m_iRenderPassBackUp; }
	void Set_RGBA(_float4 vRgva) { m_vRGBA = vRgva; }
	void Set_RimPower(_float fRimPower) { m_fRimPower = fRimPower; }
	void Set_IsPicking(_bool isPicking) { m_isPicking = isPicking; }
	void Set_IsThrowing(_bool isThrowing) { m_isThrowing = isThrowing; }
	void Set_IsPulling(_bool IsPulling) { m_isPulling = IsPulling; }
	void Set_ObjectRange(_float fRange) { m_fRangeForFrustum = fRange; }
	void Set_ObjectLayerType(LayerType eType) { m_eLayerType = eType; }
	void Set_ViewZ(_float fView) { m_fViewZ = fView; }
	void Set_ActorActive(_bool isActive) { m_isActorActive = isActive; }
	void Set_AnimationIndex(_uint iIndex) { m_iAnimationIndex = iIndex; }
protected:
 	void Compute_ViewZ(class CTransform* pTransform);
	
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	unordered_map<const _tchar*, class CComponent*>				m_Components;
	typedef unordered_map<const _tchar*, class CComponent*>		COMPONENTS;

protected:
	RECT				m_tRect = {0,0,0,0};
	_float				m_fViewZ = 0.f;
	_uint				m_iCurrentScene = 0;
	wstring				m_StrObjectFullName =TEXT("");
	wstring				m_StrLayerName = TEXT("");
	wstring				m_StrModelName = TEXT("");
protected:
	// 랜더를 할지 말지 결정하는 변수, 상황에 따라 Tick, LateTick에 사용해서 함수를 빠져나가게 설정 가능
	_bool	m_isRender = true;
	// true로 변경시 오브젝트 지워짐
	_bool	m_isDestroy = false;

	// 상호작용 밑 효과 체크용
	_bool	m_isInteraction = false;

	// For Collider
	_bool	m_isPicking = false;
	_bool	m_isThrowing = false;
	_bool	m_isPulling = false;
	_bool	m_isUpdateCollide = true;
	_bool	m_isPushing = false;
	_float4 m_vColliderDir;

	// 랜더 Pass// Default : 0
	_uint	m_iRenderPass = 0;
	// 렌더패스 백업
	_uint	m_iRenderPassBackUp = 0;
	// animation
	_uint	m_iAnimationIndex = 0;
	// 색깔
	_float4		m_vRGBA;
	// 림라이트 파워
	_float		m_fRimPower = 1.f;
	_float		m_fRangeForFrustum = 1.f;

	// 레이어타입
	LayerType	m_eLayerType = LayerType::LAYERTYPE_END;
	// ActorRender
	_bool		m_isActorActive = false;
protected:
	virtual HRESULT SetUp_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg = nullptr);


private:
	CComponent*	 Find_Component(const _tchar* pComponentTag);


public:	
	virtual CGameObject* Clone(void* pArg,_uint iCurrentScene =0) = 0;
	virtual void Free() override;	
};

END