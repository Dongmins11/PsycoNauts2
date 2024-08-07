#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CBoxCollider;
END

BEGIN(Client)

class CFruitCheckTrigger : public CGameObject
{
private:
	CFruitCheckTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFruitCheckTrigger(const CFruitCheckTrigger& rhs);
	virtual ~CFruitCheckTrigger() = default;

public: 
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	
	const FruitType Get_FruitTriggerType() const { return m_eTriggerFruitType; }
	const _matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _float4x4 Get_WorldMatrixFloat4x4() const { return m_WorldMatrix; }

protected :
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	//virtual void OnTriggerStay(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
#ifdef _DEBUG
	CCollider*				m_pRenderCollider = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos,_float3 vObjScale);
	HRESULT SetUp_ConstantTable();

private:
	_bool Setting_PlatingCounts();
protected:
	FruitType	m_eTriggerFruitType = FruitType::FRUIT_END;
	_float4x4	m_WorldMatrix = {};
	_float3		m_vColPos = { 0,0,0 };
	_uint		m_iCorrectCookingNumber = 0;
	_bool		m_isPlating = false;
public:
	static CFruitCheckTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END