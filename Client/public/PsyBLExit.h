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

class CPsyBLExit final:public CGameObject
{
private:
	CPsyBLExit(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPsyBLExit(const CPsyBLExit& rhs);
	virtual ~CPsyBLExit() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);
private: /* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex);

public: /* MoveCeck */
	void Set_Move(_bool Move) { m_MoveCheck = Move; }

private:/* È®ÀÎ¿ë*/
	void Check(_double TimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
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

private:/* Moveinfo */
	_float			        Time = 0.f;
	_float4			        m_MovePostion[2];
	_bool					m_MoveCheck = false;
	_float			        AccumulationTime = 0.003f;
	_float4					m_vMove;
	_bool					m_bCameraMove = true;
	_bool					m_bFade = true;
	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;

	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;
	_float					m_ArrKey[3];
public:
	static CPsyBLExit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;

};

END