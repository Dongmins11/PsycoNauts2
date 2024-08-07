#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CBoxCollider;
class CTexture;
END

BEGIN(Client)

class CPyroPainting final :public CGameObject
{
private:
	CPyroPainting(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPyroPainting(const CPyroPainting& rhs);
	virtual ~CPyroPainting() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:/*For.Set*/
	void Set_Fluctuate() { m_bIsFluctuate = true, m_fFluctuateInterval = 0.2f, m_iFluctuateStep = 0, m_fFluctuateSize = 0.1f; }

public:
	void Fluctuate(_double TimeDelta);
protected:
	virtual void Add_Force(_fvector vDir, PxForceMode::Enum eMode = PxForceMode::eFORCE);
	virtual void Setting_Throwing(_fvector vDir);
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnCollisionStay(CCollisionObject& collision) override;
	virtual void OnCollisionExit(CCollisionObject& collision) override;
	virtual void OnTriggerEnter(CCollisionObject& collision)override;

	virtual void SettingActorActive();
private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);

private: /* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

	virtual void OnControllerCollisionHit(CCollisionObject& collision);
	HRESULT	Create_Effect();

private:
	_int					m_iNavigationIndex = 0;
	//LayerType				m_eLayerType = LayerType::LAYERTYPE_STATIC;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;

	_bool					m_isMove = false;
	_bool					m_isColController = false;

	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	_float					m_ArrKey[3];

	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	/*For.NormalMapping*/
	_bool					m_bIsNormalMapping = true;

	/*For.Effect*/
	_bool					m_isEffectCreate = false;
	_double					m_DeleteTimeAcc = 0.0;

	/*For.Fluctuate*/
	_bool					m_bIsFluctuate = false;
	_uint					m_iFluctuateStep = 0;
	_float					m_fFluctuateInterval = 0.2f;
	_float					m_fFluctuateSize = 0.1f;

private:
	_bool					m_bBrainStromBurningCheck = false;
	_float					m_fBurningTimeAcc = 0.f;
	_uint					m_iTextureIndex = 0;
	_uint					m_iLayerIndex = 0;
	_bool					m_IsLockOpen = false;

public:
	static CPyroPainting* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END