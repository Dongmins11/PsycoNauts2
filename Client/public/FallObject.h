#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "BoxCollider.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CBoxCollider;
END

BEGIN(Client)

class CFallObject final :public CGameObject
{
private:
	CFallObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFallObject(const CFallObject& rhs);
	virtual ~CFallObject() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollisionEnter(CCollisionObject& collision) override;

public:/*For.Set*/
	void Set_Falling();
	void Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed);
private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);
private: /* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex);

private: /* MoveCeck */
	void MoveFALL();
	void CanbinetCountMove();
private: /* Dissolve*/
	void Dissolve(_double TimeDelta);
	void Dissolve_Scaling(_double TimeDelta);

private:/* 확인용*/
	void Check();

public:
	void Set_FallStart() { m_bFall = true; }


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

private:
	_int					m_iNavigationIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;
	_bool					m_bFall = false; /* 쓰러지는거 확인하기 위해서 */
	_bool					m_isMove = false;
	_bool					m_isColController = false;
	_uint					m_iMYObjectCount = 0;

private:/* Moveinfo */
	_float  Time = 0.f;
	_float  AccumulationTime = 0.001f;
	_float4 m_MoveRoation = { 0.f,0.f,0.f,0.f };

	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	_float					m_ArrKey[3];

	/*For.Dissolve*/
	_float					m_fDissolveTime = 0.f;
	_double					m_dDissolveSpeed = 1.4f;
	_bool					m_bIsDissolve = false;
	_bool					m_bBackUpDissolve = false;
	_uint					m_iTextureIndex = 0;
	_float					m_fDissolveStart = 0.1f;
	_float3					m_vBackUpScale;
	_float3					m_vDissolveScale;
	_float					m_fDissolveScaleSpeed = 0.5f;
	
public:
	static CFallObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;

};

END