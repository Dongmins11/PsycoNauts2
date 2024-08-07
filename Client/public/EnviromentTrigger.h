#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CBoxCollider;
class CSphereCollider;
END

BEGIN(Client)

class CEnviromentTrigger : public CGameObject
{
private:
	CEnviromentTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEnviromentTrigger(const CEnviromentTrigger& rhs);
	virtual ~CEnviromentTrigger() = default;

public: 
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	
	const EnviromentTriggerType Get_EnviromentTriggerType() const { return m_eTriggerType; }
	const _matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _float4x4 Get_WorldMatrixFloat4x4() const { return m_WorldMatrix; }
	const _float3 Get_StartTriggerPosition() const { return m_vStartPosition; }
	const _float3 Get_EndTriggerPosition() const { return m_vEndPosition; }
	const _float3 Get_TriggerMoveDir() const { return m_vMoveDir; }
	const _uint		Get_TriggerNum() const { return m_iCnt; }
protected :
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnTriggerStay(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
	CSphereCollider*		m_pSphereColliderCom = nullptr;
#ifdef _DEBUG
	CCollider*				m_pRenderCollider = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos,_float3 vObjScale);
	HRESULT SetUp_ConstantTable();

protected:
	EnviromentTriggerType	m_eTriggerType = EnviromentTriggerType::ETTYPE_END;
	_float4x4				m_WorldMatrix = {};
	_float3					m_vColPos = { 0,0,0 };

	_float3					m_vStartPosition = { 0,0,0 };
	_float3					m_vEndPosition = { 0,0,0 };
	_float3					m_vMoveDir = { 0,0,0 };
	_float3					m_vLookDir = { 0,0,0 };
	_uint					m_iCnt = 0;
public:
	static CEnviromentTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END