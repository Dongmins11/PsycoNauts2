#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CGiantCloche final : public CGameObject
{
private:
	enum POSTION {START_POSTION ,ENDING_POSTION ,  POSTION_END}; /* 시작 끝 enum */


private:
	CGiantCloche(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGiantCloche(const CGiantCloche& rhs);
	virtual ~CGiantCloche() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void Setting_Count_And_HitCheck();
	void Player_HitCheck(_double TimeDelta);

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
	void Setting_Move_Postion();
	void Postion_Down_Lerp(_double TimeDelta);
	void Postion_Up_Lerp(_double TimeDelta);
	
public:
	void Set_Giant_Move(_bool bMove) { m_bGiant_Move = bMove; }
	void Set_Giant_MoveUp(_bool bMove){ m_bGiantUp_Move = bMove; }


private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	//CNavigation*			m_pNavigationCom = nullptr;
	//CBoxCollider*			m_pColliderCom = nullptr;
	_bool					m_isColController = false;

	_bool					m_isMove = false;
	/* 랜더를 할지 말지 */

	_uint					m_iMyCount = 0; /* 현재의 나의 Count*/
	_bool					m_RenderInterpolate = false;


private:
	_float3					m_MovePostion[POSTION_END];
	_bool					m_bGiant_Move = false;
	_bool					m_bGiantUp_Move = false;


	_double					m_Start_RenderTimer = 0.0;
	_double					m_End_RenderTimer = 0.0;
	_double					m_AccTimer = 0.0;
	//_double					m_Inter_Timer = 1.0;
	//_double					m_Hit_Delay_Timer = 0.0;

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
	static CGiantCloche* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END