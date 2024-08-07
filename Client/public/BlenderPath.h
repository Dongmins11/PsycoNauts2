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

class CBlenderPath final :public CGameObject
{
private:
	enum Coll{ COLL_UP,  COLL_RIGHT, COLL_DOWN, COLL_LEFT, COLL_END};
private:
	CBlenderPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBlenderPath(const CBlenderPath& rhs);
	virtual ~CBlenderPath() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

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
	void MoveRoation(_double TimeDelta); /* 회전돌아가게 설정 */
	void HITCheck(_double TimeDelta); /* Hit 진행시 무적상태값 설정 */
	void PlayerMoveCheck(_double TimeDelta); /* Player의 움직임을 관리 할것 */

public:
	void Set_PlayerBenderEnter() { m_bTriggerCheck = true; } /* Player가 안으로 들어왔습니다.*/
	void Set_PlayerBenderExit() { m_bTriggerCheck = false; } /* Player가 안에 없습니다 .*/

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	
	CBoxCollider*			m_pColliderCom[COLL_END] = { nullptr ,nullptr , nullptr , nullptr };
	_float3					ColliderPos[COLL_END]; /*콜라이더의 로컬 보정 */
	_uint					m_iMyColliderCount = 0;

	_bool					m_HitCheck = false; /* 충돌되었는가 */
	_double					m_HitTimer = 0.0;

	_bool					m_RoationCheck = { true};
	_double					m_AccTime = 0.0;


	_bool					m_bTriggerCheck = false;
	_float4					m_vMoveRight;
	_float					m_fBlender_RotationMove[2] = { 0.2f, -0.2f }; /* 원통이 돌아가는 속도 입니다. */

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

private: /* 움직임에 필요 */
	_uint					m_iMyCount = 0; /* 현재의 나의 Count*/
	_float4					m_vStartRoation = { 0.f,0.f,0.f,0.f };
	_float				    m_fMove = 0.f;
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

public:
	static CBlenderPath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END

