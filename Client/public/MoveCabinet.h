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

class CMoveCabinet final:public CGameObject
{
private:
	CMoveCabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoveCabinet(const CMoveCabinet& rhs);
	virtual ~CMoveCabinet() = default;

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

private: /* MoveCeck */
	void MoveFALL();
	void CanbinetCountMove();

	/* 추가 적으로 흔들림 */
	void NextUpMove();
	void NextDownMove();
	void LastUpMove();
	void LastDownMove();
	void NextCanbinetCountMove(_uint RoationCount);
private:/* 확인용*/
	void Check();

public:
	void Set_FallStart() { m_bFall = true; }


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
	_bool					m_bFall = false; /* 쓰러지는거 확인하기 위해서 */
	_bool					m_isMove = false;
	_bool					m_isColController = false;
	_uint					m_iMYObjectCount = 0;

private:/* Moveinfo */
	_float			        Time = 0.f;
	_float			        AccumulationTime = 0.001f;
	_float4			        m_MoveRoation = { 0.f,0.f,0.f,0.f };

	_float			        m_fNextTime[4] = { 0.f,0.f,0.f,0.f };
	_bool					m_bNextMove[4] = { false,false ,false,false };
	_float4			        m_vNextMoveRoation[4] = { _float4(0.f,0.f,0.f,0.f),_float4(0.f,0.f,0.f,0.f),_float4(0.f,0.f,0.f,0.f),_float4(0.f,0.f,0.f,0.f) };
	_float			        m_NextAccumulationTime[4] = { 0.001f,0.002f, 0.001f,0.002f };

	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	_float					m_ArrKey[3];
public:
	static CMoveCabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;

};

END