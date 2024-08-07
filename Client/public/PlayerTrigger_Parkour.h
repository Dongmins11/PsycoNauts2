#pragma once
#include "Client_Defines.h"
#include "PlayerTrigger.h"


BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)
class CPlayerTrigger_Parkour : public CPlayerTrigger
{
public:
	enum TRIGGER_LOOK_STATE { LOOK_LEFT, LOOK_RIGHT, LOOK_FRONT, LOOK_BACK, LOOK_END };
private:
	CPlayerTrigger_Parkour(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayerTrigger_Parkour(const CPlayerTrigger_Parkour& rhs);
	virtual ~CPlayerTrigger_Parkour() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:	/* For.Get_Value */
	const _float4 Get_Trigger_Position() const { return m_vPosition; }
	const _float4 Get_Trigger_StartPosition() const { return m_vStartPosition; }
	const _float4 Get_Trigger_EndPosition() const { return m_vEndPosition; }
	const _float3 Get_Trigger_LookDirection() const { return m_vLookDir; }
	const _float3 Get_Trigger_LeftMoveDirection() const { return m_vLookDir; }
	const _float3 Get_Trigger_RightMoveDirection() const { return m_vLeftMoveDir; }
	const _bool	  Get_MoveChecking(TRIGGER_LOOK_STATE eLookState) const { return m_bLookChecking[eLookState]; }
public:
	virtual void Set_Position(_fvector vPos = XMVectorZero());
	virtual void Set_Matrix(const _float4x4* pMatPtr);
protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	//virtual void OnTriggerStay(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;

	const _float4x4*		m_pPlayerTransform = nullptr;
#ifdef _DEBUG
	CCollider*				m_pRenderCollider = nullptr;
#endif // _DEBUG

private:	/* For.TriggerValue */
	_float4					m_vPosition = {};
	_float4					m_vStartPosition = {};
	_float4					m_vEndPosition = {};
	_float3					m_vLeftMoveDir = {};
	_float3					m_vRightMoveDir = {};
	_float3					m_vLookDir = {};

private:	/* For.MoveChecking */
	_bool					m_bLookChecking[LOOK_END] = {false};

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
//	HRESULT SetUp_ConstantTable();

private:
	void	TriggerEnter_ForPlayerLedge(CCollisionObject& collision);
	void	TriggerEnter_ValueReset();

protected:
	// 플레이어용 트리거 타입Enum
	PlayerTriggerType		m_eTriggerType = PlayerTriggerType::PTTYPE_END;
	// 트리거 피봇위치
	_float3					m_vColPos = { 0,0,0 };

public:
	static CPlayerTrigger_Parkour* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END