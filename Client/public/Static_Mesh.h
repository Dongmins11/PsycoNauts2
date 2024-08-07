#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CBoxCollider;
END

BEGIN(Client)

class CStatic_Mesh final:public CGameObject
{
private:
	CStatic_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStatic_Mesh(const CStatic_Mesh& rhs);
	virtual ~CStatic_Mesh() = default;

public: 
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:/*For.Set*/
	void Set_Fluctuate() { m_bIsFluctuate = true, m_fFluctuateInterval = 0.2f, m_iFluctuateStep = 0, m_fFluctuateSize=0.1f; }
	void Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed);
	void Set_OffsetRGB(_float3 vWall,_float3 vFloor) { m_vOffsetRGB_Wall = vWall , m_vOffsetRGB_Floor =vFloor; }
	void Set_Reflect_Height(_float fHeight) { m_fReflect_Height = fHeight; }
public:
	void Fluctuate(_double TimeDelta);
	void Dissolve_Scaling(_double TimeDelta);
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

private:/*For. Shadow*/
	void SetUp_IsShadow();
	void SetUp_IsReflect();
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	CGameObject*			m_pEffect = nullptr;
private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos,_float3 vObjScale);
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

	/*For.Dissolve*/
	_float					m_fDissolveTime = 0.f;
	_double					m_dDissolveSpeed = 0.1f;
	_bool					m_bIsDissolve = false;
	_bool					m_bBackUpDissolve = false;
	_uint					m_iTextureIndex = 0;
	_float					m_fDissolveStart = 0.1f;
	_float3					m_vBackUpScale;
	_float3					m_vDissolveScale;
	_float					m_fDissolveScaleSpeed = 1.f;

	/*For.OffsetRGB*/
	_float3					m_vOffsetRGB_Wall = _float3(0.f, 0.f, 0.f);
	_float3					m_vOffsetRGB_Floor = _float3(0.f, 0.f, 0.f);
	_bool					m_bIsWall = false;
	_bool					m_bIsFloor = false;

	/*For. Reflect*/
	_bool					m_bReflect_Ramlt_Up_Sour = false;
	_bool					m_bReflect_Ramlt_Up_Dest = false;
	_bool					m_bReflect_Ramlt_Down_Sour = false;
	_bool					m_bReflect_Ramlt_Down_Dest = false;
	_bool					m_bReflect_Sour = false;
	_bool					m_bReflect_Dest = false;
	_float					m_fReflect_Height = 0.f;

	/*For. SetUp*/
	_bool					m_bIsSetUpShadow = false;
	_bool					m_bIsSetUpReflect = false;
public:
	static CStatic_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END