#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END
BEGIN(Client)

class CTreasure final:public CGameObject
{
private:
	CTreasure(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTreasure(const CTreasure& rhs);
	virtual ~CTreasure() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void Setting_Count_And_HitCheck();
	void Hit_Check(_double TimeDelta);
	void ScaleMove(_double TimeDelta);

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

	CGameObject*			m_pEffect = nullptr;

	_bool					m_isColController = false;

	_bool					m_isMove = false;
	/* 랜더를 할지 말지 */
	_bool					m_HitCheck = false; /* 충돌되었는가 */
	_double					m_Hit_Delay_Timer = 0.0;
	
	
	_bool				 	m_bMoveScale = false; /* 크기의 변동을 줄것이다. */
	
	_bool					m_bScaleDown = true;
	_bool					m_bScaleEnd = false;
	_bool					m_bScaleUp = false;

	_uint					m_TreasureHP = 2; /* 체력은 2 */
	_double					m_Scale_Timer = 0.0;
	_float4					m_vScaleMove;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

private:
	_int					m_iNavigationIndex = 0;
	//LayerType				m_eLayerType = LayerType::LAYERTYPE_STATIC;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;

private:
	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	_float					m_ArrKey[3];
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	/*For.NormalMapping*/
	_bool					m_bIsNormalMapping = true;

public:
	static CTreasure* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;

};

END
