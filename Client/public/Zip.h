#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CTexture;
END

BEGIN(Client)

class CZip final :public CGameObject
{
private:
	CZip(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CZip(const CZip& rhs);
	virtual ~CZip() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void Blending(_double TimeDelta);
	void End_Blending(_double TimeDelta);

private:
	void Dissolve(_double TimeDelta);
	void Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed);

public:/*For.Set*/
	void Set_Fluctuate() { m_bIsFluctuate = true, m_fFluctuateInterval = 0.2f, m_iFluctuateStep = 0, m_fFluctuateSize = 0.1f; }

public:
	void Fluctuate(_double TimeDelta);
	void End_Blending(_bool Endblenging) { m_Endblending = Endblenging; }

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
	CBoxCollider*			m_pColliderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

private:
	_int					m_iNavigationIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;

	_bool					m_isMove = false;
	_bool					m_isColController = false;


	_bool					m_RenderCheck = true;
	_double					m_RenderTimer = 0.0;
	_bool					m_OnesCreate = true;


	_bool					m_bIsDissolve = false;
	_bool					m_bBackUpDissolve = false;
	_float					m_fDissolveTime = 0.f;
	_double					m_dDissolveSpeed = 1.4f;
	_uint					m_iTextureIndex = 0;
	_float					m_fDissolveStart = 0.1f;
	_float3					m_vBackUpScale;
	_float3					m_vDissolveScale;
	_float					m_fDissolveScaleSpeed = 0.5f;

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
	_bool					m_bBlending = true;
	_float					m_BlendingTimer = 0.01f;
	_bool					m_Endblending = false;


public:
	static CZip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END

