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

class CAnimatedShelv final:public CGameObject
{
private:
	CAnimatedShelv(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAnimatedShelv(const CAnimatedShelv& rhs);
	virtual ~CAnimatedShelv() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:/* 확인용*/
	void MoveShelv();/* 이동이 가능합니다 */
	void Check();
	void Setting_Lerp();
private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);

private: /* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex);
public:
	void Set_AnimatedShelvStart();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;

	CGameObject*			m_pEffect = nullptr;
private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

private: /* 이펙트 */
	HRESULT Create_Effect();

private:
	_int					m_iNavigationIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;
	_bool					m_isMove = false;
	_bool					m_bMoveShelv = false; /* 찬장 이동 관리 */

	_bool					m_isColController = false;
	_uint					m_iMYObjectCount = 0;


private:
	_float  RoationTime = 0.f;
	_float  RoationAccumulationTime = 0.001f;
	_float4 m_SettingRoation = { 0.f,0.f,0.f,0.f };
	_float4 m_SettingLeupRoation = { 0.f,0.f,0.f,0.f };
	_float4 m_MoveRoation = { 0.f,0.f,0.f,0.f };

	_float  MoveTime = 0.f;
	_float  MoveAccumulationTime = 0.001f;
	_float4 m_SettingPosition = { 0.f,0.f,0.f,0.f }; /* 시작*/
	_float4 m_SettingLeupPosition = { 0.f,0.f,0.f,0.f }; /* 끝 */
	_float4 m_MovePostion = { 0.f,0.f,0.f,0.f };

	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;
	_float					m_ArrKey[3];

	/* For. Effect */
	_bool	m_isEffectCreate = false;

	/* For. Dissolve*/
	_double m_DissolveTimeAcc = 0.0;
	_double m_DissolveDelay = 0.4;
	
public:
	static CAnimatedShelv* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;
};

END