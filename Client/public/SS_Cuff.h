#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
class CModel;
END

BEGIN(Client)

class CSS_Cuff final :public CGameObject
{
private:
	CSS_Cuff(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSS_Cuff(const CSS_Cuff& rhs);
	virtual ~CSS_Cuff() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollisionEnter(CCollisionObject& collision) override;

private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);
private: /* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pLightMapCom = nullptr;

	CGameObject*			m_pReadyEffect = nullptr;
	CGameObject*			m_pEffect = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

private:
	_int					m_iNavigationIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;
	_bool					m_isMove = false;
	_bool					m_isColController = false;
	_uint					m_iMYObjectCount = 0;

private:/* Moveinfo */
	_float					m_fMove = 0.f;

	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	_float					m_ArrKey[3];

	/* For. Effect */
	_double					m_TimeAcc = 0.0;
	_bool					m_isReadyEffectCreate = false;
	_bool					m_isEffectCreate = false;

	/* For. Pass */
	_uint					m_iRenderPassArr[2] = { 0, 7 };
	_bool					m_isEmissiveActive[2] = { false, true };

public:
	static CSS_Cuff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;
};

END